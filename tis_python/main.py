import serial
import ctypes
import tisgrabber as tis
import numpy as np
import logging
import time

logging.basicConfig(format='%(asctime)s [%(levelname).1s] %(message)s', level=logging.DEBUG)

EXPOSURE = 0.5

def soft_trigger():
    raise NotImplementedError

def capture_callback(_, pBuffer, framenumber, pData):
    print(pData.BytesPerPixel, pData.height)
    image = np.ndarray(buffer=pBuffer.contents,
                       dtype=np.uint16,
                       shape=(pData.height.value, pData.width.value))
    print(image.shape)


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
    arduino = serial.Serial('COM3', timeout=1)  # 1s timeout
    images = []

    if not ic.IC_IsDevValid(hGrabber):
        ic.IC_MsgBox(b"No device opened", b"Auto capturing")
    else:
        try:
            ic.IC_SetFormat(hGrabber, tis.SinkFormats.Y16.value)
            ic.IC_SetPropertySwitch(hGrabber, b"Exposure", b"Auto", 0)
            ic.IC_SetPropertyAbsoluteValue(hGrabber, b"Exposure", b"Value", ctypes.c_float(EXPOSURE))
            ic.IC_StartLive(hGrabber, 1)
            while capture(ic, hGrabber) is None:  # successfully capture -> initialized
                pass
            num = 1
            arduino.write(b'N')
            while True:
                out = arduino.readline()
                logging.debug('raw serial data: %s', out)
                match out[:3]:
                    case b'[N]':
                        logging.debug('capture start %d', num)
                        images.append(capture(ic, hGrabber))
                        logging.debug('capture end %d', num)
                        num += 1
                        arduino.write(b'N')
                    case b'[F]':  # finished
                        return np.stack(images)
                    case b'[D]':  # debugging info
                        logging.info('arduino says: %s', out[3:])
                    case _:
                        logging.error(f'get invalid content: %s', out)
        finally:
            ic.IC_StopLive(hGrabber)
            ic.IC_ReleaseGrabber(hGrabber)
            arduino.close()
            logging.info('finished')


if __name__ == '__main__':
    imgs = main()
