#ifndef _UNICODE_H
#define _UNICODE_H
#pragma once
/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *  Copyright (C) 2011  Eduardo Casino
 *
 * unicode.h: Unicode conversion routines
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
 * 2011-11-01  Eduardo           * Buffer overflow control for LocalToUtf8
 *
 */
 
#include <stdint.h>

extern uint16_t unicodeTbl[128];

extern int LocalToUtf8( uint8_t far *dst, uint8_t far *src, int buflen );
extern int Utf8ToLocal( uint8_t *dst, uint8_t *src );

#endif

