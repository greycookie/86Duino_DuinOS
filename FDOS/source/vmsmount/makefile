#
# VMSMOUNT
#  A network redirector for mounting VMware's Shared Folders in DOS 
#  Copyright (C) 2011  Eduardo Casino
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA  02110-1301, USA.
#
# 2011-10-01  Eduardo           Add UPX compression
# 2011-10-05  Tom Ehlert        Use Pentium optims for smaller code
# 2011-10-14  Eduardo           Back to -3 optims as they generate the same
#                               code. Use -0 for main.c and kitten.c to allow
#                               execution of processor test.
#                               Reorder segments to mark end of transient part
# 2011-11-01  Eduardo           Add LFN (new object and segment order)
#

CC = wcc
AS = nasm
LD = wlink
UPX = upx
RM = rm -f
CFLAGS  = -bt=dos -ms -q -s -oh -os -DREVERSE_HASH
ASFLAGS = -f obj -Worphan-labels -O9
LDFLAGS =	SYSTEM dos \
			ORDER \
				clname CODE segment BEGTEXT segment _TEXT segment ENDTEXT \
				clname FAR_DATA clname BEGDATA clname DATA clname BSS \
				clname STACK \
			OPTION QUIET \
			OPTION MAP=vmsmount.map
LDFLAGS2 = SYSTEM dos OPTION QUIET OPTION MAP=vmsmount.map
UPXFLAGS = -9

TARGET = vmsmount.exe

OBJ =	kitten.obj vmaux.obj main.obj miniclib.obj vmint.obj unicode.obj \
		vmdos.obj vmcall.obj vmtool.obj vmshf.obj redir.obj lfn.obj \
		endtext.obj


all: $(TARGET)

clean:
	$(RM) $(OBJ) $(TARGET)

$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) NAME $(TARGET) FILE {$(OBJ)} $(LIBPATH) $(LIBRARY)
	$(UPX) $(UPXFLAGS) $(TARGET)

# main.obj and kitten.obj must be compiled with 8086 instructions only to gracefully
#  execute the processor check in real, older machines
#
main.obj: main.c
	$(CC) -0 $(CFLAGS) -fo=$@ $<

kitten.obj: kitten.c
	$(CC) -0 $(CFLAGS) -fo=$@ $<

kitten.obj: kitten.h

main.obj: globals.h kitten.h messages.h vmaux.h vmshf.h vmtool.h dosdefs.h redir.h unicode.h endtext.h

vmcall.obj: vmcall.h 

vmaux.obj: vmcall.h vmtool.h globals.h messages.h vmshf.h kitten.h

redir.obj: globals.h redir.h dosdefs.h vmshf.h vmtool.h vmcall.h vmdos.h vmint.h miniclib.h

vmtool.obj: vmtool.h vmcall.h

vmshf.obj: vmtool.h vmshf.h vmcall.h vmint.h vmdos.h redir.h miniclib.h globals.h

vmdos.obj: vmint.h dosdefs.h vmdos.h vmint.h vmshf.h vmtool.h vmcall.h miniclib.h unicode.h redir.h globals.h

lfn.obj: lfn.h globals.h miniclib.h vmdos.h

%.obj : %.c
	$(CC) -3 $(CFLAGS) -fo=$@ $<

%.obj : %.asm
	$(AS) $(ASFLAGS) -o $@ $<
	
