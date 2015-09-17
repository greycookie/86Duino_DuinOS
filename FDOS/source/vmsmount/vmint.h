#ifndef VMINT_H_
#define VMINT_H_
#pragma once
/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *  Copyright (C) 2011  Eduardo Casino
 *
 * vmint.h: Basic 64bit integer arithmetic
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
 */

#include <stdint.h>

uint32_t __cdecl u64div32( uint64_t *quotient, uint64_t dividend, uint32_t divisor );
uint32_t __cdecl u32div32( uint32_t *quotient, uint32_t dividend, uint32_t divisor );
void __cdecl u64add32( uint64_t *result, uint64_t value1, uint32_t value2 );
void __cdecl u64sub32( uint64_t *result, uint64_t value1, uint32_t value2 );

#endif /* VMINT_H_ */
