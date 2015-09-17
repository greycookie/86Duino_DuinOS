{
  mvalues.pas - Determine the sizes to use for DOSLFN's m[nls] switches.

  Jason Hood, 29 & 30 June, 2004.
}

{$G+}
program mvalues;

uses
  dos, strings;

var
  FindData:record
	     Attr:  byte;
	     stuff: array[1..43]  of byte;
	     Long:  array[0..259] of char;
	     Short: array[0..13]  of char;
	   end;
  LongName:string;
  ShortName:string[12];
  FindDone:boolean;
  R:Registers;

procedure FillFindData;
begin
  R.ES:=Seg(FindData);
  R.DI:=Ofs(FindData);
  MsDos(R);
  if (R.Flags and fCarry)<>0 then
    FindDone:=true
  else
  begin
    LongName:=StrPas(FindData.Long);
    if FindData.Short[0]=#0 then
      ShortName:=LongName
    else
      ShortName:=StrPas(FindData.Short)
  end;
end;

function LFNFindFirst(const FileName:string):word;
var
  i:byte;
  p:array[0..256] of char;
begin
  i:=Length(FileName);
  Move(FileName[1], p, i);
  p[i]:='*';
  p[i+1]:=#0;
  R.AX:=$714E;
  R.CX:=$37;
  R.SI:=1;
  R.DS:=Seg(p);
  R.DX:=Ofs(p);
  FindDone:=false;
  FillFindData;
  LFNFindFirst:=R.AX
end;

procedure LFNFindNext(Handle:word);
begin
  R.AX:=$714F;
  R.BX:=Handle;
  R.SI:=1;
  FillFindData;
end;

procedure LFNFindClose(Handle:word);
begin
  R.AX:=$71A1;
  R.BX:=Handle;
  MsDos(R);
  FindDone:=(R.AX=$7100);	{ used to test for presence of LFN }
end;

var
  mn,ml,ms:byte;
  name,pathl,paths:string;
  drives:string[32];
  lpath,spath:string;
  i:byte;

procedure find;
var
  h:word;
  ll,sl:byte;
begin
  ll:=Length(lpath);
  if ll<79 then
    write(lpath,' ':79-ll,#13)
  else
    write(Copy(lpath,ll-79+1,79),#13);
  h:=LFNFindFirst(lpath);
  while not FindDone do
  begin
    if Length(LongName)>mn then
    begin
      mn:=Length(LongName);
      name:=LongName+' - '+lpath;
    end;
    if Length(lpath)+Length(LongName)>ml then
    begin
      pathl:=lpath+LongName;
      ml:=Length(pathl);
    end;
    if Length(spath)+Length(ShortName)>ms then
    begin
      paths:=spath+ShortName;
      ms:=Length(paths);
    end;
    if (FindData.Attr and 16)<>0 then
    begin
      if (LongName<>'.') and (LongName<>'..') then
      begin
	ll:=Length(lpath);
	sl:=Length(spath);
	lpath:=lpath+LongName+'\';
	spath:=spath+Shortname+'\';
	find;
	lpath[0]:=Chr(ll);
	spath[0]:=Chr(sl);
      end;
    end;
    LFNFindNext(h);
  end;
  LFNFindClose(h);
end;

begin
  if ParamCount=1 then
  begin
    if (ParamStr(1)='?') or (ParamStr(1)='/?') or (ParamStr(1)='-?') then
    begin
      writeln('MVALUES scans the directory tree to find the longest long name,');
      writeln('the longest long path and the longest short path for DOSLFN.');
      writeln('Specify the drives to scan as one string (eg: ''mvalues cd'');');
      writeln('if no drive is given, C: will be used.');
      halt
    end
    else
      drives:=ParamStr(1)
  end
  else
    drives:='C';
  LFNFindClose(0);
  if FindDone then
  begin
    writeln('It would help to have the LFN functions available.');
    halt;
  end;
  mn:=0;
  ml:=0;
  ms:=0;
  for i:=1 to Length(drives) do
  begin
    lpath:=UpCase(drives[i])+':\';
    spath:=lpath;
    find;
  end;
  write(' ':79,#13);
  writeln('Longest long name is ', mn, ' characters:');
  writeln(name);
  writeln('Longest long path is ', ml, ' characters:');
  writeln(pathl);
  writeln('Longest short path is ', ms, ' characters:');
  writeln(paths);
  writeln;
  writeln('Minimum recommended settings: /mn', mn+13,
				       ' /ml', ml+13,
				       ' /ms', ms+13);
end.
