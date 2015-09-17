#ifndef _MESSAHES_H
#define _MESSAGES_H
#pragma once
/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *  Copyright (C) 2011  Eduardo Casino
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
 * 2011-10-15  Eduardo           * Messages for verbosity and buffer options
 * 2011-10-16  Eduardo           * Messages for uninstallation
 * 2011-11-02  Eduardo           * Messages for LFN support
 * 2011-11-06  Eduardo           * New /QQ option
 * 2011-11-06  Eduardo           * Only for 0.5 branch: Add 'b' version
 *
 */

#define MSG_MY_NAME				"VMSMOUNT %d.%db -"
#define MSG_COPYRIGHT			" (C) 2011 Eduardo Casino - GNU GPL Ver. 2.0\n"
#define MSG_HELP_1				"\nUsage:\n"
#define MSG_HELP_2				"   VMSMOUNT [/H][/V|/Q|/QQ] [/L:<drv>] [/B:<siz[K]>] [/LFN [/M:<n>] [/CS|/CI]]\n"
#define MSG_HELP_3				"   VMSMOUNT [/V|/Q/QQ] /U\n"
#define MSG_HELP_4				"        /H                  - Prints this message and exits\n"
#define MSG_HELP_5				"        /V                  - Verbose: Prints information on system resources\n"
#define MSG_HELP_6				"        /Q                  - Quiet: Omits copyright message\n"
#define MSG_HELP_7				"        /QQ                 - Silent: Does not print any messages at all\n"
#define MSG_HELP_8				"        /L:<drv>            - Drive letter to assign\n"
#define MSG_HELP_9				"                              (if omitted, use first available)\n"
#define MSG_HELP_10				"        /B:<siz[K]>         - Size of read/write buffer\n"
#define MSG_HELP_11				"                              (4K default, higher values increase performance)\n"
#define MSG_HELP_12				"        /LFN                - Long File Name support\n"
#define MSG_HELP_13				"        /M:<n>              - Number of mangling chars for short names\n"
#define MSG_HELP_14				"                              (2 minimum, 6 maximum, 3 default)\n"
#define MSG_HELP_15				"        /CI                 - Host file system is case insensitive\n"
#define MSG_HELP_16				"                              (default option)\n"
#define MSG_HELP_17				"        /CS                 - Host file system is case sensitive\n"
#define MSG_HELP_18				"                              (mangles file names whith lower case chars)\n"
#define MSG_HELP_19				"        /U                  - Uninstall\n"
#define MSG_ERROR_BUFFER		" ERROR: Buffer size must be between %u and %u bytes\n"
#define MSG_ERROR_INUSE			" ERROR: Drive %c: already in use\n"
#define MSG_ERROR_NO_DRIVES		" ERROR: No drive letter available (LASTDRIVE is %c)\n"
#define MSG_ERROR_INVALID_DRIVE	" ERROR: Invalid drive letter %c (LASTDRIVE is %c)\n"
#define MSG_ERROR_BADOS			" ERROR: Usupported DOS version %d.%d. Need 5.0 or higher.\n"
#define MSG_ERROR_LOL			" ERROR: Can't get the List-Of-Lists!\n"
#define MSG_ERROR_SDA			" ERROR: Can't get the SDA!\n"
#define MSG_ERROR_NLSINFO		" ERROR: Can't get the NLS tables.\n"
#define MSG_ERROR_NOVIRT		" ERROR: Not running on top of VMWARE.\n"
#define MSG_ERROR_NOSHF			" ERROR: No Shared Folders found.\n"
#define MSG_ERROR_INSTALLED		" ERROR: Already installed. Use /U to uninstall.\n"
#define MSG_ERROR_REDIR_NOT_ALLOWED " ERROR: Redirectors are not allowed.\n"
#define MSG_ERROR_UNINSTALL		" ERROR: Unable to uninstall.\n"
#define MSG_ERROR_NOTINSTALLED	" ERROR: Driver not installed.\n"
#define MSG_ERROR_MANGLE		" ERROR: Mangle characters must be between %u and %u\n"
#define MSG_INFO_MOUNT			" Mounting Shared Folders in %c:\n"
#define MSG_INFO_UNINSTALL		" Successfully removed from memory.\n"
#define MSG_INFO_VMVERS			" INFO: Running on VMware %s Version %lu\n"
#define MSG_INFO_TZ				" INFO: UTC Offset is %ld seconds\n"
#define MSG_INFO_TBL			" INFO: Active page is cp%d. Loading unicode table %s\n"
#define MSG_INFO_LOAD			" INFO: Driver loaded into memory with %u bytes used.\n"
#define MSG_WARN_CP				" WARNING: Active code page not found"
#define MSG_WARN_UNICODE		" WARNING: Can't load Unicode table: %s"
#define MSG_WARN_TBLFORMAT		" WARNING: Invalid file format: %s"
#define MSG_WARN_NOTBL			" WARNING: Can't find Unicode table: %s"
#define MSG_WARN_437			". Defaulting to cp437\n"
#define MSG_WARN_TIMEZONE		" WARNING: TZ invalid or not defined, times will be shown in UTC.\n"

#endif /* _MESSAGES_H */
