import ctypes
import logging
import time
from math import prod
import os
from threading import Lock

import serial
import tisgrabber as tis
import numpy as np
import data

logging.basicConfig(format='%(asctime)s [%(levelname).1s] %(message)s', level=logging.DEBUG)

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

    def get_image_params(self, ic, hGrabber):
        # Query the values of image description
        ic.IC_GetImageDescription(hGrabber, self.width, self.height,
                                  self.bitsPerPixel, self.colorFormat)


def capture_callback(_, pBuffer, framenumber, pData: CallbackUserdata):
    with PROCESSING_LOCK:
        arduino.write(b'N')
        logging.debug('get into callback')
        logging.debug('raw %d, %d', pData.bitsPerPixel.value, pData.colorFormat.value)
        # 16 bit output
        BIT_DEPTH = 16
        CHANNELS = 1
        assert pData.bitsPerPixel.value == BIT_DEPTH * CHANNELS
        assert pData.colorFormat.value == tis.SinkFormats.Y16
        shape = (pData.height.value, pData.width.value, CHANNELS)
        dtype = {8: np.uint8, 16: np.uint16}[BIT_DEPTH]
        pBuffer = ctypes.cast(pBuffer, ctypes.POINTER(ctypes.c_ubyte * prod(shape) * (BIT_DEPTH // 8)))
        # note: use buffer without copy may overwrite previous data
        image = np.ndarray(buffer=pBuffer.contents, dtype=dtype, shape=shape).squeeze().copy()
        logging.debug("got image shape: %s", shape)
        pData.images.append(image)


def main(exposure=4.):
    ic = tis.loadLib()
    # hGrabber = tis.openDevice(ic)
    hGrabber = ic.IC_LoadDeviceStateFromFile(None, tis.T("devicedmk.xml"))
    userdata = CallbackUserdata()
    frameReadyCallbackfunc = ic.FRAMEREADYCALLBACK(capture_callback)
    try:
        if not ic.IC_IsDevValid(hGrabber):
            ic.IC_MsgBox(b"No device opened", b"Auto capturing")
        else:
            ic.IC_SetVideoFormat(hGrabber, b"Y16 (4504x4504)")
            ic.IC_SetFormat(hGrabber, tis.SinkFormats.Y16)
            ic.IC_SetContinuousMode(hGrabber, 0)
            ic.IC_SetPropertySwitch(hGrabber, b"Trigger", b"Enable", 1)
            ic.IC_SetPropertySwitch(hGrabber, b"Exposure", b"Auto", 0)
            ic.IC_SetPropertyAbsoluteValue(hGrabber, b"Exposure", b"Value", ctypes.c_float(exposure))
            # parameters should be set before this line (IC_SetFrameReadyCallback)
            ic.IC_SetFrameReadyCallback(hGrabber, frameReadyCallbackfunc, userdata)
            ic.IC_StartLive(hGrabber, 0)
            userdata.get_image_params(ic, hGrabber)
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
    imgs = main(exposure=0.5)
