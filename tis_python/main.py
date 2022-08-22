import serial
import ctypes
import tisgrabber as tis
import numpy as np
import logging
from math import prod
from threading import Lock

logging.basicConfig(format='%(asctime)s [%(levelname).1s] %(message)s', level=logging.DEBUG)

EXPOSURE = 0.5

PROCESSING_LOCK = Lock()


# global_imgs = []

# def soft_trigger():
#     raise NotImplementedError

class CallbackUserdata(ctypes.Structure):
    """ Example for user data passed to the callback function.
    """

    def __init__(self):
        self.width = ctypes.c_long()
        self.height = ctypes.c_long()
        self.bitsPerPixel = ctypes.c_int()
        self.colorFormat = ctypes.c_int()
        self.images = []
        super().__init__()

    def set_params(self, ic, hGrabber):
        # Query the values of image description
        ic.IC_GetImageDescription(hGrabber, self.width, self.height,
                                  self.bitsPerPixel, self.colorFormat)


def capture_callback(_, pBuffer, framenumber, pData: CallbackUserdata):
    with PROCESSING_LOCK:
        arduino.write(b'N')
        logging.debug('get into callback')
        logging.debug('raw %d, %d', pData.bitsPerPixel.value, pData.colorFormat.value)
        assert pData.bitsPerPixel.value == 24
        assert pData.colorFormat.value == 1
        shape = (pData.height.value, pData.width.value, 3)
        pBuffer = ctypes.cast(pBuffer, ctypes.POINTER(ctypes.c_ubyte * prod(shape)))
        image = np.ndarray(buffer=pBuffer.contents, dtype=np.uint8, shape=shape)
        logging.debug("got image shape: %s", shape)
        pData.images.append(image[:, :, 0][:, ::-1].copy())


def capture(ic, grabber, save=None):
    if ic.IC_SnapImage(grabber, 2000) == tis.IC_SUCCESS:
        # Declare variables of image description
        Width = ctypes.c_long()
        Height = ctypes.c_long()
        BitsPerPixel = ctypes.c_int()
        colorformat = ctypes.c_int()

        # Query the values of image description
        ic.IC_GetImageDescription(grabber, Width, Height,
                                  BitsPerPixel, colorformat)

        # Calculate the buffer size
        assert BitsPerPixel.value == 16
        assert colorformat.value == tis.SinkFormats.Y16
        buffer_size = Width.value * Height.value * BitsPerPixel.value

        # Get the image data
        imagePtr = ic.IC_GetImagePtr(grabber)

        imagedata = ctypes.cast(imagePtr, ctypes.POINTER(ctypes.c_ubyte * buffer_size))

        # Create the numpy array
        image = np.ndarray(buffer=imagedata.contents,
                           dtype=np.uint16,
                           shape=(Height.value, Width.value))
        if save is not None:
            np.save(f'{save}.npy', image)
            return True
        else:
            return image
    else:
        logging.warning("No frame received in 2 seconds.")


def main():
    ic = tis.loadLib()
    hGrabber = tis.openDevice(ic)
    userdata = CallbackUserdata()
    frameReadyCallbackfunc = ic.FRAMEREADYCALLBACK(capture_callback)
    try:
        if not ic.IC_IsDevValid(hGrabber):
            ic.IC_MsgBox(b"No device opened", b"Auto capturing")
        else:
            ic.IC_SetContinuousMode(hGrabber, 0)
            ic.IC_SetPropertySwitch(hGrabber, b"Trigger", b"Enable", 1)

            ic.IC_SetPropertySwitch(hGrabber, b"Exposure", b"Auto", 0)
            ic.IC_SetPropertyAbsoluteValue(hGrabber, b"Exposure", b"Value", ctypes.c_float(EXPOSURE))
            ic.IC_SetFrameReadyCallback(hGrabber, frameReadyCallbackfunc, userdata)
            # ic.IC_SetFormat(hGrabber, 3)
            ic.IC_StartLive(hGrabber, 0)
            userdata.set_params(ic, hGrabber)
            # while capture(ic, hGrabber) is None:  # successfully capture -> initialized
            #     pass

            num = 1
            arduino.write(b'N')
            while True:
                out = arduino.readline()
                logging.debug('raw serial data: %s', out)
                match out[:3]:
                    case b'[N]':
                        # logging.debug("capture start %d", num)
                        # images.append(capture(ic, hGrabber))
                        # logging.debug("capture end %d", num)

                        ic.IC_PropertyOnePush(hGrabber, b"Trigger", b"Software Trigger")
                        logging.debug("trigger sent")
                        num += 1
                    case b'[F]':  # finished
                        with PROCESSING_LOCK:
                            return np.stack(userdata.images)
                    case b'[D]':  # debugging info
                        logging.info("arduino says: %s", out[3:])
                    case _:
                        logging.error("get invalid content: %s", out)
                        return
    finally:
        ic.IC_StopLive(hGrabber)
        ic.IC_ReleaseGrabber(hGrabber)
        arduino.close()
        logging.info('finished')


if __name__ == '__main__':
    arduino = serial.Serial('COM3', timeout=3)  # 3s timeout
    imgs = main()
