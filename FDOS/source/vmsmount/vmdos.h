#ifndef VMDOS_H_
#define VMDOS_H_
#pragma once
/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *  Copyright (C) 2011  Eduardo Casino
 *
 * vmdos.h: Conversions from/to vmware and DOS
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
 * 2011-10-04  Eduardo           * New parameter for FNameToFcbName()
 *                                 (Omit '.' and '..' if root dir)
 * 2011-11-01  Eduardo           * Add LFN support
 *
 */
 
#include <stdint.h>
#include "dosdefs.h"
#include "vmshf.h"

extern int32_t gmtOffset;
extern uint8_t far *fpFUcase;
extern FChar far *fpFChar;
extern uint8_t caseSensitive;

extern unsigned char toupper_local ( unsigned char c );
extern int IllegalChar( unsigned char c );
extern uint32_t FTimeToFatTime( uint64_t );
extern uint8_t FModeToFatAttr( VMShfAttr * );
extern VMShfAttr *FatAttrToFMode( uint8_t );
extern uint32_t DosExtActionToOpenAction( uint16_t );
extern int FNameToFcbName( char *fcbName, char *fName, uint16_t fNameLen, uint8_t isRoot, uint8_t lfn );
extern int DosPathToPortable(uint8_t *dst, uint8_t far *src, uint8_t utf );
extern uint16_t VmshfStatusToDosError( uint32_t );

#endif /* VMDOS_H_ */
