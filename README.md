# yidecoder

A high-speed barcode decoder for embedded system. The codes are used in a QR-payment-code scanner based on STM32F407VET6.

It supports QR codes and most of the common-used 1D codes.

The codes are in /YiDecoder/yidec. The main.cpp and the vs project are used for testing the codes on computer.

Dependencies:

quirc (https://github.com/dlbeer/quirc) (The required files have been copied to /YiDecoder/quirc)

opencv (Only used in main.cpp for testing the codes on computer)
