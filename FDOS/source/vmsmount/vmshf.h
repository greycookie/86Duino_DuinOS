#ifndef _VMSHF_H_
#define _VMSHF_H_
#pragma once
/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *
 * vmshf.h: file transfer via VMware Shared Folders
 *
 * This file is a derivative work of the VMware Command Line Tools, by Ken Kato
 *        http://sites.google.com/site/chitchatvmback/
 *
 * Copyright (c) 2002-2008, Ken Kato
 * Copyright (c) 2011,      Eduardo Casino
 *    Updated to HGFS protocol V3
 *    (Info about HGFS V3 from the Open VM Tools <http://open-vm-tools.sourceforge.net/>)
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
 * 2011-10-15  Eduardo           * Configurable buffer size and code cleanup
 *                               * Allow VMShfSetAttr() to be called by handle
 *                                 (Needed for fixing the "write 0" bug)
 * 2011-11-01  Eduardo           * Long file name support
 *
 */

#include <stdint.h>
#include "vmtool.h"

/*
	absolute maximum transfer block size for HGFS V3 servers. Do not exceed!!!
*/
#define VMSHF_MAX_BLOCK_SIZE	61440

/*
	default transfer block size
*/
#define VMSHF_DEF_BLOCK_SIZE	4096

/*
	minimum ransfer block size
*/
#define VMSHF_MIN_BLOCK_SIZE	2048

/*
	data transfer buffer size required for shared folder RPC call
*/
#define VMSHF_MAX_DATA_SIZE(BLOCK_SIZE)	(BLOCK_SIZE - ( sizeof( DataReq ) -1 ) )

/*
	max path string length
*/
#define VMSHF_MAX_PATH			1024

/*
	known shared folder command values
*/
#define VMSHF_OPEN_FILE_V3		24		// open/create file
#define VMSHF_READ_FILE_V3		25		// read data from file
#define VMSHF_WRITE_FILE_V3		26		// write data into file
#define VMSHF_CLOSE_FILE_V3		27		// close file
#define VMSHF_OPEN_DIR_V3		28		// open directory
#define VMSHF_READ_DIR_V3		29		// read a directory entry
#define VMSHF_CLOSE_DIR_V3		30		// close directory
#define VMSHF_GET_ATTR_V3		31		// get file/directory attributes
#define VMSHF_SET_ATTR_V3		32		// set file/directory attributes
#define VMSHF_CREATE_DIR_V3		33		// create directory
#define VMSHF_DELETE_FILE_V3	34		// delete file
#define VMSHF_DELETE_DIR_V3		35		// delete directory
#define VMSHF_MOVE_FILE_V3		36		// move/rename file/directory
#define VMSHF_GET_DIRSIZE_V3	37		// get directory size
#define VMSHF_CREATE_SIMLINK_V3	38		// create symlink
#define VMSHF_SERVER_LCK_CHG_V3	39		// change oplock on a file

#define VMSHF_CMD_MAX			40		// maximum value place holder

/*
	known shared folder status values
*/
#define VMSHF_SUCCESS			0
#define VMSHF_ENOTEXIST			1		// file not exists
#define VMSHF_EINVALID			2		// invalid handle
#define VMSHF_ENOTPERM			3		// operation not permitted
#define VMSHF_EEXIST			4		// file exists
#define VMSHF_ENODIR			5		// not a directory
#define VMSHF_ENOTEMPTY			6		// directory is not empty
#define VMSHF_EPROTOCOL			7		// protocol error
#define VMSHF_EACCESS			8		// access denied
#define VMSHF_ENAME				9		// invalid name
#define VMSHF_EGENERIC			10		// generic error
#define VMSHF_ESHARING			11		// sharing violation
#define VMSHF_ENOSPACE			12		// no space
#define VMSHF_EUNSUPPORTED		13		// operation not supported
#define VMSHF_ETOOLONG			14		// name too long
#define VMSHF_EPARAMETER		15		// invalid parameter
#define VMSHF_EDEVICE			16		// not same device

#define VMSHF_VMTOOLERR			(uint32_t)-1

#define VMSHF_INVALID_HANDLE	~0


#pragma pack(push, 1)


/* V3 File Name structure */

/*
	case sensitiveness flags
*/
#define VMSHF_CASE_DEFAULT		0x00	// use server default
#define VMSHF_CASE_SENSITIVE	0x01	// force case sensitive
#define VMSHF_CASE_INSENSITIVE	0x02	// force case insensitive

/*
	flags
*/
#define VMSHF_FILE_NAME_USE_NAME	0x00	// use file name
#define VMSHF_FILE_NAME_USE_HANDLE	0x01	// use handle instead of file name

typedef struct {
	uint32_t length;					// does NOT include terminating NULL
	uint32_t flags;						// 0 -> use name, 1-> use handle
	uint32_t caseType;					// case-sensitivity type
	uint32_t handle;					// file/dir handle
	char name[1];
} VMShfFileName;



/* V3 File Attributes structure */

/*
	valid attribute fields
*/
#define VMSHF_VALID_ATTR_NONE	0x0000
#define VMSHF_VALID_ATTR_FTYPE	0x0001
#define VMSHF_VALID_ATTR_FSIZE	0x0002
#define VMSHF_VALID_ATTR_CTIME	0x0004
#define VMSHF_VALID_ATTR_ATIME	0x0008
#define VMSHF_VALID_ATTR_UTIME	0x0010
#define VMSHF_VALID_ATTR_XTIME	0x0020
#define VMSHF_VALID_ATTR_SMODE	0x0040
#define VMSHF_VALID_ATTR_FMODE	0x0080
#define VMSHF_VALID_ATTR_GMODE	0x0100
#define VMSHF_VALID_ATTR_OMODE	0x0200
#define VMSHF_VALID_ATTR_FLAGS	0x0400
#define VMSHF_VALID_ATTR_ASIZE	0x0800
#define VMSHF_VALID_ATTR_UID	0x1000
#define VMSHF_VALID_ATTR_GID	0x2000
#define VMSHF_VALID_ATTR_HOSTID	0x4000
#define VMSHF_VALID_ATTR_VOLID	0x0800

/*
	file permission mask
*/
#define VMSHF_FILEMODE_EXEC		0x01	// file is executable
#define VMSHF_FILEMODE_WRITE	0x02	// file is writable
#define VMSHF_FILEMODE_READ		0x04	// file is readable
#define VMSHF_FILEMODE_MASK		0x07	// valid filemode bit mask
#define VMSHF_FILEMODE_ALL		0x07

/*
	windows attributes flag (use in case of windows host, same as MSDOS)
*/
#define VMSHF_ATTR_HIDDEN		0x01
#define VMSHF_ATTR_SYSTEM		0x02
#define VMSHF_ATTR_ARCHIVE		0x04
#define VMSHF_ATTR_ALL			0x07

/*
	valid file types
*/
#define VMSHF_TYPE_FILE			0x00	// regular file
#define VMSHF_TYPE_DIRECTORY	0x01	// directory
#define VMSHF_TYPE_SYMLINK		0x02	// symlink (ignored)

typedef struct {
	uint64_t mask;						// valid attribute mask
	uint32_t ftype;						// file type (file, dir, symlink)
	uint64_t fsize;						// file size (bytes)
	uint64_t ctime;						// creation date/time (ignored)
	uint64_t atime;						// last accessed date/time (ignored)
	uint64_t utime;						// last updated date/time
	uint64_t xtime;						// last changed attr date/time (ignored)
	uint8_t smode;						// special mode bits (suid, etc.) (ignored)
	uint8_t fmode;						// owner mode bits ( VMSHF_FILEMODE_* )
	uint8_t gmode;						// group mode bits (ignored)
	uint8_t omode;						// other mode bits (ignored)
	uint64_t attr;						// windows specific attributes ( VMSHF_ATTR_* )
	uint64_t asize;						// actual allocated size of file on disk (ignored)
	uint32_t uid;						// user identifier (ignored)
	uint32_t gid;						// group identifier (ignored)
	uint64_t hostid;					// id of the file on host (ignored)
	uint32_t volid;						// volume identifier (ignored)
	uint32_t emode;						// effective mode for the user on the host (ignored)
	uint64_t reserved;					// reserved ( 0ui64 )
} VMShfAttr;

/* Common request/reply headers */

typedef struct {
	uint16_t command;					// ' f'
	uint32_t id;						// request ID
	uint32_t op;						// SHF Operation
} VMShfRequestHeader;

typedef struct {
	uint16_t reply;						// ' 1' if success
	uint32_t id;						// request ID (host returns same ID as given in the request
	uint32_t status;					// request status
} VMShfReplyHeader;


/* Open Dir Search Operation */

typedef struct {
	uint64_t reserved;
	VMShfFileName dir;
} VMShfOpenDirRequestData;

typedef struct {
	uint32_t handle;					// dir search handle
	uint64_t reserved;
} VMShfOpenDirReplyData;

typedef struct {
	VMShfRequestHeader header;
	VMShfOpenDirRequestData data;
} VMShfOpenDirRequest;

typedef struct {
	VMShfReplyHeader header;
	VMShfOpenDirReplyData data;
} VMShfOpenDirReply;


/* Read Dir Operation */

typedef struct {
	uint32_t handle;					// dir search handle
	uint32_t index;						// first entry is 0
	uint32_t flags;						// reserved for multiple dir reads (unused)
	uint64_t reserved;
} VMShfReadDirRequestData;


// In actuallity, this structure is a simplification, because we're not reading
// multiple directory entries
//
typedef struct {
	uint64_t count;						// number of directory entries (unused, should be 1)
	uint64_t reserved;

	uint32_t nextEntry;					// unused, should be 0
	VMShfAttr attr;						// file attributes
	VMShfFileName file;					// file.length == 0 means no entry at this index
} VMShfReadDirReplyData;

typedef struct {
	VMShfRequestHeader header;
	VMShfReadDirRequestData data;
} VMShfReadDirRequest;

typedef struct {
	VMShfReplyHeader header;
	VMShfReadDirReplyData data;
} VMShfReadDirReply;


/* Close File/Dir Search Operations */

typedef struct {
	uint32_t handle;					// file/dir handle
	uint64_t reserved;
} VMShfCloseFileDirRequestData;

typedef struct {
	uint64_t reserved;
} VMShfCloseFileDirReplyData;

typedef struct {
	VMShfRequestHeader header;
	VMShfCloseFileDirRequestData data;
} VMShfCloseFileDirRequest;

typedef struct {
	VMShfReplyHeader header;
	VMShfCloseFileDirReplyData data;	// unused
} VMShfCloseFileDirReply;


/* Get Dir Size Operation */

typedef struct {
	uint64_t reserved;
	VMShfFileName dir;
} VMShfGetDirSizeRequestData;

typedef struct {
	uint64_t freeBytes;
	uint64_t totalBytes;
	uint64_t reserved;
} VMShfGetDirSizeReplyData;

typedef struct {
	VMShfRequestHeader header;
	VMShfGetDirSizeRequestData data;
} VMShfGetDirSizeRequest;

typedef struct {
	VMShfReplyHeader header;
	VMShfGetDirSizeReplyData data;
} VMShfGetDirSizeReply;


/* Get and Set Attributes Operations */

#define VMSHF_ATTR_HINT_NONE		0x00
#define VMSHF_ATTR_HINT_SET_ATIME	0x01
#define VMSHF_ATTR_HINT_SET_UTIME	0x02
#define VMSHF_ATTR_HINT_USE_HANDLE	0x04
typedef struct {
	uint64_t hints;						// hints if handle is valid
	uint64_t reserved;
	VMShfFileName file;
} VMShfGetAttrRequestData;

typedef struct {
	VMShfAttr attr;
	uint64_t reserved;
	VMShfFileName linkedFile;			// if attr.type is a symlink, this is the linked file ( ignored )
} VMShfGetAttrReplyData;

typedef struct {
	VMShfRequestHeader header;
	VMShfGetAttrRequestData data;
} VMShfGetAttrRequest;

typedef struct {
	VMShfReplyHeader header;
	VMShfGetAttrReplyData data;
} VMShfGetAttrReply;

typedef struct {
	uint64_t hints;						// hints if handle is valid
	VMShfAttr attr;
	uint64_t reserved;
	VMShfFileName file;
} VMShfSetAttrRequestData;

typedef struct {
	uint64_t reserved;
} VMShfSetAttrReplyData;

typedef struct {
	VMShfRequestHeader header;
	VMShfSetAttrRequestData data;
} VMShfSetAttrRequest;

typedef struct {
	VMShfReplyHeader header;
	VMShfSetAttrReplyData data;
} VMShfSetAttrReply;


/* Move/Rename File/Dir Operation */

#define VMSHF_RENAME_HINT_NONE				0x00
#define VMSHF_RENAME_HINT_USE_SRC_HANDLE	0x01
#define VMSHF_RENAME_HINT_USE_TARGET_HANDLE	0x02
#define VMSHF_RENAME_HINT_NO_REPLACE		0x04
#define VMSHF_RENAME_HINT_NO_COPY			0x08
typedef struct {
	uint64_t hints;
	uint64_t reserved;					// reserved ( 0ui64 )
	VMShfFileName src;
	VMShfFileName dst;
} VMShfMoveFileRequestData;

typedef struct {
	uint64_t reserved;
} VMShfMoveFileReplyData;

typedef struct {
	VMShfRequestHeader header;
	VMShfMoveFileRequestData data;
} VMShfMoveFileRequest;

typedef struct {
	VMShfReplyHeader header;
	VMShfMoveFileReplyData data;
} VMShfMoveFileReply;


/* Open File Operation */

/*
 * valid open request fields
 */
#define VMSHF_VALID_OPEN_NONE		0x0000
#define VMSHF_VALID_OPEN_ACCESS		0x0001
#define VMSHF_VALID_OPEN_FLAGS		0x0002
#define VMSHF_VALID_OPEN_SMODE		0x0004
#define VMSHF_VALID_OPEN_FMODE		0x0008
#define VMSHF_VALID_OPEN_GMODE		0x0010
#define VMSHF_VALID_OPEN_OMODE		0x0020
#define VMSHF_VALID_OPEN_ATTR		0x0040
#define VMSHF_VALID_OPEN_ASIZE		0x0080
#define VMSHF_VALID_OPEN_DACCESS	0x0100
#define VMSHF_VALID_OPEN_SACCESS	0x0200
#define VMSHF_VALID_OPEN_LOCK		0x0400
#define VMSHF_VALID_OPEN_FILE		0x0800

#define VMSHF_OPEN_VALID_FIELDS		( VMSHF_VALID_OPEN_ACCESS | VMSHF_VALID_OPEN_FLAGS | VMSHF_VALID_OPEN_FMODE \
									| VMSHF_VALID_OPEN_ATTR | VMSHF_VALID_OPEN_LOCK | VMSHF_VALID_OPEN_FILE )

/*
	file open access mode
*/
#define VMSHF_ACCESS_READONLY	0x00	// open for read only access
#define VMSHF_ACCESS_WRITEONLY	0x01	// open for write only access
#define VMSHF_ACCESS_READWRITE	0x02	// open for read/write access

/*
	file open mode
*/
#define VMSHF_ACTION_O_EXIST	0x00	// file must exist
#define VMSHF_ACTION_O_TRUNC	0x01	// truncated if exists, error if doesn't
#define VMSHF_ACTION_O_ALWAYS	0x02	// created if not exists
#define VMSHF_ACTION_C_NEW		0x03	// file must not exist
#define VMSHF_ACTION_C_ALWAYS	0x04	// truncated if exists

/*
 * valid lock types
 */
#define VMSHF_LOCK_NONE			0x00
#define VMSHF_LOCK_DEFAULT		0x01	// use host default
#define VMSHF_LOCK_EXCLUSIVE	0x02
#define VMSHF_LOCK_SHARED		0x03

typedef struct {
	uint64_t mask;						// valid fields mask
	uint32_t access;					// file access mode ( VMSHF_ACCESS_* )
	uint32_t action;					// file open action ( VMSHF_ACTION_* )
	uint8_t smode;						// special mode for file creation (ignored)
	uint8_t fmode;						// owner mode for file creation ( VMSHF_FILEMODE_* )
	uint8_t gmode;						// group mode for file creation (ignored)
	uint8_t omode;						// other permissions for file creation (ignored)
	uint64_t attr;						// windows specific attrs ( VMSHF_ATTR_* )
	uint64_t asize;						// space to allocate on disk for file (ignored)
	uint32_t daccess;					// windows access modes (ignored)
	uint32_t saccess;					// windows share access modes (ignored)
	uint32_t lock;						// requested lock ( VMSHF_LOCK_* )
	uint64_t reserved1;
	uint64_t reserved2;
	VMShfFileName file;
} VMShfOpenFileRequestData;

typedef struct {
	uint32_t handle;
	uint32_t lock;						// acquired lock
	uint64_t reserved;
} VMShfOpenFileReplyData;

typedef struct {
	VMShfRequestHeader header;
	VMShfOpenFileRequestData data;
} VMShfOpenFileRequest;

typedef struct {
	VMShfReplyHeader header;
	VMShfOpenFileReplyData data;
} VMShfOpenFileReply;


/* Read From File Operation */

typedef struct {
	uint32_t handle;
	uint64_t offset;
	uint32_t size;						// requested size. WARNING: Do not request more than VMSHF_BLOCK_SIZE bytes!!!
	uint64_t reserved;
} VMShfReadFileRequestData;

typedef struct {
	uint32_t size;						// actual size
	uint64_t reserved;
	char data[1];
} VMShfReadFileReplyData;

typedef struct {
	VMShfRequestHeader header;
	VMShfReadFileRequestData data;
} VMShfReadFileRequest;

typedef struct {
	VMShfReplyHeader header;
	VMShfReadFileReplyData data;
} VMShfReadFileReply;


/* Write To File Operation */

typedef struct {
	uint32_t handle;
	uint8_t flags;
	uint64_t offset;
	uint32_t size;
	uint64_t reserved;
	char data[1];
} VMShfWriteFileRequestData;

typedef struct {
	uint32_t size;
	uint64_t reserved;
} VMShfWriteFileReplyData;

typedef struct {
	VMShfRequestHeader header;
	VMShfWriteFileRequestData data;
} VMShfWriteFileRequest;

typedef struct {
	VMShfReplyHeader header;
	VMShfWriteFileReplyData data;
} VMShfWriteFileReply;


/* Delete File Operation */

#define VMSHF_DELETE_HINT_NONE			0x00
#define VMSHF_DELETE_HINT_USE_HANDLE	0x01

typedef struct {
	uint64_t hints;
	uint64_t reserved;
	VMShfFileName file;
} VMShfDeleteFileRequestData;

typedef struct {
	uint64_t reserved;
} VMShfDeleteFileReplyData;

typedef struct {
	VMShfRequestHeader header;
	VMShfDeleteFileRequestData data;
} VMShfDeleteFileRequest;

typedef struct {
	VMShfReplyHeader header;
	VMShfDeleteFileReplyData data;
} VMShfDeleteFileReply;


/* Create Directory Operation */

/*
 * valid create dir request fields
 */
#define VMSHF_CREATE_DIR_VALID_NONE		0x0000
#define VMSHF_CREATE_DIR_VALID_SMODE	0x0001
#define VMSHF_CREATE_DIR_VALID_FMODE	0x0002
#define VMSHF_CREATE_DIR_VALID_GMODE	0x0004
#define VMSHF_CREATE_DIR_VALID_OMODE	0x0008
#define VMSHF_CREATE_DIR_VALID_DIR		0x0010
#define VMSHF_CREATE_DIR_VALID_ATTR		0x0020

#define VMSHF_CREATE_DIR_VALID_FIELDS	( VMSHF_CREATE_DIR_VALID_FMODE | VMSHF_CREATE_DIR_VALID_DIR )

typedef struct {
	uint64_t mask;
	uint8_t smode;						// special mode (ignored)
	uint8_t fmode;						// owner mode
	uint8_t gmode;						// group mode (ignored)
	uint8_t omode;						// other mode (ignored)
	uint64_t attr;						// windows attributes
	VMShfFileName dir;
} VMShfCreateDirRequestData;

typedef struct {
	uint64_t reserved;
} VMShfCreateDirReplyData;

typedef struct {
	VMShfRequestHeader header;
	VMShfCreateDirRequestData data;
} VMShfCreateDirRequest;

typedef struct {
	VMShfReplyHeader header;
	VMShfCreateDirReplyData data;
} VMShfCreateDirReply;

typedef union {
	VMShfWriteFileRequest write;
	VMShfReadFileReply read;
} DataReq;

#pragma pack(pop)

extern rpc_t rpc;
extern uint16_t bufferSize;
extern uint16_t maxDataSize;
extern uint8_t *buffer;

#define VMShfCloseFile( H, ST )				VMShfCloseFileDir( VMSHF_CLOSE_FILE_V3, H, ST )
#define VMShfCloseDir( H, ST )				VMShfCloseFileDir( VMSHF_CLOSE_DIR_V3, H, ST )
#define VMShfDeleteFile( FN, UTF, ST )		VMShfDeleteFileDir( VMSHF_DELETE_FILE_V3, FN, UTF, ST )
#define VMShfDeleteDir( DN, UTF, ST )		VMShfDeleteFileDir( VMSHF_DELETE_DIR_V3, DN, UTF, ST )
#define VMShfRenameFile( FN1, FN2, UTF, ST )	\
								VMShfMoveFile( FN1, FN2, UTF, VMSHF_RENAME_HINT_NO_REPLACE | VMSHF_RENAME_HINT_NO_COPY, ST )

/*
	open a file in a shared folder
*/
extern int VMShfOpenFile(				// ret: backdoor error code
	uint32_t	access,			// in : access mode
	uint32_t	action,			// in : open action
	uint8_t		filemode,		// in : file permission
	uint32_t	fileattr,		// in : file attributes
	char far	*filename,		// in : file name
	uint8_t		utf,			// in : filename is in UTF				*/
	uint32_t	*status,		// out: shared folder status
	uint32_t	*handle);		// out: file handle

/*
	read data from a shared file
*/
extern int VMShfReadFile(				// ret: backdoor error code
	uint32_t	handle,			// in : file handle
	uint64_t	offset,			// in : byte offset to read
	uint32_t	*length,		// i/o: bytes to read / bytes retrieved
	char		**data,			// out: pointer to buffer with data
	uint32_t	*status);		// out: shared folder status

/*
	write data into a shared file
*/
extern int VMShfWriteFile(				// ret: backdoor error code
	uint32_t	handle,			// in : file handle
	uint8_t		flags,			// in : currently, 0 or HGFS_WRITE_APPEND
	uint64_t	offset,			// in : byte offset to write
	uint32_t	*length,		// i/o: bytes to write / bytes written
	char far	*data,			// in : data source pointer
	uint32_t	*status);		// out: shared folder status

/*
	close a shared file/dir
*/
extern int VMShfCloseFileDir(			// ret: backdoor error code
	uint32_t	op,				// in : operation (VMSHF_CLOSE_FILE_V3 or VMSHF_CLOSE_DIR_V3)
	uint32_t	handle,			// in : file handle
	uint32_t	*status);		// out: shared folder status

/*
	open a shared directory for file listing
*/
extern int VMShfOpenDir(				// ret: backdoor error code
	char far	*dirname,		// in : shared directory name
	uint8_t		utf,			// in : filename is in UTF				*/
	uint32_t	*status,		// out: shared folder status
	uint32_t	*handle);		// out: directory handle

/*
	read a file entry from a shared directory
*/
extern int VMShfReadDir(				// ret: backdoor error code
	uint32_t	handle,			// in : directory handle
	uint32_t	index,			// in : 0 based index number
	uint32_t	*status,		// out: shared folder status
	VMShfAttr	**fileattr,		// out: file attributes
	char		**filename, 	// out: file name
	uint32_t	*namelen);		// out: filename length

/*
	get shared file attributes
*/
extern int VMShfGetAttr(				// ret: backdoor error code
	char far	*filename,		// in : shared file name
	uint8_t		utf,			// in : filename is in UTF				*/
	uint32_t	handle,			// in : if valid, use instead of file name
	uint32_t	*status,		// out: shared folder status
	VMShfAttr	**fileattr);	// out: file attributes

/*
	set shared file attributes
*/
extern int VMShfSetAttr(				// ret: backdoor error code
	VMShfAttr	*fileattr,		// in : file attributes
	char far	*filename,		// in : file name
	uint8_t		utf,			// in : filename is in UTF				*/
	uint32_t	handle,			// in : if valid, use instead of file name
	uint32_t	*status);		// out: shared folder status

/*
	create a directory in a shared directory
*/
extern int VMShfCreateDir(				// ret: backdoor error code
	uint8_t		dirmode,		// in : directory permission
	char far	*dirname,		// in : directory name
	uint8_t		utf,			// in : filename is in UTF				*/
	uint32_t	*status);		// out: shared folder status

/*
	move / rename a file an a shared directory
*/
extern int VMShfMoveFile(				// ret: backdoor error code
	char far	*srcname,		// in : source file name
	char far	*dstname,		// in : destination file name
	uint8_t		utf,			// in : filenames are in UTF				*/
	uint64_t	hints,			// in : hints for move / rename
	uint32_t	*status);		// out: shared folder status

/*
	delete a file/directory in a shared directory
*/
extern int VMShfDeleteFileDir(			// ret: backdoor error code
	uint32_t	op,				// in : operation (VMSHF_DELETE_FILE_V3 or VMSHF_DELETE_DIR_V3)
	char far	*filename,		// in : file name
	uint8_t		utf,			// in : filename is in UTF				*/
	uint32_t	*status);		// out: shared folder status

/*
	get shared directory size (?)
*/
extern int VMShfGetDirSize(			// ret: backdoor error code
	char far	*dirname,		// in : directory name
	uint8_t		utf,			// in : filename is in UTF				*/
	uint32_t	*status,		// out: shared folder status
	uint64_t	*avail,			// out: available size in bytes
	uint64_t	*total);		// out: total size in bytes

#endif	/* _VMSHF_H_ */
