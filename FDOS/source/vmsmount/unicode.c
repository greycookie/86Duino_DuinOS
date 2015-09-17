/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *  Copyright (C) 2011  Eduardo Casino
 *
 * unicode.c: unicode conversion routines
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
 *                               * Identify unsupported chars in Utf8ToLocal()
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"

#pragma data_seg("BEGTEXT", "CODE");
#pragma code_seg("BEGTEXT", "CODE");

// Initialised to cp437
//
PUBLIC uint16_t unicodeTbl[128] = {
	0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
	0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
	0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
	0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,
	0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
	0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
	0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,	
	0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
	0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
	0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x03BC, 0x03C4,
	0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,
	0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248,
	0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0
};

static uint8_t lookupCP( uint16_t cp )
{
	uint8_t i;
	
	for ( i = 0; i < 128 && unicodeTbl[i] != cp; ++i );
	
	return ( i < 128 ? (uint8_t) i + 128 : '\0' );

}

// dst and src CAN'T BE THE SAME !!!!
// Returns resulting length or -1 if buffer overflow
//
PUBLIC int LocalToUtf8( uint8_t far *dst, uint8_t far *src, int buflen )
{
	int len = 0;	// Resulting length
	uint16_t cp;	// Unicode Code Point
	
	while ( *src )
	{
		// UTF-8 bytes: 0xxxxxxx
		// Binary CP:   0xxxxxxx
		// CP range:    U+0000 to U+007F (Direct ASCII translation)
		//
		if ( ! (*src & 0x80) )
		{	
			if ( buflen > len )
			{
				*dst++ = *src;
				++len;
				goto cont;
			}
			else
			{
				return -1;
			}
		} 

		cp = unicodeTbl[*src - 128];
		
		// UTF-8 bytes: 110yyyyy 10xxxxxx
		// Binary CP:   00000yyy yyxxxxxx
		// CP range:    U+0080 to U+07FF
		//		
		if ( ! (cp & 0xF000) )
		{
			if ( buflen > len + 1 )
			{
				*dst++ = (uint8_t)( cp >> 6 ) | 0xC0;
				*dst++ = (uint8_t)( cp & 0x3f ) | 0x80;
				len += 2;
			}
			else
			{
				return -1;
			}
		}
				
		// UTF-8 bytes: 1110zzzz 10yyyyyy 10xxxxxx
		// Binary CP:   zzzzyyyy yyxxxxxx
		// CP range:    U+0800 to U+FFFF
		//
		else
		{
			if ( buflen > len +2 )
			{
				*dst++ = (uint8_t)( cp >> 12 ) | 0xE0;
				*dst++ = (uint8_t)( (cp >> 6) & 0x3F ) | 0x80;
				*dst++ = (uint8_t)( cp & 0x3F ) | 0x80;
				len += 3;
			}
			else
			{
				return -1;
			}
		}
cont:
		++src;
	};

	// Terminate string
	//
	*dst = '\0';
	
	return len;
	
}

// Returns 0 on success, -1 if any unsupported char is found
//
PUBLIC int Utf8ToLocal(uint8_t *dst, uint8_t *src)
{
	int ret = 0;	// Return code
	uint16_t cp;	// Unicode Code point
	
	while ( *src )
	{
		// UTF-8 bytes: 0xxxxxxx
		// Binary CP:   0xxxxxxx
		// CP range:    U+0000 to U+007F (Direct ASCII translation)
		//
		if ( ! (*src & 0x80) )
		{	
			*dst = *src;
			++src;
			goto cont;
		} 
		
		// UTF-8 bytes: 110yyyyy 10xxxxxx
		// Binary CP:   00000yyy yyxxxxxx
		// CP range:    U+0080 to U+07FF
		//
		if ( ! (*src & 0x20) ) 
		{
			cp = ( (uint16_t)(*src & 0x1F) << 6 ) | *(src+1) & 0x3F;
			*dst = lookupCP( cp );
			if ( *dst == '\0' )
			{
				*dst = '_';
				ret = -1;
			}
			src += 2;
			goto cont;
		}
		
		// UTF-8 bytes: 1110zzzz 10yyyyyy 10xxxxxx
		// Binary CP:   zzzzyyyy yyxxxxxx
		// CP range:    U+0800 to U+FFFF
		//
		if ( ! (*src & 0x10) )
		{
			cp = ( (uint16_t)(*src & 0xF) << 12 ) | ( (uint16_t)(*(src+1) & 0x3F) << 6 ) | *(src+2) & 0x3F;
			*dst = lookupCP( cp );
			if ( *dst == '\0' )
			{
				*dst = '_';
				ret = -1;
			}
			src += 3;
			goto cont;
		}
		
		// UTF-8 bytes: 11110www 10zzzzzz 10yyyyyy 10xxxxxx
		// Binary CP:   000wwwzz zzzzyyyy yyxxxxxx
		// CP range:    U+010000 to U+10FFFF
		//
		if ( ! (*src & 0x08) )
		{
			*dst = '_';		// Currently unsupported
			ret = -1;
			src += 4;
			goto cont;
		}
		
		// Should not reach here
		//
		*dst = '_';
		ret = -1;
		++src;
cont:
		++dst;
		
	};
	
	// Terminate string
	//
	*dst = '\0';

	return ret;

}
