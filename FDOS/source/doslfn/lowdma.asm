;This TSR redirects INT40 or Int13 for transferring floppy data not into UMB
;For use in conjunction with UMBPCI
;This little program itself must be loaded into low memory! (no LH)
;Possible Enhancements:
;- make a dual-mode, single driver (.EXE, for simpler usage)
;- keep sector only in conventional memory; hook code can reside in UMB
;- make driver uninstallable (not applicable for a .SYS driver section)

	ideal
	model	tiny
CODESEG

IFDEF MAKECOM
bufstart = 5Ch
	org	100h
proc comstart
	;This transient code will be overwritten by next sector...
	call	transient
	jc	@@e
	mov	es,[2Ch]	;environment
	mov	ah,49h
	int	21h		;free memory block
	mov	dx,offset ISRE
	int	27h		;remain resident
@@e:	ret
endp
ELSE

comstart	equ	<>

struc INITREQUEST
 irLength	db ?
 irUnit		db ?
 irFunction	db ?	;function 00 = init is used
 irStatus	dw ?
 irReserved	db 8 dup (?)
 irUnits	db ?
 irEndAddress	dd ?	;highest resident memory location
 irParamAddress	dd ?	;command line (unused)
 irDriveNumber	db ?
 irMessageFlag	db ?
ends

DNext	dd -1		;address of next driver or -1 if end of chain
DAttr	dw 0E000h	;attribute
DStrat	dw OFFSET Strat	;offset of "strategy" routine
DIntr	dw OFFSET Intr	;offset of "interrupt" routine
DName	db 'LOWDMA$$'	;internal name of .SYS
bufstart = 12h		;after the name
ParamAddr	dd	?
proc strat far
	mov	[word LOW  ParamAddr],bx
	mov	[word HIGH  ParamAddr],es
	ret
endp strat
proc intr far
	push	es bx ax
	pushf
	 les	bx,[ParamAddr]
	 mov	[(INITREQUEST es:bx).irStatus],8103h	;an error code
	 mov	[word LOW  (INITREQUEST es:bx).irEndAddress],0
	 mov	[word HIGH (INITREQUEST es:bx).irEndAddress],cs
	 cmp	[(INITREQUEST es:bx).irFunction],0
	 jnz	@@e
	 push	es bx
	  call	transient
	 pop	bx es
	 jc	@@e
	 mov	[word LOW (INITREQUEST es:bx).irEndAddress],offset ISRE
	 mov	[(INITREQUEST es:bx).irStatus],100h	;the OK code
@@e:	popf
	pop	ax bx es
	ret
endp

ENDIF

proc transient
	mov	dx,offset msg$
	mov	ah,9
	int	21h		;write something
	push	sp
	pop	ax		;check for (186 or) 286
	cmp	ax,sp
	mov	dx,offset no286$
	jc	@@err
	P286			;needed for PUSHA/POPA and SHR n
	mov	ax,cs
	cmp	ah,0A0h
	mov	dx,offset err$
	cmc
	jc	@@err
	;Now we are ready to install, try Int40 first
	mov	ax,3540h
	int	21h
	mov	dx,es
	or	dx,bx		;Got a null pointer?
	jnz	@@set
	mov	al,13h
	int	21h		;get interrupt vector
@@set:	mov	[word LOW  OldInt13],bx
	mov	[word HIGH OldInt13],es
	mov	dx,offset NewInt13
	mov	ah,25h
	int	21h		;set interrupt vector
	mov	dx,offset ok$
@@err:
	pushf
	 mov	ah,9
	 int	21h
	popf
	ret
endp

msg$	db	'LowDMA (haftmann#software 11/01): $'
no286$	db	'requires 80286 or higher processor!',13,10,'$'
err$	db	'must not reside in UMB!',13,10,'$'
ok$	db	'installed'
IFDEF MAKECOM
	db	' (no uninstall provided)'
ENDIF
	db	13,10,'$'

	org	bufstart+512
ISRA:
proc NewInt13
	pushf
	 cmp	ah,2		;read sectors?
	 je	@@r
	 cmp	ah,3		;write sectors?
	 jne	@@no
@@r:	 or	al,al		;let BIOS handle this error condition
	 jz	@@no
	 test	dl,80h		;for hard disk?
	 jnz	@@no
	 push	ax bx
	  mov	ax,es
	  shr	bx,4		;discard lower 4 bits
	  add	ax,bx		;HMA? Wraps to conventional memory!
	  cmp	ah,0A0h		;UMB or ROM? No->CY=1
@@h:	 pop	bx ax
	 jnc	@@rw		;redirect if UMB
@@no:
	popf
	db	0EAh		;jump far
OldInt13 dd	?
@@rw:	popf
	pusha
	push	ds es
	 mov	bp,sp
	 push	bx cx ax cs
	 push	bufstart	;one byte instead of "offset buffer"
	 shr	[byte bp+18h],1	;shift carry out
	 mov	[byte bp+12h],0	;number of processed sectors ->AL
	 cld
@@l:
	 lds	si,[bp-2]	;source data
	 les	di,[bp-10]	;destination buffer
	 mov	cx,256
	 rep	movsw		;copy data

	 les	bx,[bp-10]
	 mov	cx,[bp-4]
	 mov	ah,[bp-5]
	 mov	al,1
	 sti
	 pushf
	 call	[cs:OldInt13]	;issue Int13 with local buffer reference

	 lds	si,[bp-10]	;source buffer
	 les	di,[bp-2]	;destination data
	 mov	cx,256
	 rep	movsw		;copy data back

	 jc	@@e		;on error: leave!
	 add	[byte bp-1],2	;same as adding 512 to a word
	 inc	[byte bp+12h]	;sectors transferred ->AL
	 inc	[byte bp-4]	;next sector (no calculation of head jump)
	 dec	[byte bp-6]	;residual sector count
	 jnz	@@l
	 clc
@@e:
	 mov	[bp+13h],ah	;save status ->AH
	 rcl	[byte bp+18h],1	;shift carry in
	mov	sp,bp
	pop	es ds
	popa
	iret
endp
ISRE:
	end	comstart
