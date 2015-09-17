; callver: simple DOS version override helper, by Eric Auer 2003 and 2006.
; license: public domain. have fun!
; compile with nasm -o callver.com callver.asm
;
; run as: CALLVER 1.23 program arguments
; effect: program will get dos version "1.23" from int 21h, ah=30h.
; alternatively, FreeDOS specific: CALLVER 1.23
; will change the reported DOS version to 1.23 until you change it back
; numbers have to be 1+2 digits long.

cpu 8086
	org 100h
start:	jmp setup

runit:	mov sp,stack	; not using cs:80h..cs:0ffh, need to preserve that
	mov bx,stack+4+0fh
	mov cl,4
	shr bx,cl
	push es
	mov ax,cs
	mov es,ax
	mov ah,4ah	; resize memory block on ES to minimum
	int 21h		; (must keep PSP and environment, of course)
	pop es		; (AX and BX got changed)

	push dx		; save dx
	push ds		; save ds
	push es
	mov ax,3521h	; get and store original int 21h vector
	int 21h
	mov [cs:oldint],bx
	mov [cs:oldint+2],es
	pop es
	mov dx,handint	; new int 21h handler
	mov ax,cs
	mov ds,ax
	mov ax,2521h	; set int 21h vector
	int 21h
	pop ds		; restore ds
	pop dx		; restore dx

	mov ax,4b00h	; execute program
	mov bx,cs
	mov es,bx	; ds:dx - executable file name (from runit-caller)
	mov bx,exetab	; es:bx - parameter block
	int 21h

done:	cli
	mov ax,cs
	mov ss,ax
	mov sp,0xfc
	sti
	mov al,-1
	jc leaveit	; did exec itself fail?
	mov ah,4dh	; get errorlevel in AL
	int 21h		; discarded in AH: exit type, which is 0 normal,
			; 1 ctrl-c, 2 critical error, 3 TSR
leaveit:
	push ax
	mov ax,cs
	mov ds,ax
	mov es,ax
	push ds
	lds dx,[oldint]
	mov ax,2521h	; reset int 21h vector
	int 21h
	pop ds
	pop ax
	mov ah,4ch	; exit, returning errorlevel
	int 21h

handint:
	cmp ah,30h		; dos version check?
	jz fakever
	jmp far [cs:oldint]	; chain to original handler
fakever:
	pushf			; fake int operation
	call far [cs:oldint]	; call original handler
	mov ax,[cs:dosver]	; fake returned AX value
	iret			; return to caller

	; PS: exectab is processed by patchPSP and others in FreeDOS...
exetab:	dw 0		; use caller environment segment
	dw 80h		; pointer to "arguments" in PSP segment of callver
	dw 0		; will be PSP segment of callver
	dw 5ch		; 1st FCB: use the one of callver (?)
			; looks as if offset is -1 here, no FCBs are copied
	dw 0		; ... segment
	dw 6ch		; 2nd FCB ...
	dw 0

oldint:	dd 0		; original int 21h vector
dosver:	dw 0		; ah=minor al=major

hellomessage:		; will be overwritten by stack
	db "Use: CALLVER 6.20 [PROGRAM [ARGUMENTS]]",13,10
	db "Run one program (in a new shell, using COMSPEC) with a",13,10
	db "fake DOS version number, or globally (only in FreeDOS)",13,10
	db "modify the reported DOS version (CALLVER 0.00 reverts)",13,10
	db "CALLVER is public domain."
crlfmessage:
	db 13,10,"$"

freedosmessage:
	db "FreeDOS kernel, reports DOS version:$"
freedossetmessage:
	db "Set reported DOS version to:$"
freedosintmessage:
	db "Internal revision and version: $"
realvermessage:
	db "Reported DOS version reset to default:$"

cspecerrormessage:
	db "COMSPEC not found",13,10,"$"
fdoserrormessage:
	db "SETVER mode needs a FreeDOS kernel",13,10,"$"

comarg:	db " /c "	; 4 bytes fixed size



setup:	; ENTRY POINT
	mov ax,cs
	mov [exetab+4],ax
	mov [exetab+8],ax
	mov [exetab+12],ax

	cld
	mov si,0x80	; argument length
	lodsb		; get size
	cmp al,4	; at least "1.23"
	jnb getarg0
nusage:	jmp usage
getarg0:
	jmp getarg


nofreedos:
	mov dx,fdoserrormessage
	mov ah,9	; show string
	int 21h
	mov ax,4cffh	; exit with error status
	int 21h

noprog:
	mov ax,3000h	; get DOS version
	int 21h
	cmp bh,0fdh	; FreeDOS?
	jnz nofreedos
	push ax
	mov dx,freedosmessage
	mov ah,9	; show string
	int 21h
	pop ax
	call showver
	mov ah,9
	mov dx,freedosintmessage
	int 21h
	mov ax,3306h	; get real DOS version
	int 21h
	push bx		; push bx
	mov ax,3000h	; get DOS version
	int 21h
	mov al,bl	; get revision
	call showal
	pop ax		; but pop ax
	call showver
	mov ax,33ffh	; get FreeDOS version string pointer
	int 21h		; returns DX AX
	push ds
	mov ds,dx
	cld
	mov si,ax
nameloop:
	lodsb
	or al,al
	jz namedone
	call tty	; echo to stdout (destroys AX DX)
	jmp short nameloop
namedone:
	pop ds
	mov dx,crlfmessage
	mov ah,9	; show string
	int 21h

	mov bx,[dosver+0]
	mov dx,freedossetmessage
	or bx,bx
	jnz setver
	mov ax,3306h	; get real DOS version
	int 21h		; returns BX
	mov [dosver],bx
	mov dx,realvermessage
setver:
	mov ah,9	; show string
	int 21h
	mov ax,bx
	call showver
	mov ax,33fch	; set DOS version
	int 21h
	mov ax,4c00h	; exit with errorlevel 0
	int 21h


getarg:	lodsb
	cmp al,' '	; skip spaces
	jz getarg
	cmp al,13	; end marker?
	jbe nusage
	call digit	; ensure DIGIT
	mov [dosver],al	; store MAJOR version number
	lodsb
	cmp al,'.'	; next must be dot
	jnz nusage
	lodsb
	call digit	; ensure DIGIT
	mov ah,10
	mul ah
	mov [dosver+1],al	; store MINOR version, high part
	lodsb
	call digit	; ensure DIGIT
	add [dosver+1],al	; store MINOR version, low part
	lodsb
	cmp al,' '	; space after x.yz argument, if other arg follows
	jb noprog
	jnz usage	; if arg follows, it must be separated by space
spc:	lodsb
	cmp al,' '	; skip over space
	jz spc
	jb noprog	; end marker reached before program name

	sub si,4+1	; point to 4 before program name now
	mov di,si
	dec di		; room for length byte
	mov [exetab+2],di	; arguments to command.com start here
	inc di
	mov si,comarg	; " /c " (4 chars)
	movsw
	movsw		; copied the string over the "n.nn " string now

	mov ah,4	; already have 4 chars
	mov si,di
argcnt:	lodsb
	inc ah
	cmp al,13	; eof marker?
	ja argcnt
	dec ah		; do not count eof marker
	; ... could replace eof marker by some other value here ...
	mov di,[exetab+2]	; "command tail" including length byte
	mov [di],ah	; store length
	
	mov ax,[ds:2ch]	; get environment segment from PSP
	mov ds,ax
	xor si,si

findcomspec:
	lodsw
	and ax,0dfdfh	; upcase
	cmp ax,"CO"
	jnz skiparg
	lodsw
	and ax,0dfdfh	; upcase
	cmp ax,"MS"
	jnz skiparg
	lodsw
	and ax,0dfdfh	; upcase
	cmp ax,"PE"
	jnz skiparg
	lodsw
	and al,0dfh	; upcase (only the last "c" of "comspec"...)
	cmp ax,"C="
	jnz skiparg

gotcomspec:
	mov dx,si	; pointer to executable file name of shell
	; mov ds,ds
	jmp runit	; finally, RUN "%COMSPEC% /C program arguments"

skiparg:
	lodsb
	or al,al
	jnz skiparg	; skip until 0 byte
	cmp [ds:si],al	; next byte is 0, too?
	jz cspecerr	; bad luck, no COMSPEC found
	jmp findcomspec	; try next env variable

cspecerr:
	mov dx,cspecerrormessage
	jmp short usage2

usage:	mov dx,hellomessage
usage2:	mov ax,cs
	mov ds,ax
	mov ah,9	; show string
	int 21h
	mov ax,4cffh	; report usage error and exit
	int 21h

digit:	cmp al,'0'	; leave if no digit (i.e. end marker)
	jb nodig
	cmp al,'9'
	ja nodig
	sub al,'0'
	ret		; return digit value otherwise

nodig:	pop ax		; throw away caller address
	jmp usage



tty:	mov ah,2	; output char AL to stdout, destroys AX DX
	mov dl,al
	int 21h
	ret

showal:	push ax		; show binary value in AL as decimal
	push dx
	mov ah,0
	mov dl,10
	div dl		; AL, remainder AH
	or al,al
	jz smallal
	add al,'0'
	push ax
	call tty	; high digit
	pop ax
smallal:
	mov al,ah
	add al,'0'	; low digit
	call tty
	pop dx
	pop ax
	ret

showver:		; show AL.AH as a version string " 1.23" CR LF
	push dx
	push ax
	mov al,' '
	call tty
	pop ax
	push ax
	call showal
	mov al,'.'
	call tty
	pop ax
	push ax
	mov al,ah
	call showal
	mov al,13	; CR
	call tty
	mov al,10	; LF
	call tty
	pop ax
	pop dx
	ret



	align 4		; setup and hellomessage to be overwritten
stack:	dd 0		; by stack while program is running!

