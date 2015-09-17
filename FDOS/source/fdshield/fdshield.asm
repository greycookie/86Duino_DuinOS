; FDSHIELD virus shield - by Eric Auer 2004-2006 eric@coli.uni-sb.de
; To compile: http://nasm.sf.net/ "nasm -o fdshield.com fdshield.asm"

 ; FDSHIELD is free software; you can redistribute it and/or modify
 ; it under the terms of the GNU General Public License as published
 ; by the Free Software Foundation; either version 2 of the License,
 ; or (at your option) any later version.

 ; FDSHIELD is distributed in the hope that it will be useful,
 ; but WITHOUT ANY WARRANTY; without even the implied warranty of
 ; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ; GNU General Public License for more details.

 ; You should have received a copy of the GNU General Public License
 ; along with FDSHIELD; if not, write to the Free Software Foundation,
 ; Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 ; (or try http://www.gnu.org/licenses/licenses.html at www.gnu.org).


; ------------------------------------


; History:
;
; 10jun2004 initial release
;
; 04jul2004 fixed int 2f.13 trick, added Windows detection
;
; 15mar2005 fixed int 21.3d exe protection, improved Windows
;           detection, added cache flush for /w and /W options,
;           added int 21.16 / 21.3c "...replace" exe protection,
;           added int 21.6c "ext open" exe protection, added
;           int 21.56 rename-to/from-exe protection. Without that,
;           e.g. UPX would still be able to modify exe files.
;           added int 21.5b "create but not replace" exe protection.
;           PROBLEM: UPX is allowed to DELETE exe files, so attempts
;           to UPX/DE-UPX exe files result in their deletion. Had to
;           add exe NAME del protection as well because of that...
; 18mar2005 Added FCB delete / rename block (21.13, 21.17), modified
;           message system (add "Attempt to" prefix shortcut),
;           added FASCIST_EXEPROT define to prevent exe creation.
; 19mar2005 Fixed "check wrong pointers" for FCB and ext open,
;           added more support for extended FCBs. Optimized ifexe.
;           Added /X switch for FASCIST_EXEPROT, updated help text.
;           FASCIST_EXEPROT now also protects bat files and is
;           enabled by default. Added "Enabled ..." feedback.
; 23mar2005 Added FCB / filename / PSP information display.
;           FDSHIELD is now 3k in RAM and 3.3k (UPXed) on disk.
; 25mar2005 Added device chain checking and exception for RTM from
;           the TSR blocking function.
; 26mar2005 Added exception for CWSDPMI from the TSR blocking and
;           made both exceptions depend on the new /T switch. Or is
;           it better to have a switch "allow only RTM", as CWSDPMI
;           is not needed at all in DOS boxes, DOSEMU, OS/2 etc.?
;           Both extenders in one switch is clearer for now :-P.
;           Reorganized the help screen. Now optionally with ANSI.
; 31aug2006 Replaced all RETF +2 by the ncIRET and cyIRET code
;           which only clears/sets the caller carry flag but pre-
;           serves all other caller flags like "interrupt enable".


; ------------------------------------


; Known problems:
; * LFN style access is not intercepted by /x (or /X) option.
;   Unicode can be tricky. Maybe just block all LFN access???
; * MS DOS shell uses FCB delete, so you cannot use del any.*
;   or any.tx? or similar while FDSHIELD is loaded, as it always
;   (not configurable) rejects wildcards in file name extensions
;   for the FCB delete function. One of the reasons for that is
;   that "del *.*" would *drop* subdirectories according to RBIL.
;   Comment from Arkady: FCB delete is the only fast mass-delete.

%define ANSI 1		; set to 0 if you want ANSI colors in /? help
%if ANSI		; Disadvantage: resets your screen colors.
%define HIANSI db 27,"[1m",13,	; set BOLD color and move cursor left
%define LOANSI db 27,"[0m",13,	; set PLAIN color and move cursor left
%else
%define HIANSI 			; nothing
%define LOANSI 			; nothing
%endif

; ------------------------------------


	; FDSHIELD feature flag definitions:

	; 1 (hd format block, always on in FDSHIELD)
%define BEFASCIST 1	; make WRITEEXE checks very picky * != VSAFE *
			; only useful if FASCIST_EXEPROT is enabled
			; adds bat files to the set of protected files
%define TSRBLOCK  2	; halt system if program going TSR
%define WRITEHARD 4	; make harddisk / ramdisk (!?) writes fail
	; 8 (exe scan)
%define WRITEFLOP 8	; make floppy write fail  * != VSAFE *
	; WRITEHARD and WRITEFLOP together trigger the extra feature
	; "simulate readonly attribute for all files / dirs"...!
	; 16 (boot scan)
%define VERBOSE  16	; show more messages      * != VSAFE *
%define BOOTHARD 32	; make harddisk boot write fail
%define BOOTFLOP 64	; make floppy boot write fail
%define WRITEEXE 128	; prevent com/sys/exe file modifications
			; and deletion (LFN access not blocked!)

%define FASCIST_EXEPROT 1	; define to enable strong exe protect
; without that, exe creation is allowed for: "create but not replace"
; (21.5b), "ext. open to create but not replace" (21.6c) and by
; renaming files (name, 21.56, or fcb, 21.17). Programs which create
; exe files by more suspicious functions like 21.3c are out of luck.
; FASCIST_EXEPROT is activated by the /X command line switch, and it
; adds BAT files to the set of protected files.


; ------------------------------------
; ------ START OF RESIDENT PART ------
; ------------------------------------


	org 0x100
%define DEADBEEF	0xdeadbeef	; for "dd ?" for pointers

start:
flags:			; just let the config flags overwrite this
	jmp install

; ------------------------------------

old13	dd DEADBEEF
i13:	jmp far [cs:X13ptr]
	; The i13 -> extra13 is used for int 2f.13 compatibility:
	; programs can tell "DOS" (us) to use some other handler
	; instead of extra13 temporarily. In turn, we tell those
	; programs that the "real" handler would be extra13. So
	; when they try to bypass "DOS", they will still use our
	; extra13 handler (and bypass only our i13) ;-).

	; The normal way of doing things would be telling programs
	; that old13 is the real handler and allowing them to
	; change our stored old13 pointer. But then we would allow
	; them to really bypass us.

extra13:
	; hook: fa (vsafe), 5 (format), 3 (write), 43 (lbawrite)
	; [NOT YET: 2 (read), 42 (lbaread)
	call dummy_vshield
	call dummy_vshield2
	call tbdriver_tunnelcheck
	mov word [cs:stinkcode],0x13
	call vsafe_api

	mov word [cs:stinkptr],bootfmsg
	test dl,0x80	; floppy or harddisk?
	jz i13msgf
	mov word [cs:stinkptr],boothmsg

	cmp ah,5	; formatting harddisk?
	jnz i13msgf
	mov word [cs:stinkptr],formatmsg
	call WARNINGif
	jmp short i13fail	; format harddisk always fails

i13msgf:
	cmp ah,3	; write?
	jz i13write
	cmp ah,0x0b	; write long?
	jz i13write
	cmp ah,0x43	; LBA write?
	jz i13writelba

i13chain:
	jmp far [cs:old13]


i13write:		; CHS write
	test dl,0x80	; floppy or harddisk?
	jz bf_flag
	test byte [cs:flags],BOOTHARD
	jmp short bh_flag
bf_flag:
	test byte [cs:flags],BOOTFLOP
bh_flag:
	jz i13noboot
	cmp cx,1
	jnz i13noboot
	cmp dh,0
	jnz i13noboot
i13boot:
	call WARNINGif
i13fail:
	mov ax,0x0300	; write protected, 0 transferred
	jmp cyIRET	; return and set carry

i13noboot:
	mov word [cs:stinkptr],wrprotmsg
	test dl,0x80	; floppy or harddisk?
	jz wf_flag
	test byte [cs:flags],WRITEHARD
	jmp short wh_flag
wf_flag:
	test byte [cs:flags],WRITEFLOP
wh_flag:
	jz i13chain
	jmp short i13boot	; was: i13fail


i13writelba:		; LBA write
	mov word [cs:stinkptr],wrprotmsg
	test dl,0x80	; floppy or harddisk?
	jz bfl_flag
	test byte [cs:flags],BOOTHARD
	jmp short bhl_flag
bfl_flag:
	test byte [cs:flags],BOOTFLOP
bhl_flag:
	jz i13nobootl
	push ax
	xor ax,ax
	or ax,[ds:si+8]		; sector number low low
	or ax,[ds:si+10]	; sector number low
	or ax,[ds:si+12]	; sector number high
	or ax,[ds:si+14]	; sector number high high
	pop ax	
	jnz i13nobootl		; LBA sector number 0: boot sector
i13bootl:
	call WARNINGif
	jmp i13fail

i13nobootl:
	test dl,0x80	; floppy or harddisk?
	jz wfl_flag
	test byte [cs:flags],WRITEHARD
	jmp short whl_flag
wfl_flag:
	test byte [cs:flags],WRITEFLOP
whl_flag:
	jnz i13bootl		; was: i13faill
	jmp i13chain

; ------------------------------------

old16	dd DEADBEEF
i16:	; hook: fa (vsafe), ...
	call dummy_vshield
	call dummy_vshield2
	call tbdriver_tunnelcheck
	cmp ah,0xfa	; VSAFE / VWATCH?
	jnz i16chain
	mov word [cs:stinkcode],0x16
	call vsafe_api
i16chain:
	jmp far [cs:old16]

; old19	dd DEADBEEF
; i19:	; hook? [NOT YET: (warmboot)]
; old20	dd DEADBEEF
; i20:	; hook? [NOT YET: (exit)]

; ------------------------------------

old21	dd DEADBEEF
i21:	; hook: fa (vsafe), 31 (TSR), 7305 (SI odd: write),
	;       43 (get attrib), 4e/4f (findfirst/findnext attr)
	;       3d (open), 0f (FCBopen). [NOT YET: LFN stuff!]
	;       [NOT YET: 4b (exec), 4c (exit), write (40, 15, 22)]
	; NEW 3/2005: 16 (FCBcreate/truncate), 3c (create/truncate),
	;       6c ext open, 56 rename, [NOT: 5b create,] 41 delete
	call dummy_vshield
	call dummy_vshield2
	call tbdriver_tunnelcheck
	mov word [cs:stinkcode],0x21
	call vsafe_api

	cmp ah,0x31	; TSR?
	jnz i21notsr
tsrallow:
	jmp short i21tsrfirst	; FIRST call to int 21.31: no penalty

	test byte [cs:flags],TSRBLOCK
	jnz i21tsrpanic		; If blocker on: check if this TSR is
				; allowed, and if not, panic.
	call checkRTM		; returns CY set if the TSR is RTM
	jc i21tsrrtm		; stay calm, it's only RTM...
				; Is it? You never know in DOS :-P
	mov word [cs:stinkptr],tsrmsg
	call WARNINGif

i21chain:
	jmp far [cs:old21]

i21tsrrtm:
	test byte [cs:flags],VERBOSE
	jz i21chain
	push si
	mov si,rtmmsg
	call strtty		; tell that we detected RTM
	pop si
	call regdump		; show register / PSP info
	jmp short i21chain

i21tsrfirst:
	mov byte [cs:tsrallow+1],0	; LATER int 21.31 do count
	jmp short i21notsr

i21tsrpanic:
	call checkRTM		; returns CY set if the TSR is RTM
	jc i21tsrrtm		; don't panic...
	jmp tsrpanic

i21notsr:
	test byte [cs:flags],TSRBLOCK
	jz i21nodevcheck
	cmp ah,0x3d		; check on "open file"
	jz i21devcheck
	cmp ah,0x4c		; check on "exit"
	jz i21devcheck
	cmp ah,0x4d		; check on "read errorlevel"
	jnz i21nodevcheck
i21devcheck:			; slow, so do not do it too often:
	call checkdevices	; look for device chain tampering

i21nodevcheck:
	push ax
	mov al,[cs:flags]	; all drives protected -> "all R/O"
	and al,WRITEHARD + WRITEFLOP
	cmp al,WRITEHARD + WRITEFLOP
	pop ax
	jnz i21noattrib
	cmp ax,0x4300	; get attrib?
	jnz i21noattrib43
	pushf
	cli
	call far [cs:old21]	; call original handler
	jc i21attrbug
	or cx,1		; pretend write protection being on
	jmp ncIRET	; return and clear carry
i21attrbug:
	jmp cyIRET	; return and set carry

i21noattrib43:
	cmp ah,0x4e	; findfirst?
	jb i21noattrib
	cmp ah,0x4f	; findnext?
	ja i21noattrib
	pushf
	cli
	call far [cs:old21]	; call original handler
	jc i21attrbug

	push es
	push bx
	mov ah,0x2f	; get DTA address
	pushf
	cli
	call far [cs:old21]	; call original handler
	; * Data written to Disk Transfer Area - PSP:80 unless
	; * modified by 21.1a to be ds:dx ... 21.2f gets current
	; * DTA pointer to  ES:BX
	or byte [es:bx+0x15],1	; pretend write protection on
	pop bx
	pop es
	jmp ncIRET	; return and clear carry

i21okay2:
	jmp i21okay

i21noattrib:
	cmp ah,0x13		; FCB delete? (with wildcards!)
	jz i21fcbdel
	test byte [cs:flags],WRITEEXE ; any exe protection active?
	jnz do_exeprot		; else skip all checks here
	jmp i21noexeprot

i21fcbdel:
	mov word [cs:stinkptr],fcbdelmsg	; wildcard delete
	; Always on, as FCB deletion of "???????????" would just
	; drop the directory and subdirs not doing it file by file!
	; Being overly picky (and lazy), we just block "any.*" del.
	push ax
	push bx
	mov bx,dx		; FCB is at DS:DX
	cmp byte [ds:bx],0xff	; ext FCB?
	jnz normfcbdel
	add bx,7		; ext FCB has 7 bytes extra "header"
normfcbdel:
	cmp byte [ds:bx+9],'?'	; filename extension
	jz fcbstink
	mov ax,word [ds:bx+10]
	cmp al,'?'
	jz fcbstink
	cmp ah,'?'
	jz fcbstink
	mov word [cs:stinkptr],exedelmsg
	jmp normfcbcheck	; jump right inside generic FCB check

%if FASCIST_EXEPROT
i21fcbrencheck:
	mov word [cs:stinkptr],exerenmsg
	test byte [cs:flags],BEFASCIST	; picky exe protection on?
	jz i21fcbcheck		; else skip target name checks
	mov word [cs:stinkptr],exerentomsg
	push ax
	push bx
	mov bx,dx		; FCB is at DS:DX
	cmp byte [ds:bx],0xff	; ext FCB?
	jnz normfcbren
	add bx,7		; ext FCB has 7 bytes extra "header"
normfcbren:
	push bx
	; *** QUESTION: can target FCB be extended, too?
	mov ax,[ds:bx+9+11]	; TARGET filename extension
	mov bl,[ds:bx+11+11]
	call ifexe		; set CY if exe/com/sys
	pop bx
	jc fcbstink
	mov word [cs:stinkptr],exerenmsg
	jmp normfcbcheck	; jump right inside generic FCB check
				; ... to check source file name
%endif

fcbstink:			; FCB equivalent to accessdenied
	pop bx
	pop ax
	call WARNINGif
	mov al,0xff		; "file not found or access denied"
	iret			; (FCB open is always for read/write)


do_exeprot:			; *** START of EXE PROTECTION stuff
	mov word [cs:stinkptr],exemsg
	cmp ah,0x0f		; FCB open?
%ifndef FASCIST_EXEPROT
	jnz i21noopenfcb
%else
	jz i21fcbcheck
	jmp i21noopenfcb
%endif
i21fcbcheck:
	push ax
	push bx
	mov bx,dx		; DS:DX is the unopened FCB
	cmp byte [ds:bx],0xff	; ext FCB?
	jnz normfcbcheck
	add bx,7		; ext FCB has 7 bytes extra "header"
normfcbcheck:
	mov ax,[ds:bx+9]	; filename extension
	mov bl,[ds:bx+11]
	call ifexe		; set CY if exe/com/sys
	jc fcbstink		; reject action, show warning.
	pop bx
	pop ax
	jmp i21okay2		; do not intercept

i21extopen:			; check ext. open (new 3/2005)
	push dx
	mov dx,si		; DS:SI is file name, make that DS:DX
	call ifexeAsciiz	; set CY if exe/com/sys
	pop dx
	jnc i21extopenokay
	and dl,0x0f0		; force "fail if exists" (NO MESSAGE)
%if FASCIST_EXEPROT		; reject even creation
	test byte [cs:flags],BEFASCIST	; picky exe protection on?
	jz i21extopenokay	; be no fascist :-)
; 21.6c0x.bl=mode.bh=flags.cx=attr.dl=(hi: 1=create_if_not_there,
; 0=fail_if_not_there, lo: 1=open_if_ex 2=replace_if_ex 0=fail_if_ex)
	test dl,0x0f0		; unless "fail if not yet exists" anyway
	jz i21extopenokay
	mov word [cs:stinkptr],exemsg
	push bx
	dec bx			; 0/3 becomes 3/2, 1/2 becomes 0/1
	test bl,2		; was it 1 or 2?
	pop bx
	jnz i21extopenokay	; modes 0/3 are acceptable
accessdenied2:
	jmp accessdenied
%endif

i21extopenokay:
i21okay3:
	jmp i21okay2		; chain with modified registers

i21rename:			; check if renaming exe (3/2005)
	mov word [cs:stinkptr],exerenmsg
	call ifexeAsciiz	; source name should not be exe
%ifndef FASCIST_EXEPROT
	jc accessdenied		; (even if target name is exe, too)
%else
	jc accessdenied2
%endif
%if FASCIST_EXEPROT
	test byte [cs:flags],BEFASCIST	; picky exe protection on?
	jz i21okay3		; else be happy
	mov word [cs:stinkptr],exerentomsg
	push ds			; new 3/2005
	push dx
	mov dx,es
	mov ds,dx
	mov dx,di		; target name was in es:di
	call ifexeAsciiz
	pop dx
	pop ds
	jnc i21okay3		; ignore other files
	jmp short accessdenied
%else
	jmp i21okay
%endif

i21delete:
	mov word [cs:stinkptr],exedelmsg
i21create:			; create or truncate file (3/2005)
	call ifexeAsciiz	; set CY if exe/com/sys, ignore other
	jc accessdenied		; reject action, show warning.
	jmp i21okay

i21rename2:
	jmp i21rename

i21noopenfcb:
	cmp ah,0x56		; name rename?
	jz i21rename2
	; *** NO check for int 21.71xx LFN functions yet!
	cmp ah,0x41		; name delete?
	jz i21delete
	mov word [cs:stinkptr],exereplacemsg
	cmp ah,0x16		; FCB replace/create?
%ifndef FASCIST_EXEPROT
	jz i21fcbcheck
%else
	jz i21fcbcheck2
%endif
	cmp ah,0x17		; FCB rename
%if FASCIST_EXEPROT
	jnz i21notfcbren
	jmp i21fcbrencheck	; check "from" AND "to" name
i21fcbcheck2:
	jmp i21fcbcheck
i21notfcbren:
%else
	jz i21fcbcheck		; check only "from" name
%endif
	cmp ah,0x3c		; handle "create or truncate"?
	jz i21create
%if FASCIST_EXEPROT
	test byte [cs:flags],BEFASCIST	; picky exe protection on?
	jz allow_notrunc_create
	mov word [cs:stinkptr],execreatemsg
	cmp ah,0x5b		; handle "create" (no truncate)?
	jz i21create
allow_notrunc_create:
	cmp ah,0x6c		; DOS: 6c00, OS/2: 6c01
	jnz i21noextopen
	jmp i21extopen
i21noextopen:
%else
	cmp ah,0x6c		; DOS: 6c00, OS/2: 6c01
	jz i21extopen
%endif
	cmp ah,0x3d		; handle open?
	jnz i21noexeprot
; i21open:
	call ifexeAsciiz	; set CY if exe/com/sys
	jnc i21noexeprot
	mov ah,al		; save the original open mode
	and al,7
	cmp al,3		; DOS internal / EXEC case sensitive
	jz openmodeok
	cmp al,0		; read only
	jz openmodeok
			
	mov word [cs:stinkptr],exemsg
	mov al,ah		; original open mode (for the warning)
	mov ah,0x3d		; original function

%if 1			; TAKE CARE: access denied is safer than open R/O
	call WARNINGif
	mov al,0		; override to readonly open mode
	jmp i21chain		; ... but do not deny access!
%endif

accessdenied:
	call WARNINGif		; message added 3/2005
	mov ax,5		; "access denied" - added 3/2005
	jmp cyIRET		; return and set carry

openmodeok:
	mov al,ah		; restore original open mode
	mov ah,0x3d		; restore normal AH value
				; will fall through to i21okay / i21chain

i21noexeprot:			; *** END of EXE PROTECTION stuff
	cmp ax,0x7305		; FAT32 disk I/O?
	jz i21disk
i21okay:
	jmp i21chain

i21disk:			; int 21.7305 FAT32 disk I/O
	test si,1		; write?
	jz i21okay		; no, only read!
	cmp cx,0xffff		; CX will be -1... > 32 MB
	jnz i21dwrprot		; fail for wrong CX
	cmp dl,2		; A: or B:?
	jb i21dflop
	mov word [cs:stinkptr],boothmsg
	test byte [cs:flags],BOOTHARD
	jnz i21dboot

i21dothersector:
	mov word [cs:stinkptr],wrprotmsg
	cmp dl,2		; A: or B:?
	jb i21dothflop
	test byte [cs:flags],WRITEHARD
	jnz i21dwrprot
	jmp short i21okay

i21dothflop:
	test byte [cs:flags],WRITEFLOP
	jz i21okay
i21dwrprot:
	call WARNINGif
	mov ax,0x0300		; write protection error
	jmp cyIRET		; return and set carry

i21dflop:
	mov word [cs:stinkptr],bootfmsg
	test byte [cs:flags],BOOTFLOP
	jnz i21dboot
	jmp short i21dothersector

i21dboot:
	cmp word [ds:bx+2],0	; sector number high
	jnz i21dothersector
	cmp word [ds:bx],2	; sector number low: boot sector?
	jb i21dothersector
	call WARNINGif
	jmp short i21dwrprot

; ------------------------------------

	; old25	dd DEADBEEF
	; i25:	; hook? [NOT YET: (read)]

old26	dd DEADBEEF
i26:	; hook: (boot) write protection
	cmp al,2	; A: or B:?
	jb i26flop
i26hard:		; else harddisk (or ramdisk...!)
	test byte [cs:flags],BOOTHARD
	jz i26nhardboot
	mov word [cs:stinkptr],boothmsg
	call i26ifbootsec
	jc i26fail

i26nhardboot
	mov word [cs:stinkptr],wrprotmsg
	test byte [cs:flags],WRITEHARD
	jz i26chain

i26failwarn:
	call WARNING
i26fail:
	stc
	mov ax,0x0300 	; 3 write protected / 0 write protected
	retf		; not iret! "int" 26 is more like call far.

i26flop:
	test byte [cs:flags],BOOTFLOP
	jz i26nflopboot
	mov word [cs:stinkptr],bootfmsg
	call i26ifbootsec
	jc i26failwarn

i26nflopboot:
	mov word [cs:stinkptr],wrprotmsg
	test byte [cs:flags],WRITEFLOP
	jnz i26failwarn

i26chain:
	jmp far [cs:old26]

i26ifbootsec:		; returns CY set if boot sector
	cmp cx,0xffff
	jz i26huge
	cmp dx,2	; 0 highest 1 boot!?
	jb i26isboot
i26isnormal:
	clc
	ret

i26isboot:
	mov word [cs:stinkcode],0x26
	call WARNINGif
	stc
	ret

i26huge:
	cmp word [ds:bx+2],0	; sector number high
	jnz i26isnormal
	cmp word [ds:bx],2	; sector number low
	jb i26isboot
	jmp short i26isnormal

; ------------------------------------

old27	dd DEADBEEF
i27:	; hook: TSR protection (CS=PSPseg DX=sizebytes)
	mov word [cs:stinkcode],0x2700
	test byte [cs:flags],TSRBLOCK
	jnz tsrpanic
	mov word [cs:stinkptr],tsrmsg
	call WARNINGif
	jmp far [cs:old27]

tsrpanic:
	mov word [cs:stinkptr],tsrmsg
	jmp BIG_STINK

; ------------------------------------

old2f	dd DEADBEEF
i2f:	; hook: ca0x (tbav), 13 (int13 rehook)
	;       [NOT YET: 1226 (FCBopen for NLSFUNC)]
	cmp ah,0x13	; int 13 rehook??
	jz i2fi13rehook
	cmp ax,0xcafe	; THELP?
	jz i2fokay
	cmp ah,0xca	; TBSCANX / ...?
	jnz i2fokay
	mov word [cs:stinkcode],0x2f	; TBAV-API call detected
	cmp al,0	; install check?
	jnz i2ftbother
	cmp bx,0x5442	; magic?
	jnz i2fokay
	dec al		; return "TBSCANX installed"
	or bx,0x2020	; same
	mov word [cs:stinkptr],tbavtmsg
	call WARNINGif
	iret		; reply as if we were TBSCANX

i2ftbother:
	cmp al,2	; set enable status?
	jnz i2fokay
	test bl,1	; enable?
	jnz i2fokay
	mov word [cs:stinkptr],tbavmsg
	jmp BIG_STINK
	
i2fokay:
	jmp far [cs:old2f]

i2fi13rehook:		; replace i13-called-by-dos / reboot-i13 by
			; dsdx / esbx respectively, and return
			; previous dsdx / esbx values
	push word [cs:X13ptr]	; save previous values to be able
	pop word [cs:X13old]	; to return them below ...
	push word [cs:X13ptr+2]
	pop word [cs:X13old+2]
	push word [cs:R13ptr]
	pop word [cs:R13old]
	push word [cs:R13ptr+2]
	pop word [cs:R13old+2]

	mov [cs:X13ptr],dx	; store user handler as our "BIOS"
	mov [cs:X13ptr+2],ds	; handler which WE internally use...
	mov [cs:R13ptr],bx	; store user reboot handler
	mov [cs:R13ptr+2],es	; (only used by this int 2f.13 itself)

	push cx			; display ES by copying it to CX
; ---	mov word [cs:stinkcode],ds
; ---	mov cx,es		; ... and DS by using it as warning ID
	mov word [cs:stinkcode],0x2f13
	mov cx,es		; ... DS is displayed anyway ...
	mov word [cs:stinkptr],i13rehookmsg
	call WARNINGif		; show message and AX, BX, CX and DX
	pop cx

			; To keep control, initial pointers will:
	lds dx,[cs:X13old]	; 1. tell caller about OUR extra handler
			; as "handler which DOS internally uses" ... and
	les bx,[cs:R13old]	; 2. claim that i13 handler for reboot
			; would be at f000:fff0 (or ffff:0) ... if somebody
			; tries to use it, he will trigger a reboot!
	iret

; ------------------------------------

R13old	dd DEADBEEF		; temp var
X13old	dd DEADBEEF		; temp var
R13ptr	dd 0xf000fff0		; "int 13 to be used by DOS at reboot"
X13ptr	dd DEADBEEF		; "int 13 to be used by DOS internally"
	; NORMAL values would be R: BIOS and X: BIOS or iosys-kludge.

; ------------------------------------

old40	dd DEADBEEF
i40:	; hook: 3 (write)
	cmp ah,3
	jz i40write
i40chain:
	jmp far [cs:old40]

i40write:
	mov word [cs:stinkcode],0x40
	mov word [cs:stinkptr],bootfmsg
	test byte [cs:flags],BOOTFLOP
	jz i40noboot
	cmp cx,1
	jnz i40noboot
	cmp dh,0
	jnz i40noboot
i40boot:
	call WARNINGif
	mov ax,0x0300		; write protected, 0 transferred
	jmp cyIRET		; return and set carry

i40noboot:
	mov word [cs:stinkptr],wrprotmsg
	test byte [cs:flags],WRITEFLOP
	jz i40chain
	jmp short i40boot	; was: i40fail


; ------------------------------------
; ------------------------------------


ifexe:	test byte [cs:flags],WRITEEXE	; exe protection enabled at all?
	jz no_exe			; else do not bother to check
	and ax,0xdfdf		; upcase
	and bl,0xdf		; upcase
	cmp ax,"EX"
	jnz no_ex
	cmp bl,"E"
	jmp short might_exe
	;
no_ex:	cmp ax,"CO"
	jnz no_co
	cmp bl,"M"
	jmp short might_exe
	;
no_co:	cmp ax,"SY"
	jnz no_sy
	cmp bl,"S"
	jmp short might_exe
	;
no_sy:
%if FASCIST_EXEPROT
	cmp ax,"BA"
	jnz no_ba
	test byte [cs:flags], BEFASCIST
	jz no_exe		; BAT only matters in /X mode
	cmp bl,"T"
	jmp short might_exe
%endif
no_ba:
no_exe:	clc			; no CY: no com/exe/sys (bat)
	ret
	;
might_exe:
	jnz no_exe		; last letter did not match
is_exe:	stc			; CY set: com, exe or sys (or bat)
	ret

; ------------------------------------

ifexeAsciiz:			; check if DS:DX is a program file name
				; if so, set carry flag.
	test byte [cs:flags],WRITEEXE	; exe protection enabled at all?
	jz no_exe			; else always answer "no exe"
	push ax
	push bx
	mov bx,dx		; DS:DX is the name
scanasciiz:
	mov al,[ds:bx]		; search end of ASCIIZ filename
	or al,al
	jz asciizopen
	inc bx
	jmp short scanasciiz
no3charext:			; added 3/2005
	clc
	jmp short fakeext
asciizopen:			; *** could conceivably crash if BX<4
	; *** DOES happen with "DOS32A /?", which opens "/?". Grrr.
	cmp bx,4		; possible: e.g. short string at seg:0
	jb no3charext
	mov ax,[ds:bx-3]	; first 2 chars of extension
	cmp byte [ds:bx-4],"."	; is it an extension at all?
	jnz no3charext		; no or not 3-char-long extension
	mov bl,[ds:bx-1]	; last char of extension
	call ifexe		; set CY if exe/com/sys
fakeext:
	pop bx
	pop ax
	ret

; ------------------------------------

cyIRET:				; new code 8/2006
	push bp
	mov bp,sp		; stack: BP IP CS flags
	or byte [bp+6],1	; set carry flag on stack
	pop bp
	iret			; better than retf +2

ncIRET:
	push bp			; new code 8/2006
	mov bp,sp		; stack: BP IP CS flags
	and byte [bp+6],-2	; clear carry flag on stack
	pop bp
	iret			; better than retf +2

; ------------------------------------

checkRTM:			; check if TSR is "only RTM".
	push ax			; if so, return with carry flag set.
	push bx
	push es
	push si

	push cx
	push dx
	push di
	mov ax,0x1687		; generic DPMI install check
	int 0x2f		; use: ax=0 "installed". ignore: bx=flags
	pop di			; ignore es:di entry point and size si.
	pop dx			; ignore version
	pop cx			; ignore CPU generation
	jnz issomeextender	; CWSDPMI is just as okay as RTM
	mov ax,0x0fb42		; RTM install check (see also: Ergo, TKERNEL,
	mov bx,0x1001		; DPMILOAD, all related to each other...)
	int 0x2f		; current int 2f, of course.
	or bx,bx
	jnz nrtm		; this TSR cannot be RTM - no RTM found!

issomeextender:			; (more complex checks follow)
	mov ah,0x62		; get current PSP segment to BX
	pushf
	cli
	call far [cs:old21]	; call original handler
	mov ax,bx
	dec bx
	add ax,0x10		; expected int2f handler segment
	mov es,bx		; MCB segment for PSP
	cmp word [es:8],"CW"
	jz maybecwsdpmi
	cmp word [es:8],"RT"
	jnz nrtm
	cmp byte [es:10],"M"
	jnz nrtm
	mov si,rtmhand		; signature of RTM

match2fhandler:			; MCB name was either RTM* or CWSDP*
	xor bx,bx
	mov es,bx
	cmp ax,[es:(0x2f*4)+2]	; check actual int 2f pointer
	jnz nrtm		; TSR seg+10 not equal int 2f seg? Bad!
	les bx,[es:0x2f*4]	; fetch int 2f pointer
	sub bx,si		; "signature" comparison follows
sigcompare:
	mov ax,[cs:si]
	or al,al		; end of comparison loop reached?
	jz sigspecial
	cmp al,[es:bx+si]
	jnz nrtm		; mismatch - cannot be RTM
	inc si
	jmp short sigcompare

sigspecial:
	or ah,ah		; end of signature?
	jz isrtm		; then we found a valid DOS extender
	mov al,[es:bx+si]	; else fetch 8 bit displacement
	inc si
	cbw			; sign-extend displacement to 16 bits
	add bx,ax		; skip according to displacement
	jmp short sigcompare

isrtm:	stc			; wow, it really looks like RTM!
	jmp short isrtm2

maybecwsdpmi:
	mov si,cwshand		; signature of CWSDPMI
	cmp word [es:10],"SD"
	jnz nrtm
	cmp byte [es:12],"P"	; CWSDPMI or CWSDPR0 (latter: ring 0)
	jz match2fhandler

nrtm:	clc			; no known DOS extender
isrtm2:	pop si
	pop es
	pop bx
	pop ax
	ret



	; RTM PSP is at RTM int2f - 0x10, and RTM int2f starts with:
	; cmp ax,1605 / jz wininit / cmp ax,fb42 / jnz skip / jmp main
	; (wininit returns CX=1, shows a warning: blocks Win386)
	; Supported functions: int 2f.fb42.1001 - 1003.
rtmhand	db 0x3d, 0x05, 0x16, 0x74, 0x0d, 0x3d, 0x42, 0x0fb, 0x75
	; after the 75 03: e9 xx xx ea xx xx xx xx f7 c2 01 00 75 f5
	db 0			; follow jump (3), skip "jmp near main"
	db 0xea			; jmp far ...:...
	db 0,0			; end of signature marker
	; Not checked: After jump data: 0xf7, 0xc2, 0x01, 0, 0x75, 0xf5

	; CWSDPMI PSP is at CWSDPMI int 2f segment - 0x10, the int 2f
	; handler starts with "cmp ax,1687 / jnz skip / else:
	; return CL=cpu, AX=0 (installed), BX=1 (32bit), DX=5a (0.90),
	; SI=6 (size), ES:DI=int 2f segment:entry. Offsets are e.g.:
	; handler 115a, entry 117c, in "displacement 18" case.
cwshand	db 0x3d,0x87,0x16,0x75
	; Two styles to continue here:
	; 1. displacement 16 - mov cl,[cs:...] / xor ax,ax / mov bx,1 /
	; mov dx,5a / mov si,6 / les di,[cs:...] / iret / jmp far [cs:...]
	; or 2. displacement 18 - same, but mov si,[cs:..] instead of
	; mov si,6 (depends on CWSDPMI version)
	db 0			; follow jump (16 or 18), skip mov items
	db 0x2e, 0xff, 0x2e	; jmp far [cs:...]
	db 0,0			; end of signature

; ------------------------------------

checkdevices:			; check device chain, panic if changed
	pushf
	cld
	push es
	push ax
	push bx
	push si
	mov si,0x5c		; snapshot from init time is stored in PSP
				; enough space for 82 dwords or 41 words
	mov ah,0x52		; get list of lists pointer ES:BX
	int 0x21
	add bx,0x22		; offset to get first device header
cdloop:	mov ax,[es:bx+2]	; part 2 of chain pointer
	cmp ax,-1		; stop following at end of chain
	jz cddone
	cs lodsw
	xor ax,[es:bx]		; compare part 1 and
	xor ax,[es:bx+2]	; part 2
	jnz cdfail1
	cs lodsw
	sub ax,[es:bx+6]	; strategy pointer
	sub ax,[es:bx+8]	; interrupt pointer
	jnz cdfail2
	les bx,[es:bx]		; follow chain
	cmp di,0x100-4		; stop following at end of snapshot
	jbe cdloop
cddone:	pop si			; all devices checked out okay.
	pop bx
	pop ax
	pop es
	popf
	ret

cdfail1:
	mov word [cs:stinkptr],devchainmsg	; somebody added a device
	les bx,[es:bx]			; point to THAT device
	jmp short cdfail		; *** removal is not flagged!
cdfail2:
	mov word[cs:stinkptr],devpatchmsg	; somebody hooked a device
cdfail:	mov cx,es			; affected device segment
	mov dx,bx			; affected device offset
	mov si,[cs:stinkptr]		; message: "Device ????????..."
	add si,7			; overwrite the "???" part
	mov ah,0
cdclp:	mov al,[es:bx+10]		; start of e.g. name
	cmp al,' '
	ja cdcok
	mov al,' '
cdcok:	mov [cs:si],al
	inc si
	inc bx
	inc ah
	cmp ah,8
	jb cdclp			; copy full name into message
	pop si
	pop bx
	pop ax
	pop es
	jmp BIG_STINK			; halt the system

	; NOTE: we have deliberately overwritten CX:DX with the
	; pointer to the corrupted device header. BIG_STINK does
	; not return to the caller anyway. It shows the registers.


; ------------------------------------
; ------------------------------------


vsafe_api:		; int (13/16/21).fa.5945 API of VSAFE
	pushf
	cmp ah,0xfa	; VSAFE / VWATCH?
	jnz notvsafe
	cmp dx,0x5945	; magic?
	jz isvsafe
notvsafe:
	popf
	ret

isvsafe:
	mov di,0x4559	; magic (returned by all functions!)
	cmp al,0	; install check?
	jnz vsafeother
	mov bx,0xffff	; no hotkey
	; stinkcode already set to current int number elsewhere
	mov word [cs:stinkptr],vsafetmsg
	call WARNINGif
vsafeiEnd:
	popf
	inc sp
	inc sp		; caller IP (instead of RET from fdshield subroutine)
	jmp ncIRET	; return and clear carry

vsafefake2:
	mov ax,2
	jmp short vsafeiEnd

vsafeother:
	mov word [cs:stinkptr],vsafemsg
	cmp al,2	; get/set options?
	ja vsafeother2
vsafestink:
	jmp BIG_STINK	; 1 uninstall attempt / 2 get+set options

vsafeother2:
	cmp al,4
	ja vsafeother3
	jb vsafefake2
	mov bl,1	; "hotkey disabled"
	jmp short vsafeiEnd

vsafeother3:
	cmp al,6
	jb vsafeiEnd	; "set hotkey disable flag" is ignored
	ja vsafeother4
	mov bl,0xff	; "we check network drives" (VSAFE 2 default)
	jmp short vsafeiEnd	; (some CL can be returned, too)

vsafenetset:
	cmp bl,0	; trying to disable net check?
	jz vsafestink
	jmp short vsafeiEnd

vsafeother4:
	cmp al,8
	jb vsafenetset
	ja notvsafe
	mov ax,2
	mov bx,0x200	; return "version 2.00"
	jmp short vsafeiEnd

; ------------------------------------

dummy_vshield:
	pushf
	push ax
	mov ah,42
	db 0x80, 0xfc, 0x4b, 0x74, 0x62
	db 0x80, 0xfc, 0x4c, 0x74, 0x33
	db 0x80, 0xfc, 0x00, 0x74, 0x15
dummy_vshield_end:
	push si
	push cx
	mov si,dummy_vshield
	mov cx,dummy_vshield_end-dummy_vshield
	call checksum
	xor ax,[cs:dummy_vshield_sum]
	jz vs_not_patched

	mov word [cs:stinkptr],patchvmsg
	mov word [cs:stinkcode],1
	jmp BIG_STINK

vs_not_patched:
	pop cx
	pop si
	pop ax
	popf
	ret

; ------------------------------------

dummy_vshield2:
	pushf
	push ax
	mov ah,42
	db 0x80, 0xfc, 0x0e, 0x74, 0x06
	db 0x80, 0xfc, 0x4b, 0x74, 0x09
dummy_vshield2_end:
	push si
	push cx
	mov si,dummy_vshield2
	mov cx,dummy_vshield2_end-dummy_vshield2
	call checksum
	xor ax,[cs:dummy_vshield2_sum]
	jz vs2_not_patched

	mov word [cs:stinkptr],patchvmsg
	mov word [cs:stinkcode],2
	jmp BIG_STINK

vs2_not_patched:
	pop cx
	pop si
	pop ax
	popf
	ret

; ------------------------------------

tbdriver_tunnelcheck:	; mostly the same code as in tbdriver, but
	pushf		; we also do a checksum on this code ;-)
	push bx		; save BX

	cli		; (just cloning TBDRIVER here)
	pushf		; (not really needed)
	cld		; (not really needed)

		; ( TBDISK does "test by cs:[16],1 / jnz +1d" now )

			; <<< start of exactly TBDRIVER equivalent code
	push ax		; save AX
	push bx		; push BX
	xchg ax,bx	; (not needed either!?)
	pop ax		; pop BX first time, as AX
	dec sp
	dec sp
	pop bx		; pop same value BX a second time!
	cmp ax,bx	; (tbdriver uses 0x3b 0xc3 here)
	pop bx		; restore (was AX, so we need the XCHG below)
			; <<< end of TBDRIVER equivalent code

	jz tb_not_tunneled	; 74 0e in TBDISK case
tbdriver_tunnelcheck_end:

	mov word [cs:stinkptr],tunnelmsg
	mov word [cs:stinkcode],0
	jmp BIG_STINK

tb_not_tunneled:
	xchg ax,bx	; back with the real AX
	popf		; restore (matches the extra pushf above)
	pop bx		; restore bx

	push ax
	push si
	push cx
	mov si,tbdriver_tunnelcheck
	mov cx,tbdriver_tunnelcheck_end-tbdriver_tunnelcheck
	call checksum
	xor ax,[cs:tbdriver_tunnelcheck_sum]
	jz tb_not_patched

	mov word [cs:stinkptr],patchtmsg
	mov word [cs:stinkcode],ax
	jmp BIG_STINK

tb_not_patched:
	pop cx
	pop si
	pop ax

	popf		; restore flags
	ret

; ------------------------------------

checksum:		; create checksum for cs:si len cx in ax
	dec cx		; destroys SI and CX
	xor ax,ax
checksum_loop:
	xor ax,[cs:si]
	add ax,0xcafe
	inc si
	loop checksum_loop
	ret

dummy_vshield_sum		dw 0xf001	; anything
dummy_vshield2_sum		dw 0xc001	; anything
tbdriver_tunnelcheck_sum	dw 0xbeef	; anything


; ------------------------------------
; ------------------------------------


BIG_STINK:		; display message and register dump and halt system
	push ax
	mov ax,3	; 80x25 text mode
	pushf		; use stored int 0x10 pointer!
	cli
	call far [cs:old10]
	mov ax,0x0500	; select first page
	pushf
	cli
	call far [cs:old10]
	pop ax

	mov si,stinkmsg
	call strtty
	push ax
	mov ax,[cs:stinkcode]
	call hexdump
	mov al,' '
	call tty
	pop ax
	mov si,[cs:stinkptr]
	call strtty

	call regdump

	test byte [cs:flags],VERBOSE
	jnz delay_reboot

dead_end:
	cli			; give viruses less chances
	hlt			; only NMI can leave this
	jmp short dead_end	; only RESET can leave this

delay_reboot:
	mov si,rebootmsg
	call strtty
	mov ax,0x40
	mov ds,ax
	mov cx,18*20
waitcount:
	mov ax,[ds:0x6c]
	sti		; could help viruses!
waitloop:
	hlt
	cmp [ds:0x6c],ax
	jz waitloop
	loop waitcount

	mov al,0xfe	; do a very hard keyboard controller reboot
	out 0x64,al	; bye bye virus
	db 0xea, 0, 0, 0xff, 0xff	; JMP far 0xffff:0 (reboot)
	; mov sp,0xffff	; enjoy the GPF
	; pop ax	; go for it :-P

; ------------------------------------

WARNINGif:	; display message, but only in verbose mode
	pushf
	test byte [cs:flags],VERBOSE
	jnz yeswarning
	jmp nowarning

WARNING:	; display message for suspicious activity, but 
		; only in verbose mode as viruses might see it!
	pushf
yeswarning:
	push si
	mov si,infomsg
	call strtty
	pop si
	push ax
	mov ax,[cs:stinkcode]
	call hexdump
	mov al,' '
	call tty
	pop ax
	push si
	mov si,[cs:stinkptr]
	cmp byte [cs:si],0	; message starts with magic 0?
	jnz noprefix
	mov si,attemptmsg	; start message with "Attempt to "
	call strtty		; (new 3/2005, save some RAM)
	mov si,[cs:stinkptr]
	inc si			; skip over magic 0
noprefix:
	call strtty
	pop si
	call regdump

	; string dumps added 3/2005
	cmp word [cs:stinkcode],0x21	; int 21 as the cause?
	jnz nowarning
	cmp ah,0x0f
	jb nowarning
	cmp ah,0x18
	ja nowarnfcb		; FCBish cause int 21.0f ... int 21.18?
	call fcbdump		; SHOW FCB name DS:DX
nowarnfcb:
	cmp ah,0x39
	jb nowarning
	cmp ah,0x43		; holes do not matter: 3e..40, 42
	jbe warnascii		; range of filename-bearing warnings
	cmp ah,0x56		; 4e uses filenames, too, but is not
	jz warnascii		; triggering warnings...
	cmp ah,0x5b
	jnz nowarnascii
warnascii:
	call asciidump		; SHOW file name DS:DX
	cmp ah,0x56		; rename uses TWO filenames
	jnz nowarnascii		; we are done for everything else
	push ds
	push dx
	mov dx,es
	mov ds,dx
	mov dx,di		; 2nd filename is at es:di
	call asciidump
	pop dx
	pop ds
nowarnascii:
	cmp ah,0x6c		; extended open is special
	jnz nowarning
	push dx
	mov dx,si
	call asciidump
	pop dx
nowarning:
	popf
	ret

; ------------------------------------

strtty:		; display string at CS:SI
	push ax
strtty_loop:
	mov al,[cs:si]
	or al,al
	jz strtty_done
	call tty
	inc si
	jmp short strtty_loop
strtty_done:
	pop ax
	ret

; ------------------------------------

tty:		; display char AL - destroys AX
	push bx
	push bp
	mov ah,0x0e	; teletype
	mov bx,7	; page 0, color...
	pushf
	cli
	call far [cs:old10]
	pop bp
	pop bx
	ret

old10	dd DEADBEEF	; pointer to original int 10 handler
			; (in case somebody later messes it up)

; ------------------------------------

hexdump:	; display value in AX like printf(" %x",ax)
	push ax
	push bx
	push cx
	mov bx,ax
	mov al,' '
	call tty
	mov cl,12	; highest digit first
onedigit:
	mov ax,bx
	shr ax,cl
	and al,0x0f	; one nibble
	add al,'0'
	cmp al,'9'
	jbe decdigit
	add al,('A'-1)-'9'
decdigit:
	call tty
	sub cl,4	; next digit
	cmp cl,-4
	jnz onedigit
	pop cx
	pop bx
	pop ax
	ret

; ------------------------------------

regdump:		; dump AX BX CX DX DS register values, PSP info
	push si
	mov si,regmsg
	call strtty
	pop si
	push ax
	; mov ax,ax
	call hexdump
	mov ax,bx
	call hexdump
	mov ax,cx
	call hexdump
	mov ax,dx
	call hexdump
	mov ax,ds
	call hexdump
	push bx
	mov ah,0x62	; get current PSP segment to BX
	pushf
	cli
	call far [cs:old21]	; call original handler
	mov ax,bx	; current PSP does not have to be CALLER PSP
	push ds		; ... but grabbing the caller is more work :-P
	dec ax		; MCB in front of that
	mov ds,ax
	xor bx,bx
psploop:
	mov al,[ds:bx+8]
	cmp al,' '
	jae pspokchar
	mov al,' '	; avoid unprintable chars
pspokchar:
	mov [cs:pspmsg2+bx],al
	inc bx
	cmp bl,8	; copy up to 8 chars
	jb psploop
	mov ax,ds
	pop ds
	pop bx
	push si
	mov si,pspmsg
	call strtty
	pop si
	inc ax		; PSP segment
	call hexdump
	pop ax
	push si
	mov si,crlfmsg
	call strtty
	pop si
	ret

; ------------------------------------

fcbdump:		; dump DS:DX FCB name and drive. New 3/2005.
	push si
	push bx
	push cx
	mov si,fcbmsg2	; after the CR LF,"FCB: " prefix in fcbmsg
	mov bx,dx	; FCB is at DS:DX
	mov cl,[ds:bx]	; -1 or drive, 1 based. 0 is current.
	cmp cl,-1
	jnz nfcbdump
	add bx,7	; was ext. fcb
nfcbdump:
	mov cl,[ds:bx]
	add cl,'A'-1	; just use @ for current, A for A:, etc.
	mov [cs:si],cl	; update drive letter
	mov ch,11
nfloop:	inc bx
	inc si
	mov cl,[ds:bx]
	mov [cs:si],cl
	dec ch
	jnz nfloop	; copy file name
	pop cx
	pop bx
	mov si,fcbmsg
	call strtty	; show 13,10,"FCB: A:filenameexe",13,10
	pop si
	ret

; ------------------------------------

asciidump:		; dump DS:DX ASCIIZ filename. New 3/2005.
	cld		; no prob, caller uses pushf/popf anyway...
	push si
	mov si,asciimsg
	call strtty
	push ax
	mov si,dx
adumploop:
	lodsb		; from DS
	cmp al,0	; done?
	jz adumpdone
	cmp al,' '
	ja safechar
	mov al,'?'	; replace with something safe
safechar:
	call tty
	mov ax,si
	sub ax,dx	; how far along the string are we?
	cmp ax,128
	jb adumploop	; loop only if not too long

	mov al,'+'	; mark over-long problem
	call tty
adumpdone:
	pop ax
	mov si,crlfmsg
	call strtty
	pop si
	ret


; ------------------------------------
; ------------------------------------


stinkptr	dw foomsg	; near pointer to reason string
stinkcode	dw 0x4711	; hex reason code

regmsg		db 13,10,"Registers AX BX CX DX DS:",0
		; AX BX CX DX DS (could also show ES SI DI, BP?, SP?...)
pspmsg		db " Program: "
pspmsg2		db "???????? at",0
fcbmsg		db "FCB: "	; continued...
fcbmsg2		db "A:12345678abc"	; continued...
crlfmsg		db 13,10,0
asciimsg	db "FILE: ",0

infomsg		db 13,10,"FDSHIELD:",7,0
stinkmsg	db 13,10,"System halted by FDSHIELD!",13,10
		db "Reason:",0
rebootmsg	db 13,10,"System will reboot after 20 seconds",13,10,0
attemptmsg	db "Attempt to ",0

		; message starting with 0 means "prefix attemptmsg"
tunnelmsg	db 0,"tunnel us",0
patchvmsg	db 0,"sabotage VSHIELD",0
patchtmsg	db 0,"sabotage TBDRIVER",0
vsafemsg	db 0,"disable VSAFE / VWATCH",0
tbavmsg		db 0,"disable TBAV / TBDRIVER",0
tsrmsg		db "Program stayed in RAM",0 ; tsrmsg - see above
foomsg		db 0,"cheat",0
		;
exemsg		db 0,"open program R/W",0
%if FASCIST_EXEPROT
execreatemsg	db 0,"create / replace program",0
		; alas even COPY does it that way, so we cannot
		; make an exception for compilers... Or can we?
exerentomsg	db 0,"rename to program",0
		; e.g. UPX renames temp file to exe in the end
%endif
exereplacemsg	db 0,"replace program",0
exerenmsg	db 0,"rename program",0
exedelmsg	db 0,"delete program",0
		; e.g. UPX does that, even if rename fails!
		;
fcbdelmsg	db 0,"wildcard-FCB delete",0	; for "name.*" case
vsafetmsg	db 0,"detect VSAFE / VWATCH",0
tbavtmsg	db 0,"detect TBAV / TBDRIVER",0
formatmsg	db 0,"format harddisk",0
boothmsg	db 0,"write harddisk boot sector",0
bootfmsg	db 0,"write floppy boot sector",0
wrprotmsg	db 0,"write to disk",0
i13rehookmsg	db "Tracking rehook of disk API",0

rtmmsg		db 13,10,"FDSHIELD: Okay for DOS extender to stay in RAM",0

devchainmsg	db "Device ???????? should not be here in chain",0
		; either it got added or the one before it got removed
devpatchmsg	db "Device ???????? got patched / hooked",0

; ------------------------------------
; ------- END OF RESIDENT PART -------
; ------------------------------------

install:
	mov ax,0x3000	; get DOS version
	int 0x21
	cmp al,3	; at least DOS 3?
	jb stoneage
	ja dos4
	cmp ah,1	; at least DOS 3.1?
	jnb dos31
stoneage:		; too old, give up
	mov ah,9
	mov dx,vermsg
	int 0x21
	mov ax,0x4cff
	int 0x21

dos31:			; 3.31+ has drives above 32 MB, 4.0+ has ext.
	; open, but we take no own initiative in using those (or, e.g.,
	; even DOS 3.0+ truename), so FDSHIELD itself can run on 3.1...
	; 3.1 needed for compatible LoL format, 3.0 is needed for get
	; current PSP" call 21.62. Create new, share, ioctl are 3+, too.
dos4:	push ds		; first, silently save int 10 handler pointer
	xor ax,ax
	mov ds,ax
	mov ax,[ds:(0x10*4)]
	mov [cs:old10],ax
	mov ax,[ds:(0x10*4)+2]
	mov [cs:old10+2],ax
	pop ds

	mov si,dummy_vshield
	mov cx,dummy_vshield_end-dummy_vshield
	call checksum
	mov [dummy_vshield_sum],ax

	mov si,dummy_vshield2
	mov cx,dummy_vshield2_end-dummy_vshield2
	call checksum
	mov [dummy_vshield2_sum],ax

	mov si,tbdriver_tunnelcheck
	mov cx,tbdriver_tunnelcheck_end-tbdriver_tunnelcheck
	call checksum
	mov [tbdriver_tunnelcheck_sum],ax

	mov dx,bannermsg
	mov ah,9	; show string
	int 0x21

	mov ax,0x1683	; get current VM number (Windows 3+)
	xor bx,bx	; assume 0
	int 0x2f
	or bx,bx	; any?
	jnz isWin3Win9x
	; *** next 3 tests added 3/2005 (21.3306, 2f.1600, 2f.160a) ***
	mov ax,3306h	; get internal DOS version (DOS 5+)
	xor bx,bx
	int 21h		; older DOS just reports AL=-1 here
	cmp bx,3205h	; WinNT/2000/XP/... DOS boxes report 5.50
	jz isWinNT
	mov ax,1600h	; Win32 install check
	int 2fh
	test al,7fh	; returns AL=1/-1/3 for Win 1/2/3
	jnz isWin9x
	mov ax,160ah	; Win3+ version check
	int 2fh
	or ax,ax	; zero if supported, returns version BH.BL
	jz isWin3Win9x	; and CPU level CX
	; *** add more tests here ***
	jmp short nowindows
isWin3Win9x:
isWin9x:
isWinNT:
	mov dx,winmsg	; Warn: Windows 3+ running (can spoil /v ...)!
	mov ah,9	; show string
	int 0x21
nowindows:

	mov byte [cs:flags],0	; all functions off by default
	mov si,0x81
parseloop:
	mov al,[si]
	inc si
	cmp al,9	; ignore tab
	jz parseloop
	cmp al,'/'	; ignore slash
	jz parseloop
	cmp al,' '	; ignore space
	jz parseloop
	cmp al,'?'
	jz helpme	; show help
	cmp al,13
	jnz nohelp	; everything else must be an option
	jmp parsedone

helpme:	mov dx,helpmsg
	mov ah,9	; show string
	int 0x21
	mov ax,0x4c01	; exit here
	int 0x21

nohelp:	cmp al,'t'
	jnz no_t
	or byte [cs:flags],TSRBLOCK
	jmp short parseloop

no_t:	cmp al,'T'	; STRICT version TSR block: disable checkRTM
	jnz no_t2	; (so not even DOS extenders may go TSR)
	or byte [cs:flags],TSRBLOCK
	mov word [cs:checkRTM],0xc3f8	; patch to "clc, ret"
	jmp short parseloop

no_t2:	cmp al,'v'
	jnz no_v
	or byte [cs:flags],VERBOSE
parseloop2:
	jmp short parseloop

no_v:	cmp al,'b'
	jnz no_b
	or byte [cs:flags],BOOTFLOP
	jmp short parseloop

no_b:	cmp al,'B'
	jnz no_b2
	or byte [cs:flags],BOOTHARD
	jmp short parseloop
	
no_b2:	cmp al,'w'
	jnz no_w
	or byte [cs:flags],WRITEFLOP
	jmp short parseloop
	
no_w:	cmp al,'W'
	jnz no_w2
	or byte [cs:flags],WRITEHARD
	jmp short parseloop2
	
no_w2:	cmp al,'x'
	jnz no_x
	or byte [cs:flags],WRITEEXE
	jmp short parseloop2

no_x:
%if FASCIST_EXEPROT
	cmp al,'X'
	jnz no_x2
	or byte [cs:flags],WRITEEXE + BEFASCIST
	jmp short parseloop2
no_x2:
%endif
	jmp helpme	; syntax error (unknown option) - show help

; ------------------------------------

parsedone:	; now hook all those interrupt handlers
	test byte [cs:flags],TSRBLOCK
	jz norecorddevices

; recorddevices:
	cld
	push ds
	push es
	mov di,0x5c		; overwrite "default FCB" and command line
				; enough space for 82 dwords or 41 words
	mov ah,0x52		; get list of lists pointer ES:BX
	int 0x21
	add bx,0x22		; offset to get first device header
	push es
	push ds
	pop es			; exchange es and ds
	pop ds			; now DS:BX is in chain, ES:DI is in PSP
rdloop:	mov ax,[ds:bx+2]	; part 2 of chain pointer
	cmp ax,-1		; stop following at end of chain
	jz rddone
	xor ax,[ds:bx+0]	; check part 1, too...
	stosw			; save
	mov ax,[ds:bx+6]	; strategy pointer
	add ax,[ds:bx+8]	; interrupt pointer
	stosw			; save
	lds bx,[ds:bx]		; follow chain
	cmp di,0x100-4		; stop following if storage full
	jbe rdloop
	push es
	pop ds
	mov ah,9
	mov dx,devfullmsg	; warn about overfull list
	int 0x21
rddone:	cmp di,0x100
	jz rdpop
	xor ax,ax		; pad list with 00s
	stosw
	stosw
	jmp short rddone

rdpop:	pop es
	pop ds
norecorddevices:


	mov [X13ptr+2],cs		; extra redirection layer
	mov word [X13ptr],extra13	; for int 2f.13 rehook trick


	; *** cache flush stuff added 3/2005 ***
	test byte [cs:flags],WRITEFLOP | WRITEHARD
	jz nwxInit
	mov ah,9
	mov dx,flushmsg
	int 0x21
	mov ah,0dh		; DOS disk system reset
	int 0x21		; general cache flushing
	mov ax,0x4a10
	xor bx,bx
	mov cx,0xebab
	int 0x2f		; SMARTDRV install check
	cmp ax,0xbabe
	jnz nwxInit
	mov ax,0x4a01
	mov bx,1
	int 0x2f		; SMARTDRV flush
	; not done here: cdblitz flush (2f.1500.ch=96.bx=1234, detect:
	; 2f.1500.ch=90.bx=1234 returns cx=1234), pc-cache flush
	; (16.ffa5.cx=-1, detect: 16.ffa5.cx=1111 returns cx=0),
	; quick cache flush (13.0021, detect: 13.27.bx=0 returns ax=0
	; and bx!=0), super pc-kwik flush (13.00a1.si=4358, detect:
	; 21.2b.cx=4358 returns al=0), smartdrv sys flush
	; (21.4403.bx=handle-of-smartaar.cx=0.ds:ds=buffer with 2,0)
	; ... but those caches are not common anyway ;-).
nwxInit:
	test byte [cs:flags],WRITEFLOP
	jz nwfInit
	mov dl,0		; first floppy
	xor ax,ax
	int 0x13		; reset disk
nwfInit:
	test byte [cs:flags],WRITEHARD
	jz nwhInit
	mov dl,0x80		; first harddisk
	xor ax,ax
	int 0x13		; reset disk
nwhInit:
	; *** end of cache flush stuff ***


	; Feature switch feedback display added 3/2005
	mov cl,[cs:flags]
	mov si,featurelist
featdisp:
	test cl,1		; that feature active?
	jz nofeat
	mov ah,9
	mov dx,featuremsg	; CRLF,"Enabled "
	int 0x21
	mov dx,[si]		; description of the feature
	mov ah,9
	int 0x21
nofeat:	inc si			; next feature
	inc si
	shr cl,1		; continue bit-scan
	or cl,cl		; any features left?
	jnz featdisp
	

	mov si,intlist
	xor ax,ax
	push es
	mov es,ax
	cld
hookloop:
	lodsw
	or ax,ax
	jz hookdone
	mov di,ax	; int vector number
	add di,di
	add di,di	; offset into IDT
	lodsw		; handler offset
	mov bx,ax
	cli
	mov ax,[es:di]		; old handler offset
	mov [bx-4],ax		; store
	mov ax,[es:di+2]	; old handler segment
	mov [bx-2],ax		; store
	mov [es:di],bx		; new handler offset
	mov [es:di+2],cs	; new handler segment
	jmp short hookloop

hookdone:
	sti
	pop es
	mov dx,instmsg
	mov ah,9	; show string
	int 0x21

	push es
	mov es,[cs:0x2c]	; PSP[2c] -> environment segment
	mov ah,0x49	; free memory
	int 0x21
	pop es		; (errors ignored)

	mov dx,install	; stay resident but throw away installer
	add dx,15
	mov cl,4
	shr dx,cl	; converted to paragraphs now
	mov ax,0x3100	; terminate and stay resident
	int 0x21
selftsr:		; will be "return" CS:IP on stack...

intlist:
	dw 0x13, i13,   0x16, i16,   0x21, i21,   0x26, i26
	dw 0x27, i27,   0x2f, i2f,   0x40, i40,   0, 0

; ------------------------------------

featurelist:	; for making the bit field human-readable
		; order must match bit numbers in flags byte
	dw feat0,feat1,feat2,feat3, feat4,feat5,feat6,feat7

featuremsg:
	db 13,10,"Enabled ==> $"
feat0	db "Extra COM, EXE, SYS and BAT file protection$"
feat1	db "Freeze PC when a program goes resident or a device is added$"
feat2	db "Write protection for FAT disks C:-Z:, be careful with this!$"
feat3	db "Diskette write protection$"
feat4	db "Messages for blocked or suspicious access$"
feat5	db "Boot sector write protection for MBRs and FAT disks C:-Z:$"
feat6	db "Diskette boot sector write protection$"
feat7	db "Protection of COM, EXE and SYS files$"

bannermsg:
	db "FreeDOS FDSHIELD 'virus shield'  (c) by Eric Auer 6/2004 - 8/2006",13,10
	; *** keep VERSION in above line updated ***
	db "Email: <eric*coli.uni-sb.de>    This is free open source software",13,10
	db "under the GNU Public License (see www.gnu.org) version 2 or newer",13,10
	db "$"

vermsg	db "The FDSHIELD virus shield cannot work with DOS 3.0 or older!"
	db 13,10,"$"

devfullmsg:	; out of storage space for device driver info
	db 13,10,"Warning: "
	db "Will only monitor the first 41 devices for changes!",13,10,"$"

flushmsg:
	db 13,10,"Flushing caches, activating write protection...",13,10,"$"

winmsg	db 13,10,"You are in a Windows DOS box - can only protect the box!",13,10,"$"

instmsg	db 13,10,"FDSHIELD installed now until the next reboot :-)",13,10,"$"

helpmsg:
	db 13,10
	LOANSI
	HIANSI
	db "Syntax: "
%if FASCIST_EXEPROT
	db "FDSHIELD [/?] [/v]  [/x] [/X]  [/b] [/B]  [/t] [/T]  [/w] [/W]",13,10
%else
	db "FDSHIELD [/?] [/v] [/x]   [/b] [/B]   [/t] [/T]   [/w] [/W]",13,10
%endif
%if FASCIST_EXEPROT
	db "  /v  show verbose warnings     /?  show help, do not start shield",13,10
	db "  /x  protect exe/sys/com       /X  protect exe/sys/com/bat more",13,10
%else
	db "  /v  show verbose warnings     /x  protect exe/sys/com",13,10
%endif
	LOANSI
	db "      Warning: There is no LongFileName access file protection yet",13,10
	HIANSI
	db "  /b  floppy boot protect       /B  harddisk/ramdisk boot protect",13,10
	LOANSI
	db "      Do not try to FORMAT drives with protected boot sectors",13,10
	HIANSI
	db "  /t  block TSRs and devices    /T  block CWSDPMI and RTM, enable /t",13,10
	LOANSI
	db "      Use /T in DOS boxes or load your DOS extender as TSR first",13,10
	db "      TSR block *halts* the PC when a TSR or device gets loaded",13,10
	HIANSI
	db "  /w  floppy write protect      /W  harddisk/ramdisk write protect",13,10
	LOANSI
	db "      Activating /w and /W together simulates all files readonly",13,10
		; *** If FindFirst, FindNext, Attrib43 could check drive
		; *** letters, /w and /W could do ATTRIB tricks separately.
	db "      Writes to write-protected fixed/RAM-disks can *hang* DOS",13,10
		; *** Workaround: Mark disks as non-fixed disks, or set some
		; *** "disk is write protected" flag in the dev header?
	db "      You cannot use '|' pipes without writeable TEMP directory",13,10
	db "      Do not start delayed-write caches while /w or /W is on",13,10
		; fdshield already flushes the more popular caches during init,
		; LOADING a cache after it causes evil delayed write errors.
	HIANSI
	db "Note: Sabotage check and raw harddisk format block are always on",13,10
	LOANSI
	db "    ",13,"$"	; the "    ",13 is to hide ANSI escapes
		; for the case of no ANSI being loaded. You need N spaces
		; for this, where N is the length of LOANSI.

%if 0
	db 13,10
	db "Not yet available: int hook check, network drive",13,10
	db "protection, signature and checksum test on open",13,10
	db "and exec and exit, checksum sabotage check, boot",13,10
	db "sector signature and checksum test on access and",13,10
	db "FDSHIELD start, executable write warn, hotkey...",13,10
	db 13,10
%endif


; ------------------------------------
; ------------------------------------


; Some old VSAFE ponderings:
; If it tries to config "MS" VSAFE, it is probably a virus:
;      int 13.fa / 16.fa / 21.fa - the latter is also used by other
;      tools, so check for DX=5945 magic value as well there...?
;      All return DI=4559. AL=0 is only install check.
;      AL=0 returns DI=4559 BX=hotkey if loaded. AL=1 uninstalls.
;      AL=2 configures to "enabled methods BL", where bits are:
;      7 exeprot 6 fdbootprot 5 hdbootprot 4 bootprot 3 execheck
;      2 allwrprot 1 tsrwarn 0 hdformatblock. AL=3 ???. AL=4 get
;      hotkey flag. AL=5 set hotkey flag BL. AL=6/7 get/set network
;      drive scan flag. AL=8 ???. VSAFE/VWATCH comes with PC Tools
;      8+ and bundled with MS DOS 6.x ...


; Some old TBAV ponderings:
; If it tries to config TBAV TBDRIVER, it is probably a virus:
;     int 2f.ca (also for TBSCANX by Frans Veldman)
;     Install check is AL=0 BX=5442 returns AL=-1 BX_or_2020
;     AL=1 returns version AH (22/2x/ca) state AL (0/1 off/on)
;     CX=number_of_signatures BX=emshandle_or_0 / in 2.3+ BX =
;     swapsegment_or_0 DX=EMS_XMS_handle_or_-1 (if swap=0, XMS)
;     AL=2 set state BL (0/1 off/on), AL=3 request RAM scan,
;     AL=4 request file scan
;     Careful: int 2f.cafe.bx=0 is THELP inst check, ret: seg BX!
;     Borland THELP also supports TesSeRact TSR management API at
;     int 2f.5453, but who cares...?


; Maybe have faked VSHIELD / VSAFE / TBDRIVER code as a trap for
; patching viruses?
; VSHIELD: 80 fc 4b 74 62  80 fc 4c 74 33  80 fc 00 74 15
; VSHIELD: 80 fc 0e 74 06  80 fc 4b 74 09
; TBDRIVER: cli pushf cld (pushAX) pushBX (xchgAXBX) popAX sp--
;     sp-- popBX cmpAXBX(3bc3) (popBX) jzNotTunneled
; Shields hook: Int 13, 21, 40, 2f / 20, 27 / 9, 16
;     Better avoid hotkey hookers? Better tap int 21.1, con read
;     with echo, 21.6.dl=ff (con read), 21.7 (con read),
;     21.8 (con read) for that?


; FORMAT - suppress int 13.5 depending on msb of DL (set=harddisk)
; TSR - hang if int 27 / 21.31 are used (or certain IDT slots change)


; WRPROT - suppress int 13.3 / 13.43 / 40.3 / 26 / 21.7305.SI_odd
; ... maybe do int 21.43 returned CX / int 21.4e.ds[dx+15] /
;     int 21.4f.ds[dx+15] or_1 to create impression of all files
;     being readonly at least for get_attrib and find-first/-next?
;     Avoids DOS panic about disk errors. Should depend on DL value.


; Not yet planned: Signature scanning / checksums. Scan file on:
; exec (int 21.4b),
; exit (int 21.4c / 20),
; open (int 21.3d / 21.0f / 2f.1226). Scan on create/truncate, too?


; Change open mode to readonly on open seems to be useful, though.
; Avoids having to trap file writes elsewhere ;-).
; Otherwise: trap int 21.40, 21.15, 21.22 - 2f.122x can only read
; files anyway (!), used by NLSFUNC only.


; Boot / MBR scan on read - int 13.2 / 40.2 / 13.42 / 25 ...
;     also scan when program starts? and for each warmboot?
; Int 13 handler change: int 2f.13 - caller passes ds:dx new
;     handler and es:bx "int 13 handler for reboot", function
;     returns ds:dx, 1st: io.sys int 13, es:bx, 1st: bios int 13.
;     Initial ds:dx is as es:bx unless 01/10/84 BIOS... From RBIL:


; IO.SYS hooks INT 13 and inserts one or more filters ahead of the
; original INT 13 handler.  The first is for disk change detection
; on floppy drives, the second is for tracking formatting calls and
; correcting DMA boundary errors, the third is for working around
; problems in a particular version of IBM's ROM BIOS  [Win 3.1 init
; insists on DS changing when it calls this int 2f.13 to install a
; dummy handler during init... Viruses use int 2f.13 to find int 13]


; Other idea: Warn if chklist.* or smartchk.* deletion is attempted.
; Warn if something tunnels the shield. Store file checksums and boot
; sector checksums in XMS and DOS RAM respectively. Store virus list
; (signatures) for file and boot viruses in XMS.


; Idea: Save int 10 vector at init to be sure to be able to TTY?
; Potential problem: We save oldints right before new handlers.
; This makes un-hooking unnecessary simple. But old DOS viruses
; never heard of FDSHIELD anyway.


