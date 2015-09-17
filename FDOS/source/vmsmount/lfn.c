/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *  Copyright (C) 2011  Eduardo Casino
 *
 * lfn.c: Long File Name support
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
 * 2011-11-06  Eduardo           * Add support for lower case shares
 *
 */

#include <stdint.h>
#include "lfn.h"
#include "globals.h"
#include "miniclib.h"
#include "unicode.h"
#include "vmdos.h"
#include "vmshf.h"
#include "redir.h"		// For myDS

#pragma data_seg("BEGTEXT", "CODE");
#pragma code_seg("BEGTEXT", "CODE");

PUBLIC uint8_t manglingChars = DEF_MANGLING_CHARS;
PUBLIC uint8_t hashLen = DEF_MANGLING_CHARS << 2;

PUBLIC char longFileName1[VMSHF_MAX_PATH]; 
PUBLIC char longFileName2[VMSHF_MAX_PATH]; 

inline char hexDigit( uint8_t val )
{
	return val > 9 ? val + 55 : val + 48;
}

/*
 * NOTE: The hash algorithm is from sdbm, a public domain clone of ndbm
 *       by Ozan Yigit (http://www.cse.yorku.ca/~oz/), with the result
 *       folded to the desired hash lengh (method from
 *		 http://isthe.com/chongo/tech/comp/fnv). Hence, LfnFNameHash() is
 *       also in the PUBLIC DOMAIN. (The actual code is fom the version
 *       included in gawk, which uses bit rotations instead of
 *       multiplications)
 */
// Claculates and returns hashLen bits length hash
//
#define FOLD_MASK(x) (((uint32_t)1<<(x))-1)
PUBLIC uint32_t LfnFNameHash(
	uint8_t *name, 		// in : File name in UTF-8
	uint16_t len		// in : File name length
	)
{
	uint8_t *be = name + len;			/* beyond end of buffer */
	uint32_t hval = 0;

	while ( name < be )
	{
	/* It seems that calculating the hash backwards gives an slightly
	 * better dispersion for files with names which are the same in their
	 * first part ( like image_000001.jpg, image_000002.jpg, ... )
	 */
#	ifdef REVERSE_HASH
		hval = *--be + (hval << 6) + (hval << 16) - hval;
#	else
		hval = *name++ + (hval << 6) + (hval << 16) - hval;
#	endif

	}

	/* Fold result to the desired len */
	if ( hashLen < 16 )
	{
		hval = ( ( (hval>>hashLen) ^ hval ) & FOLD_MASK( hashLen ) );
	}
	else
	{
		hval = ( hval>>hashLen ) ^ ( hval & FOLD_MASK( hashLen ) );
	}
	
    /* return our new hash value */
    return hval;
}

// Compares a DOS 8.3 name to a DOS FCB name
//  Returns TRUE if match, FALSE otherwise
//
inline int MatchFcbNameToFileName(
	char *fcbName,			// in : DOS FCB name
	char far *fileName		// in : DOS 8.3 name
	)
{
	int i = 0;
	
	while ( *fileName && i < 12 )
	{
		if ( *fileName == '.' )
		{
			if ( i != 8 && fcbName[i] != ' ' )
			{
				return 0;	// No match
			}
			i = 8;
			++fileName;
			continue;
		}
		if ( *(fileName++) != fcbName[i++] )
		{
			return FALSE;
		}
	}
	return TRUE;

}

// Finds the true, long file name from a mangled 8.3 name
//  Returns far pointer to end of converted path, NULL if buffer overflow
//
inline char far *FindRealName(
	char far *dest,			// out : destination buffer
	char far *path,			// in : search path
	char far *fileName,		// in : 8.3 file name
	int namelen,			// in : file name length
	int bufsiz				// in : buffer size
	)
{
	static char fcbName[12];
	
	uint32_t status = VMSHF_SUCCESS;
	uint32_t handle;
	int ret, i = -1;
	VMShfAttr *fAttr;
	uint32_t fNameLen;
	char *fName;
	char far *d;
	uint8_t isRoot = ( *(path+1) == '\0' );

	ret = VMShfOpenDir( path, 1, &status, &handle );

	if ( ret != VMTOOL_SUCCESS || status != VMSHF_SUCCESS )
	{
		goto notfound;
	}
	
	for ( ;; )
	{
		ret = VMShfReadDir( handle, ++i, &status, &fAttr, &fName, &fNameLen );
		
		if ( ret != VMTOOL_SUCCESS || status != VMSHF_SUCCESS || 0 == fName )
		{
			(void) VMShfCloseDir( handle, &status );
			break;
		}
		
		// Copy now, because FNameToFcbName() converts fName to DOS codepage
		//
		if ( fNameLen > (uint32_t) bufsiz )
		{
			(void) VMShfCloseDir( handle, &status );
			return (char far *)0;
		}
		d = _fstrcpy_local( dest, MK_FP( myDS, fName ) );
		
		if ( FNameToFcbName( fcbName, fName, (uint16_t)fNameLen, isRoot, 1 )
				&& MatchFcbNameToFileName( fcbName, fileName ) )
		{
			(void) VMShfCloseDir( handle, &status );
			return d;
		}
	}
	
notfound:
	if ( namelen > bufsiz )
	{
		return (char far *)0;
	}
	else
	{
		return _fstrcpy_local( dest, fileName );
	}
}

// Gets the true, LFN path from a DOS, mangled path
//  Returns far pointer to converted path, NULL if buffer overflow
//
PUBLIC char far *LfnGetTrueLongName(
	char far *dest,		// out : destination buffer
	char far *path		// in : DOS path
	)
{
	char far *s, far *d;
	char far *tl, far *ps, far *ns, savens;
	int ret, len = 0;

	s = path;
	d = dest;
	
	tl = _fstrchr_local( path, '~' );
	
	while ( *tl != '\0' )
	{
		//            tl
		//            |
		//  ps ---+   |    +--- ns
		//        |   |    |
		//   /path/fil~enam/
		//        0123456789
	
		*tl = '\0';
		ps = _fstrrchr_local( s, '\\' );
		*tl = '~';
		
		ns = _fstrchr_local( tl, '\\' );
		
		savens = *ns;				// Can be '\\' or '\0'
		*ps = *ns = '\0';
	
		ret = LocalToUtf8( d, s, VMSHF_MAX_PATH - len );
		
		if ( ret < 0 )
		{
			return (char far *)0;
		}
		
		d += ret;
		len += ret;
		
		if ( VMSHF_MAX_PATH <= len )
		{
			return (char far *)0;
		}

		*d++ = '\\';
		*d= '\0';
		++len;
		
		d = FindRealName( d, dest, ps+1, (int)(ns-ps+1), VMSHF_MAX_PATH - len );
		
		if ( d == (char far *)0 )
		{
			return (char far *)0;
		}

		len = (int) ( d - dest );
		
		*d= '\0';

		*ps = '\\';
		*ns = savens;
		
		tl = _fstrchr_local( ns, '~' );
		s = ns;
	}
	
	ret = LocalToUtf8( d, s, VMSHF_MAX_PATH - len );
	
	if ( ret < 0 )
	{
		return (char far *) 0;
	}
	else
	{
		return dest;
	}
}
	
// Generates a valid (and hopefully unique) 8.3 DOS path from an LFN
// This function assumes that fName is already in DOS character set
//  Returns always TRUE
//
PUBLIC int LfnMangleFNameToFcbName( 
	uint32_t hash,		// in : Pre-calculated hash of the filename in UTF-8
	char *fcbName,		// out : File name in FCB format. Must be 12 bytes long (we NULL-terminate it)
	char *fName,		// in : File name in DOS codepage
	uint16_t fNameLen	// in : File name length
	)
{
	char *d, *p, *s;
	int i;
	uint8_t mangle = manglingChars;
	static int ror[] = { 0, 4, 8, 12, 16, 20 };

	*(uint32_t *)(&fcbName[0]) = 0x20202020;
	*(uint32_t *)(&fcbName[4]) = 0x20202020;
	*(uint32_t *)(&fcbName[8]) = 0X00202020;

	// First, skip leading dots and spaces
	//
	while ( *fName == ' ' || *fName == '.' )
	{
		++fName;
	}
	
	// Then look for the last dot in the filename
	//
	d = p = strrchr_local( fName, '.' );
	
	// There is an extension
	//
	if ( p != fName )
	{
		*p = '\0';
		i = 0;
		s = &fcbName[8];
		while ( *++p != '\0' && i < 3 )
		{
			if ( *p != ' ' )
			{
				*s++ = IllegalChar( *p ) ? '_' : toupper_local( *p );
				++i;
			}
		}
	}
	
	i = 0;
	s = fcbName;
	p = fName;
	while ( *p && i < 8)
	{
		if ( *p != ' ' && *p != '.' )
		{
				*s++ = IllegalChar( *p ) ? '_' : toupper_local( *p );
				++i;
		}
		++p;
	}

	if ( *d == '\0' )
	{
		*d = '.';
	}

	// It is 7 because 1 char is for the tilde '~'
	//
	if ( i > 7 - mangle )
	{
		i = 7 - mangle;
	}

	fcbName[i] = '~';
	
	while ( mangle-- )
	{
		fcbName[++i] = hexDigit( (hash >> ror[mangle]) & 0xf );
	}

	// Always TRUE
	//
	return TRUE;
}

/**
 * This function must the the last one in the BEGTEXT segment!
 *
 * BeginOfTransientBlockWithLfn() must locate after all other resident functions and global variables.
 * This function must be defined in the last source file if there are more than
 * one source file containing resident functions.
 */
PUBLIC uint16_t BeginOfTransientBlockWithLfn( void )
{
	return (uint16_t) BeginOfTransientBlockWithLfn;	// Force some code
}
