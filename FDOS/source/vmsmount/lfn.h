#ifndef LFN_H_
#define LFN_H_
#pragma once
/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *  Copyright (C) 2011  Eduardo Casino
 *
 * lfn.h: functions for Long File Name support
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
 */
 
#include <stdint.h>
#include "vmshf.h"

#define DEF_MANGLING_CHARS	3
#define MIN_MANGLING_CHARS	2
#define MAX_MANGLING_CHARS	6

extern uint8_t manglingChars;
extern uint8_t hashLen;

extern char longFileName1[VMSHF_MAX_PATH]; 
extern char longFileName2[VMSHF_MAX_PATH]; 

// Claculates and returns hashLen bits length hash
//
extern uint32_t LfnFNameHash(
	uint8_t *name, 		// in : File name in UTF-8
	uint16_t len		// in : File name length
	);

// Gets the true, LFN path from a DOS, mangled path
//  Returns far pointer to converted path, NULL if buffer overflow
//
extern char far *LfnGetTrueLongName(
	char far *dest,		// out : Destination buffer for LFN
	char far *path		// in : DOS path name
	);
	
// Generates a valid (and hopefully unique) 8.3 DOS path from an LFN
// This function assumes that fName is already in DOS character set
//  Returns always TRUE
//
extern int LfnMangleFNameToFcbName( 
	uint32_t hash,		// in : Pre-calculated hash of the filename in UTF-8
	char *fcbName,		// out : File name in FCB format. Must be 12 bytes long (we NULL-terminate it)
	char *fName,		// in : File name in DOS codepage
	uint16_t fNameLen	// in : File name length
	);
	
// Locates begin of transient part of the code when /LFN is set
// Returns offset to itself
//
extern uint16_t BeginOfTransientBlockWithLfn( void );

#endif /* LFN_H_ */
