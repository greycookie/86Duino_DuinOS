	masm
COMMENT `	(Copyr. 1992-1994 haftmann#software)	+++FREEWARE+++
Includedatei fÅr das Arbeiten mit Borlands TASM, insbesondere fÅr die
Erstellung von .COM- und TSR-Programmen im IDEAL-Modus.

+++ Werbung +++
MIT HILFE DIESER DATEI KANN MAN GANZ EINFACH UND SCHNELL KöRZESTE
ASSEMBLER-PROGRAMME SCHREIBEN!     Folgende Zeilen genÅgen fÅrs erste:
	include	prolog.asm	;Dieses Inkludieren kostet KEIN BIT!
	ret			;1 Byte; DIESES PROGRAMM LéUFT !!!
	endc			;(so es erst mal nix tut)

Assemblieren mit TASM /z name[.asm]	;Wozu gibt's den Norton Commander?
Linken mit	 TLINK /t name[.obj]	;Am besten diese Jobs auf ".ASM" legen!
Ausprobieren mit NAME[.COM]		;fertig!!!

Oder mit allen Debug-Infos
TASM /zi /z /m2 name[.asm]	;2 AssemblerlÑufe, volle Debuginfo
TLINK /v /s /l name[.obj]	;volle Debuginfo, detaillierte .MAP-Datei
TDSTRIP -c -s name[.exe]	;mache .COM und .TDS
TD name[.com] [parameter]	;Programm testen

Weitere Include-Dateien von haftmann#software (z.T. in Entwicklung)
	strings.asm	;String-Funktionen wie z.B. strcpy (alles ASCIIZ)
	printf.asm	;String-Ausgabe C-like z.T. mit erweiterten Funktionen
	sound.asm	;Tonausgabe (z.Z. nur PC-Speaker) und Delays
	scan2asc.asm	;Umwandlung der Codes NUR fÅr Buchstaben und Ziffern
			;(typisches TSR-Problem, wenn Taste frei definierbar)
	hdrv.asm	;Ordentliches Sektorlesen auf Diskette und Festplatte

+++ Beschreibung der Funktionen der PROLOG.ASM +++
(<>: Pflichtparameter, [] freie Parameter)

PROLOG.ASM schaltet auf folgende Standards:
<IDEAL, MODEL tiny, %NOINCL, %TRUNC, %CONDS, %NOMACS, %NOSYMS, CODESEG>
Der IDEAL-Modus des TASM ist fÅr die Verwendung dieser Datei Bedingung.
(Warum auch sollte man sich mit den BUGS von MICROSOFT herumÑrgern?)

+++ AbkÅrzungen +++
nl	NewLine, Abk. fÅr 0dh,0ah
ParaRes	Gleichung fÅr residente Paragrafen, benîtigt die Marke ResEnd im Pgm.
by	Abk. fÅr "byte"
wo	Abk. fÅr "word"
dwo	Abk. fÅr "dword"
ofs	Abk. fÅr "offset"
len	Abk. fÅr "length"
bset	Abk. fÅr "SETFLAG"
bres	Abk. fÅr "RESFLAG"
btog	Abk. fÅr "FLIPFLAG"
btst	Abk. fÅr "TESTFLAG"
bit	Abk. fÅr "1 shl"

+++ ProgrammstÅckchen +++
ResizeM	[x1]	Komplettes ProgrammstÅck zum VerÑndern der Speicherblockgrî·e.
	Erfordert die Marke ResEnd bei Angabe ohne Parameter oder der Speicher
	wird ab <x1> freigegeben.
PRINT	<^str>	ProgrammstÅck zur Ausgabe eines $-terminierten Strings per
	DOS-Funktion.
LEA$, LEAZ, LEAP <reg>,<string> Lade Register mit Stringzeiger. Der String
	wird im CONST- (Ausnahme LEAP: DATA-) Segment abgelegt und das <reg>
	mit der Offsetadresse geladen. Ideal fÅr Stringkonstanten!
PRINT$	<str> Ausgabe eines Strings ($-terminiert) auf StdOut. Das Dollar-
	zeichen darf NICHT mit angegeben werden!
	BITTE nur fÅr KLEINE Programme verwenden, da öbersetzung schwerer.
JN	<cc>,<label>	Ersetzt fehlenden Jump Near Conditional der 80x86-CPU
	bzw. die %JUMPS-Anweisung. Erzegt den 5-Byte-Ersatzcode. Zugelassene
	Bedingungen sind z.Z. nur: "z","nz","c","nc" (Klein!!)
-->	JN ist hinfÑllig bei Verwendung von %JUMPS. Jedoch sollte man unbedingt
	2 AssemblerlÑufe durch Angabe des Schalters "/m2" aktivieren.
	Das ist auch empfehlenswert, wenn die Daten HINTEN am Programmende
	definiert sind.
JR	<label>	Erzeugt Short-Sprung (Abk. fÅr JMP SHORT label)
CALLF	[seg:ofs]	erzeugt ohne Parameter den Code 9Ah fÅr Call Far.
	Mit Parametern anschlie·end die Bytes
JMPF	[seg:ofs]	siehe CALLF
EX	<x1>,<x2>	Austausch von Registern oder Speicherzellen.
	Insbesondere fÅr den Austausch der Segmentregister ES und DS
LD	<x1>,<x2>	Laden via Push und Pop, spez. fÅr Segregs!
XLD	<x1>,<x2>,[x3]..[x8] Kettenladebefehl von rechts nach links
DOS	[x1],[x2]	DOS-Funktionsaufruf; x1=AH, x2=AL. Wenn x1 bzw.
	x2 fehlt, wird nur 1 8-Bit-Register geladen. Sind beide angegeben,
	wird AX geladen. Fehlen beide, dann nur Aufruf des Int21
	Ist x1Ú100h wird AX mit dem Wert von x1 geladen und x2 ignoriert!
VID	Dasselbe fÅr Int10
DRV	Dasselbe fÅr Int13
KBD	Dasselbe fÅr Int16
MUX	Dasselbe fÅr Int2F
INTR	Master-Makro fÅr DOS, VID, KBD... mit vorangestellter IntNo (ein Mu·!)
IPUSH	<imm16>,[reg] 8086: Konstante via reg (sonst AX) pushen; sonst wie PUSH
PUSH8	8 wichtige Register retten, AX BX CX DX DS SI ES DI, KEIN PUSHA-Ersatz!
POP8	8 wichtige Register holen, DI ES SI DS DX CX BX AX, kein POPA-Ersatz!
OUTIB/W	<dest>,<val> Ausgabe auf festes Port <100h, VR: AX
OUTVB/W	<dest>,<val>,[offset] Ausgabe auf variables Port, VR:DX,AX
STL	<dest>,<hi-reg>,<lo-reg> STore Long Speichert 2 16-bit-Regs in 32bit.
	<reg> kann auch konstant sein!
SDS )	<dest>,<reg>	FÅllt das Doppelwort "dest" mit DS:reg auf. Gegen-
SES )	funktion zu LDS, LES, LSS
SSS )	<reg> kann auch konstant sein!
SCS )
LDL	<hi-reg>,<lo-reg>,<src> LoaD Long LÑdt 2 16-bit-Regs mit 32bit
LDSS	<reg>,<src>	Fehlender LoadSS des 8086, 80286
IS286	Programm testet ob mindestens 80286; CY=0 wenn ja
IS386	Programm testet ob 80386; CY=0 wenn ja;  schlie·t _IS286 ein
RESFLAG	wie MASKFLAG, jedoch mit "verkehrter" Bitangabe; ist doch
	wesentlich sinnvoller als die Borland-Variante!
MIN r2,r1	R2:=Min(R2,R1)
MAX r2,r1	R2:=Max(R2,R1)
INCB reg	Inkrementiere Register, jedoch nicht Åber F..F hinaus (VR: ZF)
DECB reg	Dekrementiere Register, jedoch nicht unter 0..0 (VR: CY, ZF)

+++ Hilfsmakros zur komfortablen Datenablage +++
ALIGNV	<ausr>,[fÅllb]	Erzwingt ein Alignment in angegebener Ausrichtung,
	FÅllbyte defaultmÑ·ig "?".
DPS	<string>	Definiert PASCAL-String mit fÅhrendem LÑngen-Byte.
DCL	<string>	Definiert DOS-Kommandozeilenstring, wie DPS, jedoch
	mit abschlie·endem 0Dh
IRPS	<action>,<"string1">,["string2"]... Wiederhole mit jedem Zeichen
	(1 Byte) der Strings die Aktion <action>. <action> bekommt als
	Parameter genau 1 Zeichen (in Hochkommas) zugestellt. Einzel-
	bytes statt Strings erlaubt. Das Makro "nl" (s.o.) funktioniert
	leider nicht! ErwÅnschte Double-Quotes im String doppelt schreiben!
DXS	<xorbyte>,<char-chain>	Definiert gescrambelte Zeichenkette. (Zeichen-
	kette in spitzen Klammern ohne '' angeben!! Keine CRLF's u.Ñ.)
DCD	<val>	Definiert "val" als numerische Zeichenkette im Speicher.
	(zur Ausgabe der [festen] Programmgrî·e o.Ñ.) Keine VorwÑrtsreferenzen
	bitte! Zahlenbasis von RADIX abhÑngig, normal 10.
DCN	<name>	Definiert <name>, der NICHT in Hochkommas steht, als Zeichen-
	kette im Speicher. Ideal fÅr den Programmnamen: DCN @Filename
DVT	Definiere 1 Programmverteilertabellen-Eintrag: 1 Byte und 1 Word
DZ	Definiere nullterminierten String, notfalls nur eine Null
ENDC	Komfortable END-Anweisung.

+++ Unterprogramme +++
Bei allen Unterprogrammen gilt: Name=Makroname ohne Unterstrich!
_OCHR	Komplett-Routine zur Zeichenausgabe (Zeichen in AL, VR:-)
_AXHX	Komplett-Routine zur Zeichen- und Hexzahlausgabe, folgende Marken:
	AXHX:	AX hex ausgeben
	AHEX:	AL hex ausgeben
	AHEX1:	AL-Low-Nibble hex ausgeben
	OCHR:	AL ASCII ausgeben
	Alle folgenden Prozeduren definieren 1 gleichnamiges Label ohne "_"
_ALDEZ	Sternis geniale DOS-Uhrzeitkonvertierung, nur fÅr AL=0..99!
_AXDEZ	AX dezimal ausgeben 1..5 Zeichen
_UPCASE	Upcase-Routine AL->AL
_TOUP	Komfort-Upcase mit Umlauten, zieht _UPCASE nach sich
_TOLOW	Lowcase-Rountine AL->AL
_ANUM	Wandelt ASCII in AL in numerischen Wert <36 um, bei Fehler AL>=36
_INW	1 Word ab [SI] einlesen, BL=Zahlenbasis. Zieht _ANUM nach sich
	PA: AX: Zahl, CY=1: Fehler: Gar keine Ziffern oder Zahl zu gro·
	    SI zeigt auf erste Nicht-Ziffer
_INW2	1 Word ab [SI] einlesen, Zahlenbasis mit PrÑfixen # (dez) und $ (hex)
	Åberschreibbar; PA: s. _INW, dazu BL=Neue Basis. Zieht INW nach sich!
_INW3	wie INW2, jedoch dezimale Vorgabe und C-mÑ·ige PrÑfixe dazu.
	Zieht _INW2 nach sich!
_CHKWS	Check AL auf WhiteSpace 0,9,0a,0d,20 PA: Z=1 A ist Whitespace
_CRLF	Na was wohl? Aber ohne Register einzusauen! Benîtigt _OCHR!
_CASE	CASE-Anweisung via Tabelle. Tabellenaufbau (via DVT): [1 Byte Code,
	1 Word z.B. Adresse]* 1 Byte 0 Abschlu·, ggf. 1 Word "else"-Adr.
	PE: ES:DI: Zeiger auf Tabelle, AL: Code
	PA: ES:DI: Zeiger auf das Word (nach dem gefundenen Code bzw. nach
		   der Null am Ende
	    CY=1: nicht gefunden
_UPV	Unterprogrammverteiler [si]=Optionsbuchstabe, di=Tabelle, s.a. DVT

+++ Definitionen von DOS-Strukturen +++
tDTA	der Disk Transfer Area
tPSP	der Programmsegment-PrÑfix
tFCB	der Dateisteuerblock
tDirE	der Directoryeintrag auf der Platte
tBigDos	der Anforderungsblock fÅr Int25 und In26 bei BigDOS-Partitionen

+++ "Globale" Labels +++
PSPOrg	eine Marke am PSP-Anfang ("relative 0")
COMentry	hier geht das COM-Programm los!

Sollte ausnahmsweise eine .EXE gewÅnscht werden, empfiehlt sich der
Befehl ORG 0 direkt nach der Include-Anweisung. Dann ist aber ein *anderer*
Eintrittspunkt zu wÑhlen!
`
		IDEAL
		MODEL	tiny
		%NOLIST
		%NOINCL
		%TRUNC		;Begrnzen von Strings im Object-Code
		%CONDS		;Auch nicht Åbersetzte IF's listen
		%NOMACS		;Keine Makros expandieren (Papier sparen)
		%NOSYMS		;Keine "Symboltabelle" bitte!
		NOMULTERRS	;Nie mehrere Fehler pro Quellzeile bitte!

nl		equ	<13,10>	;Zeilenende
ParaRes		equ	<(ResEnd-PSPOrg+15)/16>
		;Residente Paragrafen eines .COM-Programms
by		equ	<byte>
wo		equ	<word>
dwo		equ	<dword>
ofs		equ	<offset>
len		equ	<length>
bit		equ	<1 shl>
macro bset r:rest
	SETFLAG	r
	endm
macro bres r:rest
	RESFLAG	r
	endm
macro btog r:rest
	FLIPFLAG r
	endm
macro btst r:rest
	TESTFLAG r
	endm

macro ResizeM r1
	ifb	<r1>
	 mov	bx,ParaRes
	else
	 mov	bx,(r1-PSPOrg+15) / 16
	endif
	DOS	4ah
	endm

macro PRINT str:req		;handhabbares Ausgabekommando
	mov	dx,ofs str
	DOS	9
	endm

macro LEA$ reg:req,str:rest	;LEA's mit festen Strings
local thestr
	CONST
thestr:	db	str,'$'
	CODESEG
	mov	reg,ofs thestr
	endm

macro LEAZ reg:req,str:rest
local thestr
	CONST
thestr:	dz	str
	CODESEG
	mov	reg,ofs thestr
	endm

macro LEAP reg:req,str:rest
local thestr
	DATASEG
thestr:	dps	str
	CODESEG
	mov	reg,ofs thestr
	endm

macro PRINT$ str:rest		;Ganz komfortables Makro!!
	LEA$	dx,str
	DOS	9
	endm

macro JN cc:req,lab:req
	pushstate
	jumps
	j&cc	lab
	popstate
	endm

macro JR lab:req
	jmp	short lab	;KC-like
	endm

macro SEGOFS r1:rest		;Internes Makro!
		ifnb	<r1>
@cxxf1		 instr	<r1>,<:>
		 if	@cxxf1 lt 2
		  err	<Wrong colon>
		 endif
@cxxf2		 substr	<r1>,1,@cxxf1-1
@cxxf3		 substr	<r1>,@cxxf1+1
		 dw	@cxxf3,@cxxf2
		endif
		endm

macro CALLF r1:rest		;r1 Seg:Ofs
	db	9ah
	segofs	r1
	endm

macro JMPF r1:rest		;r1: Seg:Ofs
	db	0eah
	segofs	r1
	endm

macro EX r1:req,r2:req		;zum Vertauschen von Segmentregistern
	push	r1 r2
	pop	r1 r2
	endm

macro LD r1:req,r2:req		;zum Laden von Segmentregistern
	push	r2
	pop	r1
	endm

macro XLD r1,r2,r3,r4,r5,r6,r7,r8
	ifnb	<r8>
	 mov	r7,r8
	endif
	ifnb	<r7>
	 mov	r6,r7
	endif
	ifnb	<r6>
	 mov	r5,r6
	endif
	ifnb	<r5>
	 mov	r4,r5
	endif
	ifnb	<r4>
	 mov	r3,r4
	endif
	ifnb	<r3>
	 mov	r2,r3
	endif
	mov	r1,r2
	endm

macro DOS r1,r2
	INTR	21h,<r1>,<r2>
	endm

macro VID r1,r2
	INTR	10h,<r1>,<r2>
	endm

macro DRV r1,r2
	INTR	13h,<r1>,<r2>
	endm

macro KBD r1,r2
	INTR	16h,<r1>,<r2>
	endm

macro MUX r1,r2
	INTR	2fh,<r1>,<r2>
	endm

macro LoadAX r1,r2
	ifb	<r2>
	 ifnb	<r1>
	  if	r1 ge 256
	   mov	ax,r1
	  else
	   mov	ah,r1
	  endif
	 endif
	else
	 ifb	<r1>
	  mov	al,r2
	 else
	  if (SYMTYPE r1) and (SYMTYPE r2) and 4
	   mov	ax,r1*256+r2
	  else
	   mov	ah,r1
	   mov	al,r2
	  endif
	 endif
	endif
	endm

macro INTR intno,r1,r2			;r1=ah, r2=al
	LoadAX	r1,r2
	int	intno
	endm

macro IPUSH	konst:req,reg:=<ax>
	if @Cpu and 2
	 push	konst
	else
	 mov	reg,konst
	 push	reg
	endif
	endm

macro PUSH8
	push	ax bx cx dx ds si es di
	endm

macro POP8
	pop	di es si ds dx cx bx ax
	endm

macro OUTIB dest,val
	ifdifi <val>,<al>
	 mov	al,val
	endif
	out	dest,al
	endm

macro OUTIW dest,val
	ifdifi <val>,<ax>
	 mov	ax,val
	endif
	out	dest,ax
	endm

macro DXSET dest,vv
	ifdifi <dest>,<dx>
	 mov	dx,dest		;;DX laden
	endif
	ifnb <vv>
	 if vv le 3
	  rept vv
	   inc	dx		;;DX um 1, 2 oder 3 inkrementieren
	  endm
	 else
	  add	dx,vv		;;sonst addieren
	 endif
	endif
	endm

macro OUTVB dest,val,vv
	DXSET	<dest>,<vv>
	ifdifi <val>,<al>
	 mov	al,val
	endif
	out	dest,al
	endm

macro OUTVW dest,val,vv
	DXSET	<dest>,<vv>
	ifdifi <val>,<ax>
	 mov	ax,val
	endif
	out	dest,ax
	endm

;Load Long
macro LDL regh:req,reg:req,src:rest
	local dp,sr
dp	instr	<src>,<[>
	errif dp ne 1	<missing [>	;wenn keine Klammer am Anfang
dp	sizestr <src>
sr	substr	<src>,2,dp-2
	mov	regh,[wo HIGH dwo sr]
	mov	reg,[wo LOW dwo sr]
	endm

macro LSSx reg:req,src:rest	;Load SS:reg from src
	ldl	ss,<reg>,src
	endm

;Store Long
macro STL dest:req,regh:req,reg:req
	local dll,dpp,dst
dpp	instr	<dest>,<[>
	if dpp eq 1		;;wenn Klammer am Anfang
dll	 sizestr <dest>
dst	 substr	<dest>,2,dll-2
	 mov	[wo LOW dwo dst],reg
	 mov	[wo HIGH dwo dst],regh
	else
	 mov	[wo LOW dwo dest],reg
	 mov	[wo HIGH dwo dest],regh
	endif
	endm

macro SDS dest:req,reg:rest
	stl	<dest>,ds,<reg>
	endm

macro SES dest:req,reg:rest
	stl	<dest>,es,<reg>
	endm

macro SSS dest:req,reg:rest	;Store SS:reg into dest
	stl	<dest>,ss,<reg>
	endm

macro SCS dest:req,reg:rest	;Store CS:reg into dest
	stl	<dest>,cs,<reg>
	endm

	;; Aufrufen mit DPS	'Hallo!',13,10
macro dps w1:rest
local dpsa,dpse
	;;Define Pascal String (with length byte)
	db	dpse-dpsa	;;generate warning if string too long!
dpsa:	ifnb <w1>
	 db	w1
dpse:	endif
	endm

macro dcl w1:rest		;;Definiere Kommandozeilen-String
	dps	w1
	db	13
	endm

macro	IRPS	action:req,p1:rest	;Restliche Einzelargumente
local	strq,a1
	irp	p,<p1>
	 ifnb	<p>
strq	  instr	<p>,<'>
	  if	strq eq 1
	   err	<"strings need double quote">
	  else
strq	   instr <p>,<">
	  endif		;nun strq 1 (") oder alles andere
	  if	strq ne 1
	   action p
	  else
a1	   =	0
	   irpc	c,<&p>
	    ifidn <c>,<">
a1	     =	a1+1	;ZÑhler erhîhen
	     if (a1 ne 1) and (a1 and 1)
	      action '&c'
	     endif
	    else
	     action "&c"
	    endif
	   endm
	  endif
	 endif
	endm
	endm		;Huch!

macro dxs x1:req,w1:rest		;Define Xored String
	irpc	c,<w1>
	 db	'&c' xor x1
	endm
	endm

;Achtung: Zahlenbasis von RADIX abhÑngig!
macro dcd x:req
 local	x1
x1	equ	%(&x&)
	dcn	x1
	endm

;fÅr z.B. DCN @Filename (ohne abschlie·ende Spaces wie bei ??Filename)
macro	dcn	x:req
 local	x1
x1	catstr	<'>,&x&,<'>
	db	x1
	endm

macro DVT c:req,w:req	;;Definiere Verteilertabelleneintrag
	db	c
	dw	ofs w
	endm

macro DZ str:rest	;;Definiere nullterminierten String
	ifnb	<str>
	 db	str
	endif
	db	0
	endm

macro entr w1			;wie ENTER beim 286
	if @CPU and 2
	 enter	w1,0
	else
	 push	bp
	 mov	bp,sp
	 ifnb <w1>
	  sub	sp,w1	;;lokale Variablen
	 endif
	endif
	endm

macro leav			;wie LEAVE beim 286
	if @CPU and 2
	 leave
	else
	 mov	sp,bp
	 pop	bp
	endif
	endm

macro _ochr			;Zeichenausgabe aus AL
ochr:	push	ax dx
	mov	dl,al
	DOS	2
	pop	dx ax
	ret
	endm

macro _axhx			;Hexzahlausgabe, VR: AX,F
axhx:	xchg	al,ah
	call	ahex
	xchg	al,ah
ahex:	push	ax cx		;Werte erhalten
	mov	cl,4		;oberes Nibble auf Bit 3...0
	shr	al,cl		; schieben
	pop	cx
	call	ahex1
	pop	ax
ahex1:	and	al,0fh
	add	al,90h		;Hex -> ASCII
	daa
	adc	al,40h
	daa
	_ochr
	endm

macro _ALDEZ			;AL zu Dezimal-String in AX wandeln
aldez:
	xor	ah,ah
	aam			;dividiert AL durch 10
	xchg	al,ah		;AH=Rest, Low-Teil, AL=High-Teil
	add	ax,'00'		;fertig zum Einpoken
	ret
	endm

macro _AXDEZ			;AX dezimal ausgeben
proc axdez
	push	ax cx dx
	xor	cx,cx		;VornullunterdrÅckung
	mov	bx,10000
	call	@@1 		;hinterlÑ·t in ax den Rest!
	mov	bx,1000
	call	@@1
	mov	bx,100
	call	@@1
	mov	bx,10
	call	@@1
	add	al,'0'
	call	ochr
	pop	dx cx ax
	ret
@@1:	;Ziffernausgabe, ax=Zahl, bx=Teiler, cx=Vornull-Flag
	xor	dx,dx		;High-Teil=0
	div	bx		;ax:=ax/bx, Rest dx (bx Dezimalzahl?!)
	push	dx
	or	cx,ax		;Evtl. Ziffer anmelden
	or	cx,cx		;Immer noch Vornull?
	jz	@@3		;Ziffer
	add	al,'0'
	call	ochr
@@3:	pop	ax		;Rest
	ret
	endp
	endm

macro _UPCASE
proc UpCase
	cmp	al,'a'
	jb	@@1
	cmp	al,'z'
	ja	@@1
	bres	al,bit 5
@@1:	ret
	endp
	endm

macro _TOUP		;Komfort-Upcase mit lÑnderspezifischer Umsetzung (?)
proc ToUp
	cmp	al,80h
	jc	@@e
	entr	20h
	push	bx cx dx ds ss
	pop	ds
	lea	dx,bp-20h
	push	ax	;Zeichencode
	DOS	3800h
	pop	ax
	jc	@@e1
	call	[dword bp-20h+12h]
@@e1:	pop	ds dx cx bx
	leav
@@e:	endp
	_UPCASE
	endm

macro _TOLOW
proc ToLow
	cmp	al,'A'
	jb	UpCas1
	cmp	al,'Z'
	ja	UpCas1
	or	al,20h
@@1	ret
	endp
	endm

_ANUM_USED=0
macro _ANUM			;stellt fest, ob AL eine "Ziffer" ist
				;ZulÑssig: 0..9, A..Z, a..z
proc Anum			;gemopst von CAOS NT
	;A numerisch wandeln
	;PE: A-ASCII-Code
	;PA: A: Zahl, die A reprÑsentierte
	;A>=36 wenn nicht im zulÑssigen Bereich
	;VR: AF
	SUB	al,30H
	jc	@@e
	CMP	al,10
	jc	@@e
	SUB	al,11H
	AND	al,not 20h
	ADD	al,10
@@e:	RET
	endp
_ANUM_USED=1
	endm

macro _INW	;Liest Word ein PE: BL: Zahlenbasis
	;PA: CY=1: Gar keine Ziffern zum Einlesen oder Zahl zu gro·
	;SI zeigt aufs erste falsche Zeichen
	;(Auswertung desselben ist Sache des Hauptprogramms!)
proc InW c
 uses bx,cx,dx
	xor	cx,cx
	mov	bh,ch		;Null
	mov	al,[si]
	call	Anum
	cmp	al,bl
	cmc
	jc	@@e		;;Fehler
@@1:	mov	ah,0
	xchg	ax,cx		;;bisherige Zahl nach AX, neue nach CX
	mul	bx		;;DXAX=BX*AX
	add	dx,-1
	jc	@@e		;;Fehler: Zahl zu gro·
	add	cx,ax		;;Zur neuen Ziffer bl*bisherige dazu
	jc	@@e
	inc	si
	mov	al,[si]
	call	Anum
	cmp	al,bl
	jc	@@1		;;wenn Ziffer klein genug
@@e:	xchg	ax,cx
	ret
	endp
if _ANUM_USED eq 0		;;mal sehen ob's geht!
	_ANUM
endif
	endm

macro _INW3			;;C-mÑ·ige Zahlenauswertung dazu,
				;;Dezimalvorgabe! VR:BL
InW3:	mov	bl,10		;;Immer dezimal!
	cmp	[by si],'0'
	jnz	inw3e
	mov	bl,8
	inc	si
	cmp	[by si],'x'
	jz	inw3a
	cmp	[by si],'X'
	jz	inw3a
	_INW2
	endm

macro _INW2	;wie oben, jedoch PrÑfixauswertung wie folgt:
	;BL: Default-Basis (meist 10 oder 16)
	;am Anfang #: Immer dezimal (Override-PrÑfixe hei·en die Dinger!)
	;	   $: Immer hex
InW2:	cmp	[by si],'#'
	jnz	inw2b
	mov	bl,10
	jr	inw2a
inw2b:	cmp	[by si],'$'
	jnz	inw2e
inw3a:	mov	bl,16
inw2a:	inc	si
inw3e:
inw2e:
	_INW
	endm

macro _CHKWS
proc ChkWs	;Check for WhiteSpace, Z=1: Es ist welcher
	or	al,al
	jz	@@e
	cmp	al,9
	jz	@@e
	cmp	al,' '
	jz	@@e
	cmp	al,13
	jz	@@e
	cmp	al,10
@@e:	ret
	endp
	endm

macro _CRLF
crlf:	push	ax
	mov	al,13
	call	ochr
	mov	al,10
	call	ochr
	pop	ax
	ret
	endm

macro _CASE	;fÅhrt Case-Anveisung via Tabelle durch
proc case	;PE: ES(!):DI: Tabelle, AL: Zeichen (Byte); CY=1: Zeichen nicht
		;enthalten; dann zeigt DI auf ELSE-Zweig
		;Endekennung der Tabelle: Null-Byte! (leichte EinschrÑnkung)
@@r:	cmp	[by es:di],1
	jc	@@3
	scasb
	jz	@@2	;CY=0!
	scasw		;NÑchstes Wort Åbergehen
	jr	@@r
@@3:	inc	di
@@2:	ret
	endp
	endm

macro _UPV	;zieht nicht mehr _CASE nach sich!
ifndef case
	_CASE
endif
ifndef Upcase
	_UPCASE
endif
proc upv	;Unterprogrammverteiler nach Tabelle ES:DI
		;Holt sich ein Zeichen ab [si] und fÅhrt Programm nach
		;Tabelle aus
	cld
	lodsb
	call	UpCase
	call	Case	;nach DI
	jc	@@2
	jmp	[wo es:di]	;UP rufen; CY bedeutet Abbruchs-Erzwingung,
			;z.B. Fehler oder Hilfeseite, AX=Code!
			;AX=0: Nur Abbruch, keine zentrale Fehlermeldung
			;AX=1: Fehler in Kommandozeile, SI zeigt auf
			;unpassendes Zeichen
@@2:	mov	ax,1
	dec	si	;Pointer zurÅck
	ret
	endp
	endm

macro IS286			;Is at least 80286?
	push	sp		;;continue with: jc WrongProcessor
	pop	ax
	cmp	ax,sp		;;ax less sp?
	endm

macro IS386
 local	isn286
	IS286			;Is at least 80386?
	jc	isn286
	mov	ax,7000h
	push	ax
	popf
	pushf
	pop	ax
	and	ax,7000h
	sub	ax,1		;Z->CY
isn286:
	endm

macro RESFLAG dest:req,flg:rest
	MASKFLAG dest,not (flg)
	endm
		;Alignment with Value
macro ALIGNV w1:=<16>,w2:=<?>
 local	w3
	errife w1 GT 0
w3=	w1- (($-PSPOrg) MOD w1)
	if w3 NE w1
	 db	w3 dup (w2)
	endif
	endm

macro MAX r2:req,r1:req
 local	ziel
	cmp	r2,r1
	jnc	ziel
	mov	r2,r1
ziel:
	endm

macro MIN r2:req,r1:req
 local	ziel
	cmp	r2,r1
	jc	ziel
	mov	r2,r1
ziel:
	endm

macro INCB reg:req
 local	ziel
	inc	reg
	jnz	ziel
	dec	reg
ziel:
	endm

macro DECB reg:req
 local	ziel
	if (SYMTYPE reg) and 10h	;;Register?
	 or	reg,reg
	else
	 cmp	reg,0
	endif
	jz	ziel
	dec	reg
ziel:
	endm

macro ENDC str:=<COMentry>	;;komfortable End-Anweisung
	end	str		;;Diese Konstruktion vermeidet Warnings
	endm

;+++ Definitionen von DOS-Strukturen +++
;tFCB	der Dateisteuerblock	;machen wir mal spÑter

struc tDTA	;die Disk Transfer Area
 resDrv    db	?
 resName   db	8 dup (?)
 resExt	   db	3 dup (?)
 resAttr   db	?
 resDirNo  dw	?
 resClus   dw	?
 res?	   dd	?
 Attr	   db	?
 union
  DateTime dd	?
  struc
   Time    dw	?
   Date    dw	?
  ends
 ends
 Size	   dd	?
 FName	   db	13 dup (?)
ends tDTA

struc tPSP	;der Programmsegment-PrÑfix
 Int20		dw	?
 NextMem	dw	?
 IOByte		db	?	;frei (enthielt zu CP/M-Zeiten das IOByte!)
 JmpInt21	db	?
 MemInt21	dd	?
 MemInt22	dd	?
 MemInt23	dd	?
 MemInt24	dd	?
 PPSP		dw	?	;Vater-PSP
 HandleTab	db	20 dup (?) ;fÅr - siehe da - 20 Handles!
 EnvSeg		dw	?
 pSyStack	dd	?
 MaxOpen	dw	?
 pHandleTab	dd	?
 res1		db	24 dup (?) ;braucht QEMM
 LongInt21	db	3 dup (?)  ;fÅr ein Call Far (Wer macht denn SOWAS?)
 res2		db	9 dup (?)
 FCB1		db	16 dup (?)
 FCB2		db	20 dup (?) ;Was soll denn DAS!?
 union
  CmdLine	db	80h dup (?)
  StdDTA	tDTA	<>
 ends
ends

struc tExecBlock
 EnvSeg	dw	0
 CmdTail dd	80h
 FCB1	dd	5ch
 FCB2	dd	6ch
ends tExecBlock

struc tDirE	;der Directoryeintrag auf der Platte
 FName	db	8 dup (?)
 Ext	db	3 dup (?)
 Attr	db	?
 res	db	10 dup (?)
 union
  DateTime dd	?
  struc
   Time dw	?
   Date dw	?
  ends
 ends
 Clust	dw	?
 Size	dd	?
ends

struc tBigDos	;der Anforderungsblock fÅr Int25 und In26 bei BigDOS-Partitionen
 Adr	dd	?
 Secs	dw	?
 Sec1	dd	?
ends

struc tDevHdr	;defaultmÑ·ig ein NUL Device Header
;;Einbau mit "DevHdr tDevHdr <,,ofs Strat,ofs Inter,'MYDEV$'>
 Next	   dd	-1	;NÑchster Treiber, wird von DOS eingetragen
 Attr	   dw	8004h
 pStrat    dw	?
 pIntr	   dw	?
 pName	   db	'NUL     '
ends

	%LIST
	CODESEG
PSPOrg:	;eine Marke Wert 0 aber verschieblich weil im Codesegment definiert
	org	100h
COMentry:
;	ENDC			;Semikolon nur zu Testzwecken entfernen!
