#ifndef _GLOBALS_H
#define _GLOBALS_H
#pragma once
/*
 * VMSMOUNT
 *  A network redirector for mounting VMware's Shared Folders in DOS 
 *  Copyright (C) 2011  Eduardo Casino
 *
 * globals.h: Some global definitions used in various modules
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
 * 2011-10-01  Eduardo           * change Errorlevels as per Bernd Blaauw suggestion
 *                               * bump version number to 0.2
 * 2011-10-02  Eduardo           * bump version number to 0.3
 * 2011-10-09  Eduardo           * bump version number to 0.4
 * 2011-10-15  Eduardo           * New errorlevel ERR_BUFFER
 *                               * New verbosity options
 * 2011-10-17  Eduardo           * New errorlevels ERR_UNINST/ERR_NOTINST
 * 2011-10-18  Eduardo           * bump version number to 0.5
 * 2011-11-06  Eduardo           * New message printing macros
 *
 */

#include "kitten.h"
#include "vmdos.h"
#include "vmshf.h"

#define PUBLIC

#define VERSION_MAJOR	0
#define VERSION_MINOR	5

#define TRUE	1
#define FALSE	0

#define ERR_SUCCESS	0
#define ERR_UNINST	245
#define ERR_NOTINST	246
#define ERR_BUFFER	247
#define ERR_BADOPTS	248
#define ERR_WRONGOS	249
#define ERR_NOVIRT	250
#define ERR_NOSHF	251
#define ERR_NOALLWD	252
#define ERR_INSTLLD	253
#define ERR_BADDRV	254
#define ERR_SYSTEM	255


#define VERB_FPRINTF(L, F, ...) if ( verbosity >= L ) fprintf( F, __VA_ARGS__ )
#define VERB_FPUTS(L, S, F) if ( verbosity >= L ) fputs( S, F )

typedef enum { SILENT, QUIET, NORMAL, VERBOSE } Verbosity;

extern nl_catd cat;
extern Verbosity verbosity;

#endif /* _GLOBALS_H */
