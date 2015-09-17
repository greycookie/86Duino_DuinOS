/* BOOTFIX - Boot sector checking utility.
   Copyright (c) 2004 by Arkady V.Belousov

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <string.h>	/* memcpy, memcmp */
#include <stdio.h>	/* printf */
#ifndef __WATCOMC__
# include <dos.h>	/* REGS, SREGS, int86, int86x, FP_SEG, FP_OFF */
#endif

#include "bootfix.h"

#define PGM		"BOOTFIX"
#define VERSION		"1.4"
#define YEARS		"2004"

/*======================================================================*/

int dump = 0;

#ifndef __WATCOMC__
union REGS r;
struct SREGS segr;
#endif

#include "diskio.inc"
#include "getbpb.inc"

/*----------------------------------------------------------------------*/

#ifdef __WATCOMC__

void truename (char far *dest, CStr src);
# if defined __TINY__ || defined __SMALL__ || defined __MEDIUM__
#  pragma aux truename =				\
	"mov ah,0x60"					\
	"int 0x21"					\
	parm [es di] [si]				\
	modify exact [ax];
# else /* defined __COMPACT__ || defined __LARGE__ || defined __HUGE__ */
#  pragma aux truename =				\
	"mov ah,0x60"					\
	"int 0x21"					\
	parm [es di] [ds si]				\
	modify exact [ax];
# endif

#else

# define truename(dst,src) (void)(			\
	segr.es = FP_SEG (dst), r.x.di = FP_OFF (dst),	\
	segr.ds = FP_SEG (src), r.x.si = FP_OFF (src),	\
	r.h.ah = 0x60,					\
	int86x (0x21, &r, &r, &segr))

#endif

/*----------------------------------------------------------------------*/

#define ERR	"ERROR: "
#define WARN	"WARNING: "

void PROC_REG say     (CStr s)			{ printf (s);		  }
#define       say2(f,s)				  printf (f, s)
void PROC_REG sayerr  (CStr s)			{ say2 (ERR  "%s.\n", s); }
void PROC_REG saywarn (CStr s)			{ say2 (WARN "%s.\n", s); }
void PROC_REG sayval  (CStr f, unsigned v)	{ printf (f, v);	  }

/*----------------------------------------------------------------------*/

void PROC_STK dump_sector (CStr title, const byte *mem) {
	say (title);
	size_t pos = 0;
	do {	sayval ("%04X ", pos);
		do sayval (" %02X", *mem), mem++, pos++; while (pos % 8u);
		say ("-");
		do sayval ("%02X ", *mem), mem++, pos++; while (pos % 8u);
		say (" ");
		pos -= 16u, mem -= 16u;
		do {	byte c = *mem;
			if (c < (byte)' ') c = '.';
			sayval ("%c", c), mem++, pos++;
		} while (pos % 16u);
		say ("\n");
	} while (pos < 32u*16u);
}

enum FS_t { NOBPB = 0, FAT12 = 12, FAT16 = 16, FAT32 = 32 };

FS_t PROC_STK check_BPB (const BPB32 *pb, const BOOTSECINFO *pi) {
	ulong FAT_sz = pb->FAT_sectors ? pb->FAT_sectors : pb->xFAT_sectors;
	unsigned cluster_sz = pb->cluster_sectors;
	if (dump) {
		printf ("  Bytes/sector\t\t: %u\n"
			"  Sectors/cluster\t: %u\n"
			"  FAT start, size\t: %u, %lu * %u\n"
			"  Root %s\t\t: %lu\n"
			"  Total sectors\t\t: %u, %lu\n"
			"  Hidden sectors\t: %lu\n"
			"  Sectors/track, heads\t: %u, %u\n",
			pb->sector_bytes,
			cluster_sz,
			pb->FAT_start, FAT_sz, pb->FATs,
			pb->FAT_sectors ? "entries" : "cluster",
			pb->FAT_sectors ? (ulong)pb->root_entries : pb->root_cluster,
			pb->total_sectors, pb->xTotal_sectors,
			pi && pi->signature != 0x29 ? (ulong)(word)pb->hidden_sectors
								 : pb->hidden_sectors,
			pb->track_sectors, pb->heads);

		if (pb->FAT_sectors == 0)
			printf ("  Version, mirror flags\t: %04Xh, %02Xh\n"
				"  Info, backup sectors\t: %u, %u\n",
				pb->version, pb->mirror,
				pb->info_sector, pb->backup_sector);

		if (pi && pi->signature == 0x29)
			printf ("  Drive, media ID, FS\t:  %02Xh, %02Xh, \"%.8s\"\n"
				"  Serial#, label\t: %08lXh, \"%.11s\"\n",
				pi->drive_no, pb->media_ID, pi->FS_type,
				pi->serial_no, pi->label);
		else	sayval ("  Media ID\t\t: %02Xh\n", pb->media_ID);
	}

	int err = 0;
	if (pb->sector_bytes != SEC_SZ) {
		sayerr ("not supported sector size");
		err++;
	}
	/* == 0 || > 128 || not power of two ? */
	if (cluster_sz-1 >= 128 || ((cluster_sz-1) & cluster_sz)) {
		sayerr ("wrong cluster size");
		err++;
		cluster_sz = 128;
	}
	ulong data_start;
	if (pb->FAT_start == 0 || (data_start = FAT_sz * pb->FATs) == 0) {
		sayerr ("wrong FAT start or size");
		err++;
	}
	unsigned entries = pb->root_entries;
	if (pb->FAT_sectors == 0 && entries) {
		saywarn ("nonzero root entries field");
		entries = 0;
	}
	if (entries % (SEC_SZ / DIRENT_SZ)) {
		saywarn ("root entries are unaligned to sector boundary");
		err++;
	}
	ulong total = pb->total_sectors ? pb->total_sectors : pb->xTotal_sectors;
	if (total < (data_start = data_start + pb->FAT_start
				+ (entries + (SEC_SZ / DIRENT_SZ - 1))
					   / (SEC_SZ / DIRENT_SZ))) {
		sayerr ("too small volume size");
		err++;
		total = data_start;
	}
	/* total clusters = data clusters + 2 */
	ulong last = (total - data_start) / cluster_sz + 1u;
	if (pb->FAT_sectors) {
		if (last > FAT16_LAST) {
			sayerr ("too many FAT16 clusters");
			err++;
		}
	} else {
		if (pb->root_cluster-2 >= last - 1) {
			sayerr ("wrong root cluster");
			err++;
		}
		if (last <= FAT16_LAST) {
			sayerr ("too few FAT32 clusters");
			err++;
	}	}
	if (pi) {
		if (pi->signature != 0x29)
			saywarn ("no extended boot record signature");
		else if (memcmp (pi->FS_type,
				 pb->FAT_sectors == 0 ?	"FAT32   " :
				 last > FAT12_LAST   ?	"FAT16   " :
							"FAT12   ", 8))
			saywarn ("FS type string is wrong");
	}
	ulong calc_FAT_sz = (pb->FAT_sectors == 0 ?
		last			   / (SEC_SZ / 4u) :
				last > FAT12_LAST ?
		last			   / (SEC_SZ / 2u) :
		((unsigned)last * 3u + 2u) / (2u * SEC_SZ)) + 1;
	if (calc_FAT_sz > FAT_sz) {
		sayerr ("FAT too short");
		err++;
	} else if (calc_FAT_sz != FAT_sz)
		printf (WARN "%lu FAT sectors is enough.\n", calc_FAT_sz);
	if (pb->track_sectors == 0 || pb->heads == 0)
		saywarn ("undefined geometry (tracks,heads)");

	if (err)	      return NOBPB;
			   FS_t fs = FAT32;
	if (pb->FAT_sectors) {	fs = FAT12;
	if (last > FAT12_LAST)	fs = FAT16; }
	sayval ("Volume is a FAT%u.\n", fs);
	return fs;
}

/*----------------------------------------------------------------------*/

int PROC_REG bpb_cmp (const BPB32 *pb1, const BPB32 *pb2) {
	return memcmp (pb1, pb2, pb1->FAT_sectors ? sizeof (BPB) : sizeof (BPB32));
}

int PROC_STK check_drive (byte drive) {
	BPB32 def_bpb, cur_bpb;
	FS_t cur_fs;

	say ("Reading default BPB...\n");
	if (get_BPB (drive, DEFAULT, &def_bpb)) {
		saywarn ("BPB not available or wrong");
		cur_fs = NOBPB;
	} else	cur_fs = check_BPB (&def_bpb, NULL);

	say ("Reading current BPB...\n");
	if (get_BPB (drive, CURRENT, &cur_bpb)) {
		saywarn ("BPB not available or wrong");
		cur_fs = NOBPB;
	} else if (cur_fs == NOBPB) {
		cur_fs = check_BPB (&cur_bpb, NULL);
	} else if (bpb_cmp (&def_bpb, &cur_bpb)) {
		saywarn ("default and current BPB are different");
		cur_fs = check_BPB (&cur_bpb, NULL);
	}

	say ("Reading boot sector...\n");
	bootsector_t curboot;
	if (diskIO (drive, 0, 1, &curboot, IO_READ)) {
		sayval (ERR "read: %c:\n", 'A' + drive);
		return 1;
	}
	int err = 1;
	for (;;) {
		if (*(word*)(curboot.dump + 510) != 0xAA55) {
			sayerr ("no boot sector signature (AA55h)");
			break;
		}
		if (*(word*)(curboot.dump + 508))
			saywarn ("no Win95 boot sector signature (0)");
		if (check_BPB (&curboot._32.bpb, curboot._.bpb.FAT_sectors
						? &curboot._  .i
						: &curboot._32.i) == NOBPB) {
			sayerr ("wrong boot record");
			break;
		}
		if (cur_fs != NOBPB && bpb_cmp (&cur_bpb, &curboot._32.bpb)) {
			sayerr ("boot record and current BPB are different");
			break;
		}
		err = 0; break;
	}
	if (dump) dump_sector ("\nBoot sector:\n", curboot.dump);
	return err;
}

/*======================================================================*/

#define HELP(retcode) \
  (say ("Syntax: " PGM " [/d] drive:\n\n" \
	"Options:\n\n" \
	"  /d  Dump default BPB and boot sector.\n"), \
	(retcode))

int _Cdecl main (int argc, PCStr *argv) {
	say (PGM " v" VERSION " - Boot sector checking utility.\n"
		"Copyright (c) " YEARS " by Arkady V.Belousov, licensed under GPL2.\n\n");

	/*--- parse command line */

	PCStr dstarg = NULL;
	for (argv++; --argc; argv++) {
		PCStr argp = *argv;
		if (*argp == '\0') continue;

		if (*argp == '/') {
			argp++;
			if (_upper (*argp) == 'D' && argp [1] == '\0') {
				dump = 1; continue;
			}
			return HELP (1);
		}
		if (dstarg == NULL) { dstarg = argp; continue; }
		return HELP (1);
	} /* for */

	/*--- check drive parameter */

	if (dstarg == NULL || dstarg [1] != ':' || dstarg [2])
		return HELP (1);

	say2 ("Checking drive %s\n", dstarg);
	static char tmp [] = " :\\"; tmp [0] = _upper (*dstarg);
	static char tmpbuf [SYS_MAXPATH] /*= ""*/;
	truename (tmpbuf, tmp);
	if (tmpbuf [0] == '\0') {
		sayerr ("no such drive");
		return 1;
	}
	if (memcmp (tmpbuf, tmp, 4)) {
		sayerr ("SUBSTed or networked drive");
		return 1;
	}

	/*--- process drive */

	return check_drive ((byte)(tmpbuf [0] - 'A'));
}

/*======================================================================*/

#ifdef __WATCOMC__
# include "malloc.inc"
#endif

#ifdef __BORLANDC__
# define NOBUFFER
# include "setupio.ib"
#endif
