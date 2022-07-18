import serial
import ctypes
import tisgrabber as tis
import numpy as np

ic = tis.loadLib()

hGrabber = tis.openDevice(ic)
arduino = serial.Serial('COM3', timeout=1)  # 1s timeout


def capture(ic):
    if ic.IC_SnapImage(hGrabber, 2000) == tis.IC_SUCCESS:
        # Declare variables of image description
        Width = ctypes.c_long()
        Height = ctypes.c_long()
        BitsPerPixel = ctypes.c_int()
        colorformat = ctypes.c_int()

        # Query the values of image description
        ic.IC_GetImageDescription(hGrabber, Width, Height,
                                  BitsPerPixel, colorformat)
        print(Width.value, Height.value, BitsPerPixel.value, colorformat.value)

        # Calculate the buffer size
        bpp = int(BitsPerPixel.value / 8.0)
        buffer_size = Width.value * Height.value * BitsPerPixel.value

        # Get the image data
        imagePtr = ic.IC_GetImagePtr(hGrabber)

        imagedata = ctypes.cast(imagePtr,
                                ctypes.POINTER(ctypes.c_ubyte *
                                               buffer_size))

        # Create the numpy array
        image = np.ndarray(buffer=imagedata.contents,
                           dtype=np.uint8,
                           shape=(Height.value,
                                  Width.value,
                                  bpp))
    else:
        print("No frame received in 2 seconds.")

if not ic.IC_IsDevValid(hGrabber):
    ic.IC_MsgBox(tis.T("No device opened"), tis.T("Simple Live Video"))
else:
    try:
        ic.IC_StartLive(hGrabber, 1)
        finish = False
        arduino.write(b'N')
        while not finish:
            out = arduino.readline()
            if out[:3] == b'[N]':
                capture(ic)
                arduino.write(b'N')
            elif out[:3] == b'[F]':
                finish = True
            elif out[:3] == b'[D]':
                print(out)
            else:
                print(f'get invalid content: {out}')
                finish = True
    finally:
        ic.IC_StopLive(hGrabber)
        ic.IC_ReleaseGrabber(hGrabber)
