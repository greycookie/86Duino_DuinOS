============================================================================
                   ___   __   ____        _
                  ( _ ) / /_ |  _ \ _   _(_)_ __   ___
                  / _ \| '_ \| | | | | | | | '_ \ / _ \
                 | (_) | (_) | |_| | |_| | | | | | (_) |
                  \___/ \___/|____/ \____|_|_| |_|\___/
============================================================================


                           86Duino DOS SDK v1.00

                        README.TXT -- September 2015

INTRODUCTION
------------

The 86Duino DOS SDK supports the C/C++ language; the user can include 
"Arduino.h" to call all 86Duino API (ex. digitalWrite) listed in the
86Duino Language Reference (http://www.86duino.com/?page_id=2255).


HOW TO COMPILE
--------------

Copy the Makefile in the example folder and paste it to your program folder 
that you want to compile, and then type "make" command to compile source
code to produce your 86Duino program (if you want to use 86Duino libraries,
see the following "USE THIRD-PARTY LIBRARIES" section).


USE THIRD-PARTY LIBRARIES
-------------------------

If you need to use in your program 86Duino libraries listed in the 86Duino 
Libraries Reference (http://www.86duino.com/?page_id=2257), please modify
the Makefile as follows:

  Add the names of the libraries that you want to use behind the 
  "THIRD_LIB_NAME" variable (ex. to add the TimerOne library to compile, you 
  need to modify like "THIRD_LIB_NAME := TimerOne" in Makefile). Since some 
  libraries may depend on other libraries, the names of those libraries must 
  also be added behind the variable "THIRD_LIB_NAME". See APPENDIX.1 for a
  list that describes the relation of libraries. 

  In addition, some libraries may employ internal libraries installed in 
  the DJGPP directory, and hence you must specify those internal 
  libraries (by "-l" + "library name") behind the variable "EXTEN_LIB". 
  See APPENDIX.2 for a list that describes what to add behind the EXTEN_LIB.

APPENDIX 1
==========

Adafruit_CC3000:  SPI
FreeIMU1:         Wire
Mirf:             SPI
RadioHead:        SPI
RF24:             SPI
SpiderL3S:        SPI
TFT:              SPI

APPENDIX 2
==========

Ethernet:     -lswsk
Rosserial86:  -lswsk
USBHost:      -lalleg


HISTORY
-------
1.00 - Support all API and libraries that 86Duino Coding 210 supports
