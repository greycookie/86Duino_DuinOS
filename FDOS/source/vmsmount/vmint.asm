; Basic 64bit integer arithmetic
;
; Copyright (c) 2002-2008, Ken Kato
;   http://sites.google.com/site/chitchatvmback/
;
;   Ported to NASM by Eduardo Casino
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
; MA  02110-1301, USA.
;

CPU 386
BITS 16

SEGMENT BEGTEXT CLASS=CODE PUBLIC

JUMP_ALIGN EQU 2

GLOBAL _u64div32
GLOBAL _u32div32
GLOBAL _u64add32
GLOBAL _u64sub32

; uint32_t __cdecl u64div32( uint64_t *quotient, uint64_t dividend, uint32_t divisor )
;   *quotient = dividend / divisor, returns remainder
;
ALIGN JUMP_ALIGN
_u64div32:
	push ebx
	push ecx
	mov bx, word [esp+0x0a]		; Pointer to quotient
	mov eax, dword [esp+0x10]	; High dword of dividend
	xor edx, edx
	mov ecx, dword [esp+0x14]	; divisor
	div ecx
	mov dword [bx+4], eax		; High dword of quotient
	mov eax, dword [esp+0x0c]	; Low dword of dividend
	div ecx
	mov dword [bx], eax			; Low dword of quotient
	mov ax, dx					; Return remainder in dx:ax
	ror edx, 16
	pop ecx
	pop ebx
	ret

; uint32_t __cdecl u32div32( uint32_t *quotient, uint32_t dividend, uint32_t divisor )
;   *quotient = dividend / divisor, returns remainder
;
ALIGN JUMP_ALIGN
_u32div32:
	push ebx
	push ecx
	mov bx, word [esp+0x0a]		; Pointer to quotient
	mov eax, dword [esp+0x0c]	; Dividend
	xor edx, edx
	mov ecx, dword [esp+0x10]	; Divisor
	div ecx
	mov dword [bx], eax			; Quotient
	mov ax, dx					; Return remainder in dx:ax
	ror edx, 16
	pop ecx
	pop ebx
	ret

; void __cdecl u64add32( uint64_t *result, uint64_t value1, uint32_t value2 )
;   *result = value1 + value2
;
ALIGN JUMP_ALIGN
_u64add32:
	push ebx
	mov bx, word [esp+0x06]
	mov eax, dword [esp+0x08]
	add eax, dword [esp+0x10]
	mov dword [bx], eax
	mov eax, dword [esp+0x0c]
	adc eax, 0
	mov dword [bx+4], eax
	pop ebx
	ret

; void __cdecl u64sub32( uint64_t *result, uint64_t value1, uint32_t value2 )
;   *result = value1 - value2
;
ALIGN JUMP_ALIGN
_u64sub32:
	push ebx
	mov bx, word [esp+0x06]
	mov eax, dword [esp+0x08]
	sub eax, dword [esp+0x10]
	mov dword [bx], eax
	mov eax, dword [esp+0x0c]
	sbb eax, 0
	mov dword [bx+4], eax
	pop ebx
	ret
