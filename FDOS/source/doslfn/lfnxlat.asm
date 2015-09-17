;Umsetzer der LFN-API von DOSLFN (und dritter DOS-Treiber fÅr lange Dateinamen
;zum Protected Mode fÅr Windows und alle DOS-Boxen
;Fehlt noch: LoSA
;haftmann#software, 12/02
	.386p
	%NOLIST
	INCLUDE TVMM.INC
	%NOLIST
	INCLUDE Debug.INC
	INCLUDE V86MMGR.INC
	%LIST
	MASM51
	LOCALS

Declare_Virtual_Device LFNXLAT,1,0,LFNXLAT_Control

;****************
;** XLAT-Daten **
;****************
VxD_DATA_SEG
LFNCall_Table:
LFNCall_move:
	Xlat_API_ASCIIZ		es,di
LFNCall_Type_DSDX:	;mkdir,rmdir,chdir,unlink,attr
	Xlat_API_ASCIIZ		ds,dx
	Xlat_API_Exec_Int	21h

LFNCall_ffirst:
	Xlat_API_ASCIIZ		ds,dx
LFNCall_fnext:
	Xlat_API_Fixed_Len	es,di,317	;=sizeof(TWin32FindData)
LFNCall_fclose:
	Xlat_API_Exec_Int	21h

LFNCall_pwd:
	Xlat_API_Fixed_Len	ds,si,256	;weil ohne Laufwerk!?
	Xlat_API_Exec_Int	21h

LFNCall_name:
	Xlat_API_Calc_Len	es,di,CalcLen_name
LFNCall_creat:
	Xlat_API_ASCIIZ		ds,si
	Xlat_API_Exec_Int	21h

LFNCall_volinfo:
	Xlat_API_ASCIIZ		ds,dx
	Xlat_API_Var_Len	es,di,cx
	Xlat_API_Exec_Int	21h

LFNCall_handleinfo:
	Xlat_API_Fixed_Len	ds,dx,34h	;nicht in DOSLFN.COM
	Xlat_API_Exec_Int	21h

LFNCall_timeconv:
	Xlat_API_Fixed_Len	ds,si,8
	Xlat_API_Exec_Int	21h

LFNCall_genshort:
	Xlat_API_ASCIIZ		ds,si
	Xlat_API_Calc_Len	es,di,CalcLen_genshort
	Xlat_API_Exec_Int	21h

LFNCall_subst_query:
	Xlat_API_Fixed_Len	ds,dx,260	;nicht in DOSLFN.COM
	Xlat_API_Exec_Int	21h

DosFunc_71:
	db	39h
	db	3Ah
	db	3Bh
	db	41h
	db	43h
	db	47h
	db	4Eh
	db	4Fh
	db	56h
	db	60h
	db	6Ch
	db	0A0h
;	db	0A1h		;Default!
	db	0A6h
	db	0A7h
	db	0A8h
;	db	0A9h		;Nur fÅr Real-Mode Server
;	db	0AAh		;Extrawurst!
Num_DosFunc_71	= $-DosFunc_71	;=15
;Diese Offsets brauchen weniger Platz als Adressen - und mÅssen nicht
;reloziert werden! Die .386-Grî·e sinkt so enorm
	db	LFNCall_Type_DSDX	-LFNCall_Table
	db	LFNCall_Type_DSDX	-LFNCall_Table
	db	LFNCall_Type_DSDX	-LFNCall_Table
	db	LFNCall_Type_DSDX	-LFNCall_Table
	db	LFNCall_Type_DSDX	-LFNCall_Table
	db	LFNCall_pwd		-LFNCall_Table
	db	LFNCall_ffirst		-LFNCall_Table
	db	LFNCall_fnext		-LFNCall_Table
	db	LFNCall_move		-LFNCall_Table
	db	LFNCall_name		-LFNCall_Table
	db	LFNCall_creat		-LFNCall_Table
	db	LFNCall_volinfo		-LFNCall_Table
;	db	LFNCall_fclose		-LFNCall_Table
	db	LFNCall_handleinfo	-LFNCall_Table
	db	LFNCall_timeconv	-LFNCall_Table
	db	LFNCall_genshort	-LFNCall_Table
;	db	LFNCall_creat		-LFNCall_Table

Xlat_71AA:
	db	LFNCall_Type_DSDX	-LFNCall_Table	;BH=0
	db	LFNCall_fclose		-LFNCall_Table	;BH=1
	db	LFNCall_subst_query	-LFNCall_Table	;BH=2

OldInt21_Ofs	dd	?
OldInt21_Sel	dd	?

VxD_DATA_ENDS


VxD_ICODE_SEG

BeginProc LFNXLAT_Sys_Critical_Init
	Debug_Out "LFNXLAT: installing Int21 hook"
	mov	al,21h
	VMMcall Get_PM_Int_Vector
	mov	[OldInt21_Ofs],edx
	mov	[OldInt21_Sel],ecx
	mov	esi,OFFSET32 LFNXLAT_Int21
	push	eax
	 VMMcall Allocate_PM_Call_Back
	 movzx	edx,ax
	 xchg	ecx,eax
	pop	eax
	jc	@@NoPM
	shr	ecx,10h
	VMMcall Set_PM_Int_Vector
	clc
	ret
@@NoPM:
	debug_out "LFNXLAT: Could not allocate PM call back"
	VMMjmp	Fatal_Memory_Error

EndProc LFNXLAT_Sys_Critical_Init

VxD_ICODE_ENDS


VxD_LOCKED_CODE_SEG

BeginProc LFNXLAT_Control
	Control_Dispatch Sys_Critical_Init, LFNXLAT_Sys_Critical_Init
	clc
	ret
EndProc LFNXLAT_Control

VxD_LOCKED_CODE_ENDS


VxD_CODE_SEG

BeginProc CalcLen_name
	LD	ecx,80
	cmp	[ebp.Client_CL],1
	jz	@@e
	mov	ecx,260
@@e:	ret
EndProc CalcLen_name

BeginProc CalcLen_genshort
	LD	ecx,11
	cmp	[ebp.Client_DH],0
	jz	@@a
	mov	cl,13
@@a:	cmp	[ebp.Client_DL],20h
	jb	@@e
	add	ecx,ecx		;bei Unicode-RÅckgabe 22 oder 26 Bytes
@@e:	ret
EndProc CalcLen_genshort

BeginProc LFNXLAT_Int21
;Sollte man hier auch die API zum Sektorzugriff auf FAT32 (AX=7305) umsetzen?
;Das ist eigentlich Aufgabe eines noch zu schreibenden DOS7XLAT.386...
	mov	eax,[ebp.Client_EAX]
	cmp	ah,71h
	jz	@@lfn
	mov	edx,[OldInt21_Ofs]
	mov	ecx,[OldInt21_Sel]
	VMMjmp	Simulate_Far_Jmp	;ans Standard-DPMI weiterleiten
@@lfn:
	Debug_Out "LFNXLAT: Int21 AH=71 called, doing translation?"
	VMMcall Simulate_Iret		;Interrupt "verspeisen"
	cmp	al,0AAh			;Subst-Funktionen
	je	@@subst
	mov	esi,offset32 LFNCall_Table;alles relativ dazu verkÅrzt Code
	LD	edx,LFNCall_fclose-LFNCall_Table
	lea	edi,[esi+DosFunc_71-LFNCall_Table]
	LD	ecx,Num_DosFunc_71
	repne	scasb
	jnz	@@notrans
	add	edi,Num_DosFunc_71-1	;auf Offset-Byte
@@trans:
	movzx	edx,byte ptr[edi]
@@notrans:
	add	edx,esi
	VxDjmp	V86MMGR_Xlat_API
@@subst:
	movzx	edi,[ebp.Client_BH]
	cmp	edi,2
	ja	@@notrans
	lea	edi,[edi+esi+Xlat_71AA-LFNCall_Table]
	jmp	@@trans
EndProc LFNXLAT_Int21

VxD_CODE_ENDS


	END
