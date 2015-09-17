program l;
{longname: Programm zur AusfÅhrung der Int21/AH=71-API}
{Bequemer: Man benutzt DOS7, ggf. patchen und darauf Win3.x starten}
uses parser,strings,WinDos;
var
 sp: PChar;
 kdo, arg: PChar;
 s: array[0..259] of Char;
 DosError: Integer;
{eine Eingabe- und eine Ausgabevariable fÅr W32FindFirst/NextFile}
const
 DosTimeFormat:Integer=1; {0=Win32, 1=DOS}
var
 UnicodeConversion:Byte;	{Bit 1: Nicht konvertierbare Unicodes}
	{Bit 0: Nicht konvertierbare OEM-Kodes: tritt unter DOS nicht auf}

type
 Bool=WordBool;
 ELH=(lo,hi);
 LongRec=record lo,hi:Word; end;
 LongLong=array[ELH] of LongInt;
 TWin32FindData=record
  attr: LongInt;
  timec: LongLong;
  timea: LongLong;
  timem: LongLong;
  sizeh,sizel: LongInt;
  rsv: LongLong;
  namel: array[0..259] of Char;
  names: array[0..13] of Char;
 end;

function FindFirst(filter:PChar; attr:Word; var fd:TWin32FindData):Word;
 assembler; asm
	push	ds
	 mov	si,[DosTimeFormat]
	 lds	dx,[filter]
	 mov	cx,[attr]
	 les	di,[fd]
	 stc
	 mov	ax,714Eh
	 int	21h
	pop	ds
	mov	[UnicodeConversion],cl
	jnc	@@e
	mov	[DosError],ax
	xor	ax,ax
@@e: end;

function FindNext(h: Word; var fd: TWin32FindData):Bool; assembler;
 asm	mov	bx,[h]
	mov	si,1
	les	di,[fd]
	mov	si,[DosTimeFormat]
	stc
	mov	ax,714fh
	int	21h
	mov	[UnicodeConversion],cl
	jnc	@@e
	mov	[DosError],ax
	xor	ax,ax
@@e: end;

function FindClose(h: Word):Bool; assembler;
 asm	mov	bx,[h]
	stc
	mov	ax,71A1h
	int	21h
	jnc	@@e
	mov	[DosError],ax
	xor	ax,ax
@@e: end;

procedure PutAttr(a:Word);
{schreibt 6 Zeichen}
 const
  astr:PChar='rhsvda';
 var
  i:Integer;
 begin
  for i:=5 downto 0 do
  if a and (1 shl i) <>0
  then Write(astr[i])
  else Write('-');
 end;

procedure PutDateTime(time:LongInt; dateonly:Boolean);
{schreibt 12 Zeichen}
 var
  dt: TDateTime;
 begin
  if time=0 then begin
   Write('------');
   if not dateonly then write('------');
   exit;
  end;
  UnpackTime(time,dt);
  Write(dt.year mod 100:2,dt.month:2,dt.day:2);
  if not dateonly then
  Write(dt.hour:2,dt.min:2,dt.sec:2);
 end;

procedure dir; far;
 var
  fh: Word;
  fd: TWin32FindData;
 begin
  DosError:=0;
  fh:=FindFirst(arg,$3F,fd);
  if (DosError=0) and (fh<>0) then begin
   WriteLn('ATTRIB DTIME_CREATE DTIME_MODIFY DATE_A FILESIZE _______ALIAS LFN...');
   repeat
    PutAttr(fd.attr); Write(' ');
    PutDateTime(fd.timec[lo],false); Write(' ');
    PutDateTime(fd.timem[lo],false); Write(' ');
    PutDateTime(fd.timea[lo],true);
    Write(fd.sizel:9,' ');
    Write(fd.names:12,' ');
    WriteLn(fd.namel);
   until not FindNext(fh,fd);
   DosError:=0;
   FindClose(fh);
  end;
 end;

procedure Doscall_Type_DSDX(_ax,_bx,_cx,_si:Word; esdi:Pointer);
{Aufrufe mit DS:DX=Name (arg) und weiteren Registern}
 begin
  asm	push	ds
	 lds	dx,[arg]
	 les	di,[esdi]
	 mov	si,[_si]
	 mov	cx,[_cx]
	 mov	bx,[_bx]
	 mov	ax,[_ax]
	 stc
	 int	21h
	pop	ds
	jnc	@@e
	mov	[DosError],ax
@@e:
  end;
 end;

procedure Doscall_Type_DSSI_ESDI(_ax,_bx,_cx,_dx:Word);
{Aufrufe mit DS:SI=Name und ES:DI=Ergebnispuffer}
 begin
  asm	push	ds
	 push	ds
	 pop	es
	 lds	si,[arg]
	 lea	di,[s]
	 mov	dx,[_dx]
	 mov	cx,[_cx]
	 mov	bx,[_bx]
	 mov	ax,[_ax]
	 stc
	 int	21h
	pop	ds
	jnc	@@e
	mov	[DosError],ax
@@e:
  end;
  if DosError=0 then WriteLn(s);
 end;

procedure touch; far;
 var
  time: LongInt;
  dt: TDateTime;
  dow: Word;
 begin
  GetDate(dt.year,dt.month,dt.day,dow);
  GetTime(dt.hour,dt.min,dt.sec,dow);
  PackTime(dt,time);
  DosCall_Type_DSDX($7143,0,LongRec(time).lo,0,Pointer(LongRec(time).hi));
 end;

procedure chdir; far;
 begin
  if arg[0]<>#0 then begin	{chdir}
   asm	push	ds
	 lds	dx,[arg]
	 mov	ax,713Bh
	 int	21h
	pop	ds
	jnc	@@e
	mov	[DosError],ax
@@e:
   end
  end else begin		{pwd}
   asm	mov	ah,19h
	int	21h
	add	al,'A'
	mov	byte ptr [s],al
	mov	word ptr [s+1],'\:'
	mov	dl,0
	mov	si,offset s+3
	mov	ax,7147h
	int	21h
	jnc	@@e
	mov	[DosError],ax
@@e:
   end;
   WriteLn(s);
  end;
 end;

procedure noop; far; assembler;
 asm
 end;

procedure truename; far;
 begin
  DosCall_Type_DSSI_ESDI($7160,0,0,0);
 end;

procedure shortname; far;
 begin
  DosCall_Type_DSSI_ESDI($7160,0,1,0);
 end;

procedure longname; far;
 begin
  DosCall_Type_DSSI_ESDI($7160,0,2,0);
 end;

procedure truename2; far;
 begin
  DosCall_Type_DSSI_ESDI($7160,0,$8000,0);
 end;

procedure shortname2; far;
 begin
  DosCall_Type_DSSI_ESDI($7160,0,$8001,0);
 end;

procedure longname2; far;
 begin
  DosCall_Type_DSSI_ESDI($7160,0,$8002,0);
 end;

procedure genshort; far;
 begin
  DosCall_Type_DSSI_ESDI($71A8,0,0,$111);
 end;

procedure genshort_FCB; far;
 begin
  s[11]:=#0;			{Hand-Terminierung}
  DosCall_Type_DSSI_ESDI($71A8,0,0,$011);
 end;

procedure mkdir; far;
 begin
  DosCall_Type_DSDX($7139,0,0,0,nil);
 end;

procedure rmdir; far;
 begin
  DosCall_Type_DSDX($713A,0,0,0,nil);
 end;

procedure creat; far; assembler;
 asm	push	ds
	 lds	si,[arg]
	 mov	dx,21h
	 mov	cx,1	{gemein: SchreibgeschÅtzt!}
	 mov	bx,0
	 mov	ax,716Ch
	 stc
	 int	21h
	pop	ds
	jnc	@@ok
	mov	[DosError],ax
	jmp	@@e
@@ok:	xchg	bx,ax
	mov	ah,3Eh
	int	21h
	jnc	@@e
	mov	[DosError],ax
@@e:
 end;

procedure unlink; far;
 begin
  DosCall_Type_DSDX($7141,0,0,0,nil);
 end;

procedure unlink2; far;
 begin
  DosCall_Type_DSDX($7141,0,0,1,nil);
 end;

procedure move; far;
 begin
  DosCall_Type_DSDX($7156,0,0,0,NextItem(sp,Word('"'),DELIM_Whitespace));
 end;

procedure timeconv; far;
 begin
 end;

procedure attr; far;
 var
  a: Word;
 begin
  DosCall_Type_DSDX($7143,0,0,0,nil);
  asm	mov	[a],cx
  end;
  if DosError=0 then begin
   PutAttr(a);
   WriteLn;
  end;
 end;

const
 cmds:array[0..20] of PChar=(
  'cd','chdir',		{47h,3Bh}
  'dir',		{4Eh,4Fh,A1h}
  'truename',		{60h,CL=0}
  'shortname',		{60h,CL=1}
  'longname',		{60h,CL=2}
  'truename2',		{60h,CL=0,CH=80}
  'shortname2',		{60h,CL=1,CH=80}
  'longname2',		{60h,CL=2,CH=80}
  'genshort',		{A8h}
  'genshort_FCB',	{A8h}
  'mkdir',		{39h}
  'rmdir',		{3Ah}
  'creat',		{6Ch (+3Eh)}
  'del','rm',		{41h}
  'ren','move',		{56h}
  'timeconv',		{A7h}
  'attr',		{43h}
  'touch');		{43h}
 prgs:array[LOW(cmds)..HIGH(cmds)] of procedure=(
  chdir,chdir,
  dir,
  truename,
  shortname,
  longname,
  truename2,
  shortname2,
  longname2,
  genshort,
  genshort_FCB,
  mkdir,
  rmdir,
  creat,
  unlink2,unlink,
  move,move,
  timeconv,
  attr,
  touch);

var
 i: Integer;

begin
 sp:=Ptr(PrefixSeg,$80);
 sp[byte(sp^)+1]:=#0;	{nullterminieren}
 Inc(sp);
 DosError:=0;
 kdo:=NextItem(sp,Word('"'),DELIM_Whitespace);
 arg:=NextItem(sp,Word('"'),DELIM_Whitespace);

 for i:=LOW(cmds) to HIGH(cmds) do begin
  if stricomp(kdo,cmds[i])=0 then begin
   prgs[i];
   if DosError<>0
   then WriteLn('error code (hi:lo decimal): ',
     DosError shr 8,':',DosError and $FF);
   exit;
  end;
 end;

 writeln('unknown command ',kdo);
 for i:=LOW(cmds) to HIGH(cmds) do begin
  write(cmds[i],' ');
 end;
 writeln;
end.
