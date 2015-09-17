/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *
 * This file is a derivative work of the VMware Command Line Tools, by Ken Kato
 *        http://sites.google.com/site/chitchatvmback/
 *
 * vmtool.c: VMware backdoor call wrapper functions
 *
 * Copyright (c) 2002-2008,	Ken Kato
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
 * 2011-10-11  Eduardo           * Remove unused VMRpcClose()
 *
 */
 
#include <stdint.h>
#include "vmcall.h"
#include "vmtool.h"
#include "globals.h"

#pragma data_seg("BEGTEXT", "CODE");
#pragma code_seg("BEGTEXT", "CODE");

static CREGS r;

/*
	send RPC command
*/
PUBLIC int VMRpcSend(const rpc_t *rpc, const unsigned char *command, uint32_t length)
{
	/* send command length */
	
	r.eax.word = VMWARE_MAGIC;
	r.ebx.word = length;
	r.ecx.word = VMCMD_INVOKE_RPC | VMRPC_SENDLEN;
	r.edx.word = rpc->channel | VMWARE_CMD_PORT;
	r.ebp.word = 0;
	r.esi.word = rpc->cookie1;
	r.edi.word = rpc->cookie2;

	VmwCommand( &r );

	if ( r.eax.word || r.ecx.halfs.high == 0 )
		return -1;

	if ( !length )
		return 0;

	/* send command string */

	if ( rpc->cookie1 && rpc->cookie2 ) {

		/* enhanced RPC */

		r.eax.word = VMWARE_MAGIC;
		r.ebx.word = VMRPC_ENH_DATA;
		r.ecx.word = length;
		r.edx.word = rpc->channel | VMWARE_RPC_PORT;
		r.esi.word = (uint32_t)command;
		r.edi.word = rpc->cookie2;
		r.ebp.word = rpc->cookie1;

		VmwRpcOuts( &r );

		if ( r.ebx.word != VMRPC_ENH_DATA)
			return -1;
	}
	else {
		int i;
		/* conventional RPC */

		for (;;) {
			r.eax.word = VMWARE_MAGIC;
			r.ebx.word = 0;
			for ( i = 0; i < length && i < 4; ++i )
				r.ebx.byte[i] = command[i];
			r.ecx.word = VMCMD_INVOKE_RPC | VMRPC_SENDDAT;
			r.edx.word = rpc->channel | VMWARE_CMD_PORT;
			r.ebp.word = r.edi.word = r.esi.word = 0;

			VmwCommand( &r );

			if ( r.eax.word || r.ecx.halfs.high == 0 )
				return -1;

			if ( length <= 4 )
				break;

			length -= 4;
			command += 4;
		}
	}

	return 0;
}


PUBLIC int VMRpcRecvLen(const rpc_t *rpc, uint32_t *length, uint16_t *dataid)
{
	/* receive result length */

	r.eax.word = VMWARE_MAGIC;
	r.ebx.word = 0;
	r.ecx.word = VMCMD_INVOKE_RPC | VMRPC_RECVLEN;
	r.edx.word = rpc->channel | VMWARE_CMD_PORT;
	r.ebp.word = 0;
	r.esi.word = rpc->cookie1;
	r.edi.word = rpc->cookie2;

	VmwCommand( &r );

	if ( r.eax.word || r.ecx.halfs.high == 0 )
		return -1;

	*length = r.ebx.word;
	*dataid = r.edx.halfs.high;

	return 0;
}

PUBLIC int VMRpcRecvDat(const rpc_t *rpc, unsigned char *data, uint32_t length, uint16_t dataid)
{
	if ( rpc->cookie1 && rpc->cookie2 ) {
		
		/* enhanced RPC */
				
		r.eax.word = VMWARE_MAGIC;
		r.ebx.word = VMRPC_ENH_DATA;
		r.ecx.word = length;
		r.edx.word = rpc->channel | VMWARE_RPC_PORT;
		r.esi.word = rpc->cookie1;
		r.edi.word = (uint32_t)data;
		r.ebp.word = rpc->cookie2;

		VmwRpcIns( &r );

		if ( r.ebx.word != VMRPC_ENH_DATA )
			return -1;
	}
	else {
		/* conventional RPC */

		for (;;) {

			r.eax.word = VMWARE_MAGIC;
			r.ebx.word = dataid;
			r.ecx.word = VMCMD_INVOKE_RPC | VMRPC_RECVDAT;
			r.edx.word = rpc->channel | VMWARE_CMD_PORT;
			r.ebp.word = r.edi.word = r.esi.word = 0;
			
			VmwCommand( &r );

			if ( r.eax.word || r.ecx.halfs.high == 0 )
				return -1;

			*(data++) = r.ebx.word;

			if (length <= 4)
				break;

			length -= 4;
		}
	}

	r.eax.word = VMWARE_MAGIC;
	r.ebx.word = dataid;
	r.ecx.word = VMCMD_INVOKE_RPC | VMRPC_RECVEND;
	r.edx.word = rpc->channel | VMWARE_CMD_PORT;
	r.ebp.word = 0;
	r.esi.word = rpc->cookie1;
	r.edi.word = rpc->cookie2;

	VmwCommand( &r );

	if (r.eax.word || r.ecx.halfs.high == 0)
		return -1;

	return 0;
}

