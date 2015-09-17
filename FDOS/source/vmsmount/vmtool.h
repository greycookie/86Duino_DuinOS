/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *
 * This file is a derivative work of the VMware Command Line Tools, by Ken Kato
 *        http://sites.google.com/site/chitchatvmback/
 *
 * vmtool.h: VMware backdoor call wrapper functions
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

#ifndef _VMTOOL_H_
#define _VMTOOL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

/*
	magic number and port numbers
*/
#define VMWARE_MAGIC		0x564D5868UL	/* Beckdoor magic number (VMXh)	*/
#define VMWARE_CMD_PORT		0x5658			/* Backdoor command port (VX)	*/
#define VMWARE_RPC_PORT		0x5659			/* RPC data transfer port (VY)	*/

/*
	backdoor command numbers
*/
#define VMCMD_GET_MEGAHERZ	0x01			/* Get processor MHz			*/
#define VMCMD_APM_FUNCTION	0x02			/* APM function					*/
#define VMCMD_GET_DISKGEO	0x03			/* ? Get disk geometry			*/
#define VMCMD_GET_CURSOR	0x04			/* Get cursor position			*/
#define VMCMD_SET_CURSOR	0x05			/* Set cursor position			*/
#define VMCMD_GET_CLIPLEN	0x06			/* Get host->guest copy length	*/
#define VMCMD_GET_CLIPDATA	0x07			/* Get host->guest copy data	*/
#define VMCMD_SET_CLIPLEN	0x08			/* Set guest->host copy length	*/
#define VMCMD_SET_CLIPDATA	0x09			/* Set guest->host copy data	*/
#define VMCMD_GET_VERSION	0x0a			/* Get version number			*/
#define VMCMD_GET_DEVINFO	0x0b			/* Get device information 		*/
#define VMCMD_SET_DEVSTAT	0x0c			/* Set device state				*/
#define VMCMD_GET_OPTIONS	0x0d			/* Get GUI option settings		*/
#define VMCMD_SET_OPTIONS	0x0e			/* Set GUI option settings		*/
#define VMCMD_GET_SCRSIZE	0x0f			/* Get host's screen size		*/
#define VMCMD_UNUSED_10H	0x10			/* ? monitorControl				*/
#define VMCMD_GET_HARDWARE	0x11			/* Get Virtual HW version 		*/
#define VMCMD_OSNOTFOUND	0x12			/* OS not found message popup	*/
#define VMCMD_GET_BIOSUUID	0x13			/* Get BIOS UUID				*/
#define VMCMD_GET_MEMSIZE	0x14			/* Get VM memory size in MB		*/
#define VMCMD_UNUSED_15H	0x15			/* ? (likely to be unused)		*/
#define VMCMD_OS2INTCUR		0x16			/* ? getOS2IntCursor			*/
#define VMCMD_GET_GMTIME	0x17			/* Get host's time (GMT)		*/
#define VMCMD_STOPCATCHUP	0x18			/* ? stopCatchup				*/
#define VMCMD_UNUSED_19H	0x19			/* ? (likely to be unused)		*/
#define VMCMD_UNUSED_1AH	0x1a			/* ? (likely to be unused)		*/
#define VMCMD_UNUSED_1BH	0x1b			/* ? (likely to be unused)		*/
#define VMCMD_INITSCSIOP	0x1c			/* ? initScsiOprom				*/
#define VMCMD_UNUSED_1DH	0x1d			/* ? (likely to be unused)		*/
#define VMCMD_INVOKE_RPC	0x1e			/* Invoke RPC command.			*/
#define VMCMD_RESERVED_0	0x1f			/* ? rsvd0	host memory size?	*/
#define VMCMD_RESERVED_1	0x20			/* ? rsvd1						*/
#define VMCMD_RESERVED_2	0x21			/* ? rsvd2						*/
#define VMCMD_ACPID			0x22			/* ? ACPID						*/
#define VMCMD_UNUSED_23h	0x23			/* ? (likely to be unused)		*/
#define VMCMD_UNUSED_24H	0x24			/* ? (likely to be unused)		*/
#define VMCMD_PATCHSMBIOS	0x25			/* ? PatchSMBIOS				*/
#define VMCMD_UNUSED_26H	0x26			/* ? (likely to be unused)		*/
#define VMCMD_ABSMOUSEDAT	0x27			/* ? absMouseDataDisable		*/
#define VMCMD_ABSMOUSESTA	0x28			/* ? absMouseStatusDisable		*/
#define VMCMD_ABSMOUSECMD	0x29			/* ? absMouseCommandDisable		*/
#define VMCMD_UNKNOWN_2AH	0x2a			/* ? (likely to be unused)		*/
#define VMCMD_PATCHACPITB	0x2b			/* ? PatchACPITables			*/
#define VMCMD_GETTIMEFULL	0x2e			/* 								*/

#define VMCMD_MAXVAL_WS2	0x21			/* max value for VMware 2.x		*/
#define VMCMD_MAXVAL_WS3	0x21			/* max value for VMware 3.x		*/
#define VMCMD_MAXVAL_WS40	0x22			/* max value for VMware 4.0		*/
#define VMCMD_MAXVAL_WS45	0x29			/* max value for VMware 4.5		*/
#define VMCMD_MAXVAL_WS5	0x2b			/* max value for VMware 5.x		*/

/*
	knowm APM function/parameter combination (command 0x02)
*/
#define VMAPM_SUSPEND		0x00070004UL
#define VMAPM_POWEROFF		0x00070007UL

/*
	copy and paste max length (commands 0x06, 0x07, 0x08, 0x09)
*/
#define VMCLIP_MAXLEN_WS40	65355			/* max data len up to WS 4.0	*/
#define VMCLIP_MAXLEN_WS45	65436			/* max data len since WS 4.5	*/

/*
	connectable device information (commands 0x0b, 0x0c)
*/
#define VMDEV_DEVICE_MAX	52				/* device number = 0 - 51		*/
#define VMDEV_INFO_LENGTH	40				/* device information length	*/
#define VMDEV_STATE_OFFSET	36				/* state data in device info	*/

/*
	GUI options bitmask (commands 0x0d, 0x0e)
*/
#define VMPREF_GRAB_MOVE	0x0001			/* grab by cursor movement		*/
#define VMPREF_UNGRAB_MOVE	0x0002			/* ungrab by cursor movement	*/
#define	VMPREF_SCROLL_MOVE	0x0004			/* scroll by cursor movement	*/
#define VMPREF_COPY_PASTE	0x0010			/* copy and paste 				*/
#define VMPREF_FULLSCREEN	0x0040			/* full screen (read only)		*/
#define VMPREF_ENTER_FULL	0x0080			/* full screen (write only)		*/
#define VMPREF_SYNCRONIZE	0x0400			/* time syncronization			*/

/*
	BIOS UUID length (command 0x13)
*/
#define VMWARE_UUID_LENGTH	16				/* UUID length in bytes			*/

/*
	RPC subcommand values (command 0x1e)
*/
#define VMRPC_OPEN			0x00000000UL	/* Open RPC channel				*/
#define VMRPC_SENDLEN		0x00010000UL	/* Send RPC command length		*/
#define VMRPC_SENDDAT		0x00020000UL	/* Send RPC command				*/
#define VMRPC_RECVLEN		0x00030000UL	/* Receive RPC reply length		*/
#define VMRPC_RECVDAT		0x00040000UL	/* Receive RPC reply			*/
#define VMRPC_RECVEND		0x00050000UL	/* Unknown backdoor command		*/
#define VMRPC_CLOSE			0x00060000UL	/* Close RPC channel			*/

/*
	RPC command magic numbers (command 0x1e)
*/
#define VMRPC_OPEN_RPCI		0x49435052UL	/* 'RPCI' channel open magic	*/
#define VMRPC_OPEN_TCLO		0x4f4c4354UL	/* 'TCLO' channel open magic	*/

#define VMRPC_ENHANCED		0x80000000UL	/* enhanced RPC open magic bit	*/
#define VMRPC_ENH_DATA		0x00010000UL	/* enhanced RPC data xfer megic	*/

/*
	private RPC context struct
*/
typedef struct _rpc {
	uint32_t channel;						/* channel number				*/
	uint32_t cookie1;						/* cookie#1 for enhanced rpc	*/
	uint32_t cookie2;						/* cookie#2 for enhanced rpc	*/
} rpc_t;

/*
	private return values
*/
#define VMTOOL_SUCCESS		0				/* success						*/
#define VMTOOL_STX_ERROR	1				/* syntax error					*/
#define VMTOOL_CMD_ERROR	2				/* backdoor call error			*/
#define VMTOOL_RPC_ERROR	3				/* RPC protocol error			*/
#define VMTOOL_SYS_ERROR	4				/* system error					*/


/*
	backdoor I/O wrapper functions
*/

/*
	send data with command 0x1e, subcommands 1, 2 and/or enhanced data channel
*/
int	VMRpcSend(const rpc_t *rpc, const unsigned char *command, uint32_t length);

/*
	receive data length with command 0x1e, subcommand 3
*/
int VMRpcRecvLen(const rpc_t *rpc, uint32_t *length, uint16_t *dataid);

/*
	receive data with command 0x1e, subcommands 4,5 and/or enhanced data channel
*/
int VMRpcRecvDat(const rpc_t *rpc, unsigned char *data, uint32_t length, uint16_t dataid);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* _VMTOOL_H_ */
