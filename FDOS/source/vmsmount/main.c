/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *  Copyright (C) 2011  Eduardo Casino
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 *
 * Acknowledgements:
 *    "Undocumented DOS 2nd ed." by Andrew Schulman et al.
 *    Ken Kato's VMBack info and Command Line Tools
 *                    (http://sites.google.com/site/chitchatvmback/)
 *    VMware's Open Virtual Machine Tools
 *                    (http://open-vm-tools.sourceforge.net/)
 *    Tom Tilli's <aitotat@gmail.com> TSR example in Watcom C for the 
 *                    Vintage Computer Forum (www.vintage-computer.com)
 *  
 * TODO:
 *
 *   Codepage change detection
 *   Full Long fil names support
 *
 *
 * 2011-10-01  Eduardo           * Fixed a bug when printing default error messages
 *                               * New errorlevels: If successful, returns drive number
 *                                 (starting with A == 1)
 * 2011-10-09  Eduardo           * Add a CPU test
 * 2011-10-15  Eduardo           * Configurable buffer size
 *                               * New verbosity options
 *             Tom Ehlert        * Replace fscanf_s() with a much lighter implementation
 * 2011-10-17  Eduardo           * Fixed a (new) bug when printing default error messages
 *                               * Uninstallation
 * 2011-11-02  Eduardo           * Partial long file names support (with mangling)
 * 2011-11-06  Eduardo           * New /QQ option
 *
 */ 
#include <process.h>
#include <dos.h>
#include <env.h>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>
#include <ctype.h>

#include "globals.h"
#include "kitten.h"
#include "messages.h"
#include "vmaux.h"
#include "dosdefs.h"
#include "redir.h"
#include "endtext.h"
#include "unicode.h"
#include "vmdos.h"
#include "vmshf.h"
#include "lfn.h"

PUBLIC nl_catd cat;
PUBLIC Verbosity verbosity = NORMAL;
static int uninstall = 0;

static struct SREGS segRegs;

// Far pointers to resident data
// These are set by GetFarPointersToResidentData()
//
static uint8_t	far * fpLfn;
static uint8_t	far * fpDriveNum;
static CDS 		far * far * fpfpCDS;
static SDA		far * far * fpfpSDA;
static SDB		far * far * fpfpSDB;
static FDB		far * far * fpfpFDB;
static char		far * far * fpfpFcbName1;
static char		far * far * fpfpFcbName2;
static char		far * far * fpfpFileName1;
static char		far * far * fpfpFileName2;
static char		far * far * fpfpCurrentPath;
static void		(__interrupt __far * far * fpfpPrevInt2fHandler)();
static void 	(__interrupt __far *fpNewInt2fHandler)();
static char		far * far * fpfpLongFileName1;
static char		far * far * fpfpLongFileName2;
static uint16_t far *fpUnicodeTbl;

static uint8_t	far * far *fpfpFUcase;
static FChar	far * far *fpfpFChar;
static int32_t	far *fpGmtOffset;
static uint8_t	far	*fpCaseSensitive;

static uint8_t	far *fpHashLen;
static uint8_t	far *fpManglingChars;

PUBLIC rpc_t	far *fpRpc;
static uint16_t	far *fpBufferSize;
static uint16_t	far *fpMaxDataSize;
static uint8_t	* far *fpBuffer;

static SysVars far *fpSysVars;
static CDS far *currDir;
static char far *rootPath = " :\\";

static Signature signature;
static char *myName = "VMSMOUNT";

// CPU identification routine
// (Info from http://www.ukcpu.net/Programming/Hardware/x86/CPUID/x86-ID.asp)
//
// in 808x and 8018x, flags 12-15 are always set.
// in 286, flags 12-15 are always clear in real mode.
// in 32-bit processors, bit 15 is always clear in real mode.
//                       bits 12-14 have the last value loaded into them.
//
static uint8_t RunningIn386OrHigher( void );
#pragma aux RunningIn386OrHigher =									\
	"pushf"				/* Save current flags */					\
	"xor ax, ax"													\
	"push ax"														\
	"popf"				/* Load all flags cleared */				\
	"pushf"															\
	"pop ax"			/* Load flags back to ax */					\
	"and ax, 0xf000"	/* If 86/186, flags 12-15 will be set */	\
	"cmp ax, 0xf000"												\
	"je return"														\
	"mov ax, 0xf000"												\
	"push ax"														\
	"popf"				/* Load flags 12-15 set */					\
	"pushf"															\
	"pop ax"			/* Load flags back to ax */					\
	"and ax, 0xf000"	/* If 286, flags 12-15 will be cleared */	\
	"jz return"														\
	"mov al, 0x01"													\
	"return:"														\
	"popf"				/* Restore flags */							\
	value [al];

//	00h not installed, OK to install
//	01h not installed, not OK to install 
//	FFh some redirector is installed
//
static uint8_t InstallationCheck( void );
#pragma aux InstallationCheck =							\
	"mov ax, 1100h"	/* Installation check */			\
	"int 2fh"											\
	value [al];

static void GetFarPointersToResidentData( void )
{
	// From redir.c
	fpNewInt2fHandler		= segRegs.cs:>Int2fRedirector;
	fpfpPrevInt2fHandler	= segRegs.cs:>&fpPrevInt2fHandler;
	fpLfn = segRegs.cs:>&lfn;
	fpDriveNum = segRegs.cs:>&driveNum;
	fpfpSDA = segRegs.cs:>&fpSDA;
	fpfpCDS = segRegs.cs:>&fpCDS;
	fpfpSDB = segRegs.cs:>&fpSDB;
	fpfpFDB = segRegs.cs:>&fpFDB;
	fpfpFcbName1	= segRegs.cs:>&fpFcbName1;
	fpfpFcbName2	= segRegs.cs:>&fpFcbName2;
	fpfpFileName1	= segRegs.cs:>&fpFileName1;
	fpfpFileName2	= segRegs.cs:>&fpFileName2;
	fpfpCurrentPath	= segRegs.cs:>&fpCurrentPath;
	
	// from lfn.c
	fpfpLongFileName1	= segRegs.cs:>&fpLongFileName1;
	fpfpLongFileName2	= segRegs.cs:>&fpLongFileName2;
	*fpfpLongFileName1	= segRegs.cs:>&longFileName1;
	*fpfpLongFileName2	= segRegs.cs:>&longFileName2;
	
	// From unicode.c
	fpUnicodeTbl = segRegs.cs:>&unicodeTbl;
	
	// From vmdos.c
	fpfpFUcase = segRegs.cs:>&fpFUcase;
	fpfpFChar = segRegs.cs:>&fpFChar;
	fpGmtOffset = segRegs.cs:>&gmtOffset;
	fpCaseSensitive = segRegs.cs:>&caseSensitive;
		
	// From lfnc.
	fpManglingChars = segRegs.cs:>&manglingChars;
	fpHashLen = segRegs.cs:>&hashLen;

	// From vmshf.c
	fpRpc = segRegs.cs:>&rpc;
	fpBufferSize = segRegs.cs:>&bufferSize;
	fpMaxDataSize = segRegs.cs:>&maxDataSize;
	fpBuffer = segRegs.cs:>&buffer;
	
	return;
	
}

static void PrintUsageAndExit(int err)
{
	VERB_FPRINTF( NORMAL, stderr, MSG_MY_NAME, VERSION_MAJOR, VERSION_MINOR );
	VERB_FPRINTF( NORMAL, stderr, MSG_COPYRIGHT );
	VERB_FPUTS( QUIET, catgets( cat, 0, 1, MSG_HELP_1 ), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0, 2, MSG_HELP_2 ), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0, 3, MSG_HELP_3 ), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0, 4, MSG_HELP_4 ), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0, 5, MSG_HELP_5 ), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0, 6, MSG_HELP_6 ), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0, 7, MSG_HELP_7 ), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0, 8, MSG_HELP_8 ), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0, 9, MSG_HELP_9 ), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0,10, MSG_HELP_10), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0,11, MSG_HELP_11), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0,12, MSG_HELP_12), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0,13, MSG_HELP_13), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0,14, MSG_HELP_14), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0,15, MSG_HELP_15), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0,16, MSG_HELP_16), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0,17, MSG_HELP_17), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0,18, MSG_HELP_18), stderr );
	VERB_FPUTS( QUIET, catgets( cat, 0,19, MSG_HELP_19), stderr );
	exit( err );
	
}

static int GetOptions(char *argString)
{
	int argIndex, i;
	uint32_t d, bufsiz= 0;
	uint8_t mangle = 0;
	char c, *s;
	int vset= 0;
	int bset= 0;
	int lset= 0;
	int lfnset= 0;
	int mset= 0;
	int cset= 0;
	
	for ( argIndex= 0; argString[argIndex] != '\0'; ++argIndex )
	{
		switch ( argString[argIndex] )
		{
			case ' ':
			case '\t':
				break;	// Skip spaces
			
			case '/':
			case '-':
				switch ( c= toupper( argString[++argIndex] ) )
				{
					case 'H':
					case '?':
						return 1;
						break;
					case 'Q':
						if ( vset++ )
						{
							return 2;	// /Q, /QQ or /V already set
						}
						if ( toupper( argString[argIndex+1] ) == 'Q' )
						{
							verbosity = SILENT;
							++argIndex;
						}
						else
						{
							verbosity = QUIET;
						}
						break;
					case 'V':
						if ( vset++ )
						{
							return 2;	// /Q or /V already set
						}
						verbosity = VERBOSE;
						break;
					case 'U':
						++uninstall;
						break;
					case 'B':
						if ( bset++ || argString[++argIndex] != ':' )
						{
							return 2; // Already been here or option needs a number
						}
						s = &argString[++argIndex];
						for ( i= 0; isdigit( s[i] ) ; ++i, ++argIndex );
						if ( i != 0 )
						{
							d = 1;
							while ( i-- )
							{
								bufsiz += ( s[i] - 0x30 ) * d;
								if ( bufsiz > UINT16_MAX )
								{
									bufsiz = UINT16_MAX;
									break; // bufsiz too large
								}
								d *= 10;
							}
							if ( toupper( argString[argIndex] ) == 'K' )
							{
								if ( bufsiz > ( UINT16_MAX >> 10 ) )
								{
									bufsiz = UINT16_MAX;
								}
								else
								{
									bufsiz <<= 10;
								}
							}
							else
							{
								--argIndex;
							}
						}
						else
						{
							return 2; // Not a number
						}
						*fpBufferSize = (uint16_t) bufsiz;
						break;
					case 'L':
						if ( toupper( argString[argIndex+1]) == 'F' && toupper( argString[argIndex+2]) == 'N' )
						{
							if ( lfnset++ )
							{
								return 2;	// LFN already set
							}
							*fpLfn = 1;
							argIndex += 2;
						}
						else
						{
							if ( lset++ || argString[++argIndex] != ':' )
							{
								return 2; // /L already set or needs a drive letter
							}
							if ( !isalpha( c= toupper( argString[++argIndex] ) ) )
							{
								return 2; // Missing or invalid drive letter
							}
							*fpDriveNum = c - 'A';
						}
						break;
					case 'M':
						if ( !lfnset || mset++ || argString[++argIndex] != ':' )
						{
							return 2; // Already been here or needs a number or LFN not set;
						}
						if ( ! isdigit( c = argString[++argIndex] ) )
						{
							return 2; // Argument must be a number
						}
						if ( isdigit( argString[argIndex+1] ) )
						{
							mangle = 10;	// Anything bigger than 8 would do
							while ( isdigit( argString[argIndex+1] ) )
							{
								++argIndex;
							}
						}
						else {
							mangle = c - 0x30;
						}
						*fpManglingChars = mangle;
						*fpHashLen = mangle << 2;	// Translate nibbles to bits
						break;
					case 'C':
						if ( !lfnset || cset++ )
						{
							return 2;	// // Already been here or LFN not set;
						}
						switch ( toupper( argString[++argIndex] ) )
						{
							case 'S':
								*fpCaseSensitive = TRUE;
								break;
							case 'I':
								*fpCaseSensitive = FALSE;
								break;
							default:
								return 2;
						}
						break;
					default:
						return 2; // Invalid option
				}
				break;
	
			default:
				return 2; // Non switch
		}
	}
	
	return 0;
}

static int GetNLS(void)
{
	union REGS r;
	struct SREGS s;
	static NLSTable nlsTable;

	segread( &s );
	
	// Get FUCASE (File Uppercase Table)
	//
	r.w.ax = 0x6504;
	r.x.bx	= r.x.dx = 0xffff;
	r.x.cx	= 5;

	s.es	= s.ds;
	r.x.di	= (uint16_t) &nlsTable;
		
	intdosx( &r, &r, &s );

	if ( r.x.cx != 5 )
		return 1;

	*fpfpFUcase = (uint8_t far *)nlsTable.pTableData + 2;	// Skip size word

	// Get FCHAR (File Terminator Table)
	//
	r.w.ax	= 0x6505;
	
	intdosx( &r, &r, &s );

	if ( r.x.cx != 5 )
		return 1;

	*fpfpFChar = (FChar far *)nlsTable.pTableData ;
	
	return 0;

}

static int GetSDA(void)
{
	union REGS r;
	struct SREGS s;
	
	r.w.ax = 0x5D06;
	segread( &s );
	s.ds = r.x.si = 0;
			
	intdosx( &r, &r, &s );
	
	if ( !s.ds && !r.x.si )
		return 1;
		
	*fpfpSDA = (SDA far *) MK_FP( s.ds, r.x.si );
	*fpfpSDB = &(*fpfpSDA)->findFirst;
	*fpfpFDB = &(*fpfpSDA)->foundEntry;

	return 0;

}

static int GetSysVars(void)
{
	union REGS r;
	struct SREGS s;

	r.h.ah = 0x52;
	segread( &s );
	s.es = r.x.bx = 0;
	
	intdosx(&r, &r, &s);
	
	if ( !s.es && !r.x.bx )
		return 1;

	fpSysVars = (SysVars far *) MK_FP( s.es, r.x.bx - SYSVARS_DECR );
	
	return 0;

}

static int AlreadyInstalled( void )
{
	int driveNum;
	
	for ( driveNum = 4; driveNum < fpSysVars->lastDrive; ++driveNum )
	{
		currDir = &fpSysVars->currDir[driveNum];
		if ( currDir->u.Net.parameter == VMSMOUNT_MAGIC )
		{
			break;
		}
	}
	
	// I'm going to be a bit paranoid here
	//
	if ( driveNum < fpSysVars->lastDrive )
	{
		if ( (uint32_t) currDir->u.Net.redirIFSRecordPtr != 0ui32
				&& (uint32_t) currDir->u.Net.redirIFSRecordPtr != 0xffffffffui32 )
		{
			if ( ! _fstrncmp( ((Signature far *) currDir->u.Net.redirIFSRecordPtr)->signature, segRegs.ds:>myName, 9 ) )
			{
				return 1;
			}
		}
	}

	return 0;
}

static uint16_t savedSS, savedSP;

// As we made the return address of the TSR PSP to point
// to TerminateTsr, execution of UninstallDriver() continues here
// 
#pragma aux TerminateTsr aborts;
void __declspec( naked ) TerminateTsr( void )
{
	_asm {
	
		// restore stack
		//
		mov		ax, seg savedSS
		mov		ds, ax
		mov		ss, savedSS
		mov		sp, savedSP

		// and registers
		//
		pop		di
		pop		es
		pop		ds
		
		// DOS 2+ internal - SET CURRENT PROCESS ID (SET PSP ADDRESS)
		// AH = 50h
		// BX = segment of PSP for new process
		//	
		mov		bx, _psp				// restore our PSP
		mov		ah, 0x50
		int		0x21
		
		// DOS 2+ - EXIT - TERMINATE WITH RETURN CODE
		// AH = 4Ch
		// AL = return code
		//
		mov		al, ERR_SUCCESS			// terminate successfully
		mov		ah, 0x4c
		int		0x21
	}

}

// This call is made after AlreadyInstalled(), so currDir points to the
// installed driver's CDS
// WARNING: fpfpPrevInt2fHandler needs to be set!!
//
// This function only returns on error. If successful, execution continues
// with TerminateTsr()
//
static void UninstallDriver( void )
{
	Signature far *sig = (Signature far *) currDir->u.Net.redirIFSRecordPtr;
	uint16_t psp = sig->psp;
		
	if ( sig->ourHandler != *fpfpPrevInt2fHandler )
	{
		// Can't uninstall, another handler was installed
		return;
	}
	
	_dos_setvect( 0x2f, sig->previousHandler );
	currDir->flags = currDir->flags & ~(NETWORK|PHYSICAL);			// Invalidate drive
	currDir->u.Net.parameter = 0x00;
	currDir->u.Net.redirIFSRecordPtr = (void far *)0xffffffffui32;

	VMAuxEndSession( sig->fpRpc );									// Close HGFS session
	
	VERB_FPUTS( QUIET, catgets( cat, 2, 1, MSG_INFO_UNINSTALL ), stderr );

	// Switch PSP and Int 4Ch method of unloading the TSR
	// (Undocumented DOS, Second Edition)
	//
	_asm {
		// save registers
		//
		push	ds
		push	es
		push	di

		// set TSR PSP return address to TerminateTsr
		//
		mov		es, psp
		mov		di, 0x0a		// Position of terminate address of previous program
		mov		ax, offset TerminateTsr
		stosw
		mov		ax, cs
		stosw

		// DOS 2+ internal - SET CURRENT PROCESS ID (SET PSP ADDRESS)
		// AH = 50h
		// BX = segment of PSP for new process
		//
		mov		bx, es			// set current PSP to TSR's
		mov		ah, 0x50
		int		0x21
		
		// save stack frame
		//
		mov		ax, seg savedSS
		mov		ds, ax
		mov		savedSS, ss
		mov		savedSP, sp
		
		// DOS 2+ - EXIT - TERMINATE WITH RETURN CODE
		// AH = 4Ch
		// AL = return code
		//
		mov		ax, 0x4c00		// successfully terminate TSR
		int		0x21
		
		/* execution continues in TerminateTsr() */
	}
	
}

static int SetCDS(void)
{
	int rootPathLen;
	
	if ( *fpDriveNum != 0xFF ) {
		currDir = &fpSysVars->currDir[*fpDriveNum];
		if ( currDir->flags & (NETWORK|PHYSICAL) ) {
			VERB_FPRINTF( QUIET, stderr, catgets( cat, 1, 0, MSG_ERROR_INUSE ), *fpDriveNum + 'A' );
			return 1;
		}
	}
	else {
		// Skip from A to D
		//
		for ( *fpDriveNum = 4;
				( (currDir = &fpSysVars->currDir[*fpDriveNum])->flags & (NETWORK|PHYSICAL) )
												&& *fpDriveNum < fpSysVars->lastDrive; ++*fpDriveNum );
		
		if ( *fpDriveNum == fpSysVars->lastDrive ) {
			VERB_FPRINTF( QUIET, stderr, catgets( cat, 1, 1, MSG_ERROR_NO_DRIVES ), fpSysVars->lastDrive + '@' );
			return 1;
		}
	}
	
	// Undocumented DOS 2nd Ed. p. 511
	//   Set Physical (0x4000), Network (0x8000) and REDIR_NOT_NET (0x80) bits
	//
	currDir->flags = NETWORK|PHYSICAL|REDIR_NOT_NET;
	
	// Undocumented DOS 2nd Ed. p. 514
	//   Set Network UserVal
	// My own: use redirIFSRecordPtr to point to the driver signature
	//
	currDir->u.Net.parameter = VMSMOUNT_MAGIC;
	currDir->u.Net.redirIFSRecordPtr = MK_FP( _psp, 0x81 );

	rootPathLen = _fstrlen( rootPath );
	rootPath[rootPathLen - 3] = *fpDriveNum + 'A';
	_fstrcpy( currDir->currentPath, rootPath );
	currDir->backslashOffset = rootPathLen - 1;

	*fpfpCDS = currDir;
	*fpfpCurrentPath = currDir->currentPath + currDir->backslashOffset;
	*fpfpFcbName1 = &(*fpfpSDA)->fcbName1;
	*fpfpFcbName2 = &(*fpfpSDA)->fcbName2;
	*fpfpFileName1 = &(*fpfpSDA)->fileName1[rootPathLen-1];	
	*fpfpFileName2 = &(*fpfpSDA)->fileName2[rootPathLen-1];	
	
	return 0;
}

static void LoadUnicodeConversionTable(void)
{
	union REGS r;
	char filename[13];
	char fullpath[_MAX_PATH];
	char buffer[256];
	struct stat filestat;
	FILE *f;
	int i, ret;
	
	// get current Code Page
	//
	//	AX = 6601h
	//	  Return: CF set on error
	//	    AX = error code (see #01680 at AH=59h/BX=0000h)
	//	  CF clear if successful
	//      BX = active code page (see #01757) <---
	//      DX = system code page (see #01757)
	//
	r.w.ax = 0x6601;
			
	intdos( &r, &r );

	if ( r.x.cflag ) {
		// Can't get codepage. Use ASCII only
		//
		VERB_FPUTS( QUIET, catgets( cat, 1, 19, MSG_WARN_CP ), stderr );
		goto error;
	}
	
	sprintf( filename, r.x.bx > 999 ? "c%duni.tbl" : "cp%duni.tbl", r.x.bx );
	
	VERB_FPRINTF( VERBOSE, stdout, catgets( cat, 9, 2, MSG_INFO_TBL ), r.x.bx, filename );
	
	_searchenv( filename, "PATH", fullpath);
	if ( '\0' == fullpath[0] )
	{
		VERB_FPRINTF( QUIET, stderr, catgets( cat, 1, 16, MSG_WARN_NOTBL ), filename );
		goto error;
	}
	
	f = fopen( fullpath, "rb" );
	
	if ( NULL == f )
	{
		VERB_FPRINTF( QUIET, stderr, catgets( cat, 1, 17, MSG_WARN_UNICODE ), filename );
		goto error;
	}
	
	// Tom: this fscanf_s implementation is VERY handy - but costs 5 K .exe size (3 K packed)
	// therefore I'm tempted to implement this manually
#if 0	
	if ( EOF == fscanf_s( f, "Unicode (%s)", buffer, sizeof( buffer) ) )
	{
		VERB_FPRINTF( QUIET, stderr, catgets( cat, 1, 18, MSG_WARN_TBLFORMAT ), filename );
		goto close;
	}
#else
	if ( fread( buffer, 1, 9, f ) != 9 ||     // "Unicode (
						memcmp( buffer, "Unicode (", 9 ) != 0 )
	{
		VERB_FPRINTF( QUIET, stderr, catgets( cat, 1, 18, MSG_WARN_TBLFORMAT ), filename );
		goto close;
	}   

	memset( buffer, 0, sizeof( buffer ) );
	                                          
	for ( i = 0; i < sizeof( buffer ); i++ )
	{
		if ( fread( buffer+i, 1, 1, f ) != 1 )
		{
			VERB_FPRINTF( QUIET, stderr, catgets( cat, 1, 18, MSG_WARN_TBLFORMAT ), filename );
			goto close;
		}
		if ( buffer[i] == ')' )  
		{
			buffer[i] = 0;
			break;	
		}
	}
#endif	

	ret = fread( buffer, 1, 3, f );

	if ( ret != 3 || buffer[0] != '\r' || buffer[1] != '\n' || buffer[2] != 1 )
	{
		VERB_FPRINTF( QUIET, stderr, catgets( cat, 1, 18, MSG_WARN_TBLFORMAT ), filename );
		goto close;
	}
	
	if ( 256 != (ret = fread( buffer, 1, 256, f )) )
	{
		VERB_FPRINTF( QUIET, stderr, catgets( cat, 1, 17, MSG_WARN_UNICODE ), filename );
		goto close;
	}
	
	_fmemcpy( fpUnicodeTbl, segRegs.ds:>buffer, 256 );
	
	return;

close:
	fclose( f );
error:
	VERB_FPUTS( QUIET, catgets( cat, 1, 20, MSG_WARN_437 ), stderr );
	
}

static void GetTimezoneOffset(void)
{
	char *tz;
	time_t utcTime, localTime;
	auto struct tm tmbuf;
	
	tz  = getenv( "TZ" );
	
	if ( NULL == tz )
	{
		VERB_FPUTS( QUIET, catgets( cat, 1, 15, MSG_WARN_TIMEZONE ), stderr );
	}
	else
	{
		// Compute offset between UTC and local times
		//
		localTime = time( NULL );
		_gmtime( &localTime, &tmbuf );
		tmbuf.tm_isdst = daylight;
		utcTime = mktime( &tmbuf );
	
		*fpGmtOffset = localTime - utcTime;
	
		VERB_FPRINTF( VERBOSE, stdout, catgets( cat, 9, 1, MSG_INFO_TZ ), *fpGmtOffset );
	}
	return;
}

static uint16_t SetBufferAndActualBufferSize( void )
{
	uint16_t transientSize;

	*fpBuffer = ( *fpLfn ) ? (uint8_t *) BeginOfTransientBlockWithLfn
							: (uint8_t *)  (uint8_t *) BeginOfTransientBlockNoLfn;
	
	transientSize = (uint16_t) EndOfTransientBlock
		- ( ( *fpLfn ) ? (uint16_t) BeginOfTransientBlockWithLfn : (uint16_t) BeginOfTransientBlockNoLfn );
	
	if (*fpBufferSize > transientSize || *fpBufferSize > VMSHF_MAX_BLOCK_SIZE
													|| *fpBufferSize < VMSHF_MIN_BLOCK_SIZE )
	{
		return ( ( transientSize > VMSHF_MAX_BLOCK_SIZE ) ? VMSHF_MAX_BLOCK_SIZE : transientSize );
	}

	*fpMaxDataSize = VMSHF_MAX_DATA_SIZE( *fpBufferSize );
	
	return 0;
}

static uint16_t SizeOfResidentSegmentInParagraphs( void )
{
	uint16_t	sizeInBytes;
	
	sizeInBytes	=
		( ( *fpLfn )? (uint16_t) BeginOfTransientBlockWithLfn : (uint16_t) BeginOfTransientBlockNoLfn )
		+ *fpBufferSize;
	sizeInBytes	+= 0x100;	// Size of PSP (do not add when building .COM executable)
	sizeInBytes	+= 15;		// Make sure nothing gets lost in partial paragraph

	return sizeInBytes >> 4;
}

static void SetSignature( void )
{
	strcpy( signature.signature, myName );
	signature.psp = _psp;
	signature.ourHandler = fpNewInt2fHandler;
	signature.previousHandler = *fpfpPrevInt2fHandler;
	signature.fpRpc = fpRpc;
	_fmemcpy( MK_FP( _psp, 0x81 ), segRegs.ds:>&signature, sizeof( signature ) );
	
	return;
}

int main(int argc, char **argv)
{

	char		argString[128];
	int			tblSize;
	uint16_t	ret;
	uint16_t	paragraphs;
	
	segread( &segRegs );
	
	cat = catopen( myName, 0 );
		
	GetFarPointersToResidentData();
	
	ret = GetOptions( getcmd( argString ) );
	
	if ( ret == 1 )		// User requested help
	{
		verbosity = NORMAL;					// Ignore verbosity level
		PrintUsageAndExit( ERR_SUCCESS );
	}
	if ( ret == 2 )		// Invalid option
	{
		PrintUsageAndExit( ERR_BADOPTS );
	}

	VERB_FPRINTF( QUIET, stderr, MSG_MY_NAME, VERSION_MAJOR, VERSION_MINOR );

	VERB_FPRINTF( NORMAL, stdout, MSG_COPYRIGHT );
	
	// Check that CPU is x386 or higher (to avoid nasty things if somebody
	// tries to run this in a REAL machine
	//
	if ( ! RunningIn386OrHigher() )
	{
		VERB_FPUTS( QUIET, catgets(cat, 1, 3, MSG_ERROR_NOVIRT ), stderr );
		return( ERR_NOVIRT );
	}

	// Check OS version. Only DOS >= 5.0 is supported
	//
	if ( _osmajor < 5 )
	{
		VERB_FPRINTF( QUIET, stderr, catgets( cat, 1, 2, MSG_ERROR_BADOS ), _osmajor, _osminor );
		return( ERR_WRONGOS );
	}

	if ( *fpLfn 
		&& (*fpManglingChars < MIN_MANGLING_CHARS || *fpManglingChars > MAX_MANGLING_CHARS ) )
	{
		VERB_FPRINTF( QUIET, stderr, catgets(cat, 1, 14, MSG_ERROR_MANGLE ), MIN_MANGLING_CHARS, MAX_MANGLING_CHARS );
		return( ERR_BADOPTS );
	}
	
	if ( VMAuxCheckVirtual() )
	{
		VERB_FPUTS( QUIET, catgets(cat, 1, 3, MSG_ERROR_NOVIRT ), stderr );
		return( ERR_NOVIRT );
	}
	
	if ( 0x01 == InstallationCheck() )
	{
		VERB_FPUTS( QUIET, catgets(cat, 1, 6, MSG_ERROR_REDIR_NOT_ALLOWED ), stderr );
		return( ERR_NOALLWD );
	}
	
	if ( GetSysVars() )
	{
		VERB_FPUTS( QUIET, catgets( cat, 1, 5, MSG_ERROR_LOL ), stderr );
		return( ERR_SYSTEM );
	}
	
	// Needed by UninstallDriver()
	//
	*fpfpPrevInt2fHandler = _dos_getvect( 0x2F );
	
	if ( AlreadyInstalled() )
	{
		if ( uninstall )
		{
			UninstallDriver();
			
			/* UninstallDriver() only returns on error */
			
			VERB_FPUTS( QUIET, catgets( cat, 1, 12, MSG_ERROR_UNINSTALL ), stderr );
			return( ERR_UNINST );
		}
		else
		{
			VERB_FPUTS( QUIET, catgets( cat, 1, 7, MSG_ERROR_INSTALLED ), stderr );
			return( ERR_INSTLLD );
		}
	}
	else
	{
		if ( uninstall )
		{
			VERB_FPUTS( QUIET, catgets( cat, 1, 13, MSG_ERROR_NOTINSTALLED ), stderr );
			return( ERR_NOTINST );
		}
	}
	
	if ( GetSDA() )
	{
		VERB_FPUTS( QUIET, catgets( cat, 1, 9, MSG_ERROR_SDA ), stderr );
		return( ERR_SYSTEM );
	}
	
	if ( GetNLS() )
	{
		VERB_FPUTS( QUIET, catgets( cat, 1, 10, MSG_ERROR_NLSINFO ), stderr );
		return( ERR_SYSTEM );
	}
	
	LoadUnicodeConversionTable();
	GetTimezoneOffset();

	ret = SetBufferAndActualBufferSize();

	if ( ret )
	{
		VERB_FPRINTF( QUIET, stderr, catgets( cat, 1, 11, MSG_ERROR_BUFFER ), VMSHF_MIN_BLOCK_SIZE, ret );
		return( ERR_BUFFER );
	}
			
	if ( *fpDriveNum != 0xFF && !( *fpDriveNum < fpSysVars->lastDrive ) )
	{
		VERB_FPRINTF( QUIET, stderr, catgets( cat, 1, 4, MSG_ERROR_INVALID_DRIVE ),
							*fpDriveNum + 'A', fpSysVars->lastDrive + '@');
		return( ERR_BADDRV );
	}
	
	if ( VMAuxBeginSession( fpRpc ) )
	{
		VERB_FPUTS( QUIET, catgets( cat, 1, 8, MSG_ERROR_NOSHF ), stderr );
		return( ERR_NOSHF );
	}
		
	if ( SetCDS() )
	{
		ret = ERR_SYSTEM;
		goto err_close;
	}

	SetSignature();
	
	_dos_setvect( 0x2f, fpNewInt2fHandler );	

	paragraphs = SizeOfResidentSegmentInParagraphs();
	VERB_FPRINTF( VERBOSE, stdout, catgets( cat, 9, 3, MSG_INFO_LOAD ), paragraphs << 4 );

	VERB_FPRINTF( QUIET, stderr, catgets( cat, 2, 0, MSG_INFO_MOUNT ), *fpDriveNum + 'A' );
	
	catclose( cat );
	flushall();		// Flush all streams before returning to DOS
	_dos_keep( *fpDriveNum + 1, paragraphs );

err_close:
	VMAuxEndSession( fpRpc );
	return ret;
}
