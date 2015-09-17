#ifndef VMCALL_H_
#define VMCALL_H_
#pragma once
/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *
 * This file is a derivative work of the VMware Command Line Tools, by Ken Kato
 *        http://sites.google.com/site/chitchatvmback/
 *
 * Copyright (c) 2002-2008,	Ken Kato
 * Copyright (C) 2011  Eduardo Casino
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
 * 2011-10-11  Eduardo           * Inline assembly for RPC backdoor
 *
 */
 
#include <stdint.h>

#pragma pack(1)
typedef struct Regs {
	union {
		uint8_t byte[4];
		struct {
			uint16_t low;
			uint16_t high;
		} halfs;
		uint32_t word;
	} eax;
	union {
		uint8_t byte[4];
		struct {
			uint16_t low;
			uint16_t high;
		} halfs;
		uint32_t word;
	} ebx;
	union {
		uint8_t byte[4];
		struct {
			uint16_t low;
			uint16_t high;
		} halfs;
		uint32_t word;
	} ecx;
	union {
		uint8_t byte[4];
		struct {
			uint16_t low;
			uint16_t high;
		} halfs;
		uint32_t word;
	} edx;
	union {
		uint8_t byte[4];
		struct {
			uint16_t low;
			uint16_t high;
		} halfs;
		uint32_t word;
	} ebp;
	union {
		uint8_t byte[4];
		struct {
			uint16_t low;
			uint16_t high;
		} halfs;
		uint32_t word;
	} edi;
	union {
		uint8_t byte[4];
		struct {
			uint16_t low;
			uint16_t high;
		} halfs;
		uint32_t word;
	} esi;
} CREGS;

#define LOAD_REGS \
	"movzx eax,ax" \
	"pushad" \
	"push eax" \
	"mov esi, 18h[eax]" \
	"mov edi, 14h[eax]" \
	"mov ebp, 10h[eax]" \
	"mov edx, 0ch[eax]" \
	"mov ecx, 08h[eax]" \
	"mov ebx, 04h[eax]" \
	"mov eax, 00h[eax]"

#define STORE_REGS \
	"xchg 00h[esp], eax" \
	"mov 18h[eax], esi" \
	"mov 14h[eax], edi" \
	"mov 10h[eax], ebp" \
	"mov 0ch[eax], edx" \
	"mov 08h[eax], ecx" \
	"mov 04h[eax], ebx" \
	"pop dword ptr 00h[eax]" \
	"popad"
	
static void _VmwCommand( CREGS *r );
#pragma aux _VmwCommand = \
	LOAD_REGS \
	"in eax, dx" \
	STORE_REGS \
	parm [ax]	\
	modify [ax];

static void _VmwRpcOuts( CREGS *r );
#pragma aux _VmwRpcOuts = \
	LOAD_REGS \
	"pushf" \
	"cld" \
	"rep outsb" \
	"popf" \
	STORE_REGS \
	parm [ax]	\
	modify [ax];

static void _VmwRpcIns( CREGS *r );
#pragma aux _VmwRpcIns = \
	LOAD_REGS \
	"push es" \
	"push ds" \
	"pop es" \
	"pushf" \
	"cld" \
	"rep insb" \
	"popf" \
	"pop es" \
	STORE_REGS \
	parm [ax]	\
	modify [ax];
	

extern void VmwCommand(CREGS *);
extern void VmwRpcIns(CREGS *);
extern void VmwRpcOuts(CREGS *);

#endif	/* VMCALL_H_ */

