unit parser;
{Parsen von Kommandozeilen u.Ñ. "komma-separierten" Listen}
{$C MOVEABLE PRELOAD PERMANENT}
{$F-,A+,G+,K+,W-}
{$I-,Q-,R-,S-}
{$V+,B-,X+,T+,P+}
interface
const
 DELIM_NoTrimLeft=$100;
 DELIM_NoTrimRight=$200;
 DELIM_NilWhenNone=$400;	{liefert NIL, wenn KEIN Element mehr da ist}
 DELIM_Whitespace=$800;
function NextItem(var s: PChar; escap, delim:Word):PChar;
{escap und delim teilen sich in Low-Byte und High-Byte auf:
 escap.lo=einleitendes Flucht-Zeichen (Åblich '"', 0 wenn keine Flucht)
 escap.hi=ausleitendes Flucht-Zeichen (0 wenn gleich einleitendem)
 delim.lo=Trennzeichen, 0 fÅr keins (nur trimmen)
 delim.hi=DELIM_xxx-Bits siehe oben
 Entsprechend der Kommandozeilen-Zusammensetzungs-Strategie der COMMAND.COM
 wird z.B. < "Lange Datei"".""xyz" > zu <Lange Datei.xyz> gewandelt,
 d.h. Flucht-Zeichen schalten hin und her und kînnen selbst nicht mit
 Åbergeben werden, falls beide gleich sind
 Der String, auf den s zeigt, wird "zerstîrend gelesen", die via
 Returnwert (der immer gleich s beim Aufruf ist) Zeichenkette
 ist auch nach mehrfachem Aufruf nicht kaputt.}

function OneItem(d,s: PChar; escap, delim:Word):PChar;
{eine Modifikation, die als RÅckgabewert den vorgerÅckten Zeiger hat
 d darf NIL sein; dann wird nur geparst
 (z.B. zum ZÑhlen der Argumente vor einer Speicheranforderung)}

implementation

procedure IsWS; assembler;
{testet AL auf Wei·raum, d.h. $09, $0A, $0D, $20}
 asm	cmp	al,09h
	jz	@@e
	cmp	al,0Ah
	jz	@@e
	cmp	al,0Dh
	jz	@@e
	cmp	al,' '
@@e:	end;

function OneItem(d,s: PChar; escap, delim:Word):PChar; assembler;
 asm	push	ds
	 cld
	 lds	si,[s]
	 les	di,[d]
	 mov	cx,ds
	 jcxz	@@e	{wenn da NIL drinsteht, immer durchreichen!}

	 mov	cx,[escap]
	 or	ch,ch
	 jnz	@@1
	 or	cl,cl
	 jz	@@1
	 mov	ch,cl	{gleich machen wenn CH=0 und CL<>0}
@@1:
	 mov	dx,[delim]
	 mov	bx,di	{Abhack-Zeiger fÅr TrimRight}
	 test	dh,1	{NoTrimLeft?}
	 jnz	@@outquoteloop
@@trimleft:
	 lodsb; or al,al; jz @@raus
	 call	IsWS
	 jz	@@trimleft
	 jmp	@@oq1

@@oq3:	 mov	bx,di	{Abhackposition vorrÅcken}
@@outquoteloop:
	 lodsb; or al,al; jz @@raus
@@oq1:	 cmp	al,cl
	 je	@@inquoteloop
	 test	dh,8	{Whitespace als Trennzeichen?}
	 jnz	@@oq2
	 cmp	al,dl
	 je	@@raus_sep
	 push	cx
	  mov	cx,es
	  jcxz	@@no1
	  stosb
@@no1:	 pop	cx
	 test	dh,2	{NoTrimRight?}
	 jnz	@@oq3
	 call	IsWS
	 jz	@@outquoteloop
	 jmp	@@oq3
@@oq2:
	 call	IsWS
	 jz	@@raus_sep
	 push	cx
	  mov	cx,es
	  jcxz	@@no2
	  stosb
@@no2:	 pop	cx
	 jmp	@@oq3

@@inquoteloop:
	 lodsb; or al,al; jz @@raus
	 cmp	al,ch
	 je	@@outquoteloop
	 push	cx
	  mov	cx,es
	  jcxz	@@no3
	  stosb
@@no3:	 pop	cx
	 mov	bx,di	{hier jeden Wei·raum mitnehmen!}
	 jmp	@@inquoteloop

@@raus:		{ohne Separator: NIL in S einspeichern lassen!}
	 dec	si	{wieder AUF DIE NULL zurÅckstellen}
	 test	dh,4
	 jz	@@raus_sep
	 xor	si,si
	 mov	ds,si		{DS:SI=0}
@@raus_sep:
	{Rechts-Trimmung ausfÅhren, gleichzeitig terminieren}
	 mov	byte ptr es:[bx],0
	{vorgerÅcktes S zurÅckliefern}
@@e:	 xchg	si,ax
	 mov	dx,ds
	pop	ds
 end;

function NextItem(var s: PChar; escap, delim:Word):PChar; assembler;
 asm	les	di,[s]
	les	di,es:[di]
	push	es
	push	di
	 push	es
	 push	di
	 push	es
	 push	di
	 push	[escap]
	 push	[delim]
	 push	cs
	 call	near ptr OneItem
	 les	di,[s]
	 stosw
	 xchg	dx,ax
	 stosw
	pop	ax
	pop	dx
 end;

end.
