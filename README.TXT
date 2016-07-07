============================================================================
                   ___   __   ____        _
                  ( _ ) / /_ |  _ \ _   _(_)_ __   ___
                  / _ \| '_ \| | | | | | | | '_ \ / _ \
                 | (_) | (_) | |_| | |_| | | | | | (_) |
                  \___/ \___/|____/ \____|_|_| |_|\___/
============================================================================


                             DuinOS v1.2.1

                       README.TXT -- December 2015

INTRODUCTION
------------

DuinOS is a DOS operating system for 86Duino, which provides a convenient 
environment for developing DOS programs running on 86Duino.

DuinOS includes the complete FreeDOS, special system-tools, and an 86Duino 
software development kit to complie and execute 86Duino programs. It also 
provides special DOS shell programs to show DOS shell via the com port or
USB device port; the user can type DOS commands in these DOS shells on
86Duino without a VGA card (especially useful for 86Duino Zero/EduCake).
Source codes of most special system-tools are also in DuinOS, and the user
can hack and re-compile these tools.


SOFTWARE INCLUDED
-----------------

    *FreeDOS 1.1
    *DJGPP 2.05
    *FreeBASIC 1.03.0
    *OpenWATCOM 1.9
    *86Duino DOS SDK
    *86Duino DOS system tools
    *DOS device drivers for Vortex86EX
    *FreeRTOS 8.2.2
    *WinCE 6.0


CHANGELOG
-----------
v1.1.0 - add OpenWATCOM, FreeRTOS, WinCE, 86ADC, 86CLOCK, 86CRSBAR, 86INFO, 86IO
v1.2.0 - update DJGPP from 2.05deta to 2.05
v1.2.1 - update gcc from 5.3.0 to 6.1.0, update gpp from 5.3.0 to 6.1.0, update make from 4.1 to 4.2.1, update gdb from 7.10.1 to 7.11


*For more details, see the wbsite: http://www.86duino.com/
