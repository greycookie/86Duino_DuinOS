Flashrom

Flashrom is a program for saving and updating your BIOS firmware,
provided you have an EEPROM chip on which it is stored.

Saving your system BIOS to a file usually goes fine,
erasing and writing can be dangerous steps, don't perform them
if you cannot afford a non-working machine.

WARNING #1: USING THIS PROGRAM CAN MAKE YOUR SYSTEM NOT WORK ANYMORE!!!

WARNING #2: This is not a generic BIOS dumping program, it's intended
to only work on systems having an EEPROM.
This means you're out of luck anyway on the following platforms:
* anything below a Pentium processor (so no 8086/80186/286/386/4886)
* VMware and other emulators (despite bios440.rom = myfile.bin)
* Maybe UniFlash has more luck for these situations.

WARNING #3: Using this program on GNU/Linux distributions like
            Parted Magic might be more beneficial due to online
            help requests (using email, IRC etc) being possible.

WARNING #4: Don't even think about updating BIOS on laptops using
            this program. Due to special/custom chips/controllers for 
            supporting events and hardware buttons things often
            go wrong. Stick to vendor tools

Not all chips and chipsets are supported yet even if you do have a
system with EEPROM to write to.
Things you can write to this chip:
* UEFI (Universal Extensible Firmware Interface, written in C )
* BIOS (Supplied by vendor, ASM-based on Award/Phoenix or AMI code)
* Coreboot (written in C). Additional layer SeaBIOS helps to run DOS

Other things that can be used for flashing are an extended collection of
add-in cards (usually mass-storage controllers and network interface cards)
as well as some external (LPT/RS232/USB) EEPROM programmers.
Please check http://www.flashrom.org for more details.


Reading is reasonably safe. Explicitly erasing might work unconditionally.
Writing is hopefully implemented as:
0) Decompress new BIOS file (if compressed BIOS files supported)
1) read new content (BIOS file from USB stick for example) in memory
2) verify against read source (BIOS file from USB stick)
3) read current contents from EEPROM into memory, optionally to disk as backup
4) verify read contents against EEPROM
5) compare new content with current content
6) write new contents if it differs from current content
7) verify written content
8) if failed, try to write new file for a 2nd time
9) verify 2nd attempt against memory contents
8) write previously saved contents to EEPROM if verify failed twice (restore backup)
9) verify backup was written correctly

Expected memory usage:
- 1.0 * 6MB for FlashROM itself, as compiled for DJGPP with CWSDPMI stub
- 0.5 * [EEPROM size] in case of compressed new BIOS file
- 1.0 * [EEPROM size] for reading current EEPROM size (1MB for example)
- 1.0 * [EEPROM size] for uncompressed new BIOS file  (1MB for example)
-------------------------------------------------------------------------
total: 6MB + 2.0 to 2.5 times EEPROM size

In case of any issues please notify the flashrom team by email at:
flashrom@flashrom.org

Minimum platform  : sourcecode written/compiled for 80686 (Pentium Pro) or later
Pre-minimum-safe? : no idea, hope so. Anyone got an 8086 to run this program?


Other ideas:
* Flashrom should echo the commandline with which it was invoked in its verbose log
* SeaBIOS supports uncompressed DOS floppy images, have a utility to replace them
  from a CoreBoot/FlashROM firmware image.
* SeaBIOS supports compressed DOS floppy images. Hopefully also image can be padded
  to next floppy size. Have a DOS util for compressing floppy image and pad it.
* both ideas should allow to update the functionality of the DOS bootdisk image
  without the necessity of (booting into Linux and) recompiling Coreboot.

Document created by Bernd Blaauw for FreeDOS 1.1 release