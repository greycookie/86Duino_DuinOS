#include <stddef.h>	/* offsetof */

#pragma option -a-	/* force byte alignment for BC++ 3.1 */
#pragma pack (1)	/* force byte alignment for Watcom */

#ifndef PROC_REG
# if defined __BORLANDC__
#  define PROC_REG __fastcall
#  define PROC_STK pascal
# elif defined __TURBOC__
#  define PROC_REG pascal
#  define PROC_STK pascal
# else
#  define PROC_REG
#  define PROC_STK
# endif
#endif

#ifndef _Cdecl
# define _Cdecl
#endif

#define LOCAL static

/* essentially - verify alignment on byte boundaries at compile time */

#define CHECK(tmpname,cond)			\
	struct tmpname {			\
		char fail1 [(cond) ? 1 : 0];	\
		char fail2 [(cond) ? 1 : -1]; };

/*----------------------------------------------------------------------*/

#if _MAX_PATH < 260
# define SYS_MAXPATH	260u
#else
# define SYS_MAXPATH	_MAX_PATH
#endif

#define SEC_SZ		512u

/* Last valid cluster number */

#define FAT12_LAST	0x0FF5 /*  4085u */
#define FAT16_LAST	0xFFF5 /* 65525u */

/* Min sectors per FAT */

#define FAT16_MIN	((unsigned)(((FAT12_LAST+1ul) * 2u + SEC_SZ-1) / SEC_SZ))
#define FAT32_MIN	((unsigned)(((FAT16_LAST+1ul) * 4u + SEC_SZ-1) / SEC_SZ))

#define DIRENT_SZ	32u

/*----------------------------------------------------------------------*/

typedef const char	CStr[], *PCStr;
typedef char		Str[], *PStr;

typedef unsigned long	ulong;

typedef unsigned char	byte;	CHECK (VerifyBSize, sizeof (byte) == 1);
typedef unsigned short	word;	CHECK (VerifyWSize, sizeof (word) == 2);
typedef unsigned long	dword;	CHECK (VerifyDSize, sizeof (dword) == 4);

typedef byte		UBYTE;
typedef word		UWORD;
typedef dword		UDWORD;

/*----------------------------------------------------------------------*/

/* BPB: BIOS Parameter Block.						*/
/* Can be obtained via INT 21/440D/CX=0860.				*/

typedef struct {
  word	sector_bytes;	/* 00 bytes per sector (usually 512)		*/
  byte	cluster_sectors;/* 02 sectors per cluster			*/
  word	FAT_start;	/* 03 first sector of first FAT (also		*/
			/*    # "reserved" sectors, usually 1)		*/
  byte	FATs;		/* 05 # FATs (usually 2)			*/
  word	root_entries,	/* 06 entries per root directory (0 for FAT32)	*/
	total_sectors;	/* 08 volume size (if 0, use xTotal_sectors)	*/
  byte	media_ID;	/* 0A media descriptor				*/
  word	FAT_sectors,	/* 0B sectors per FAT (0 for FAT32)		*/
	track_sectors,	/* 0D sectors per track				*/
	heads;		/* 0F */
  dword	hidden_sectors,	/* 11 */
	xTotal_sectors;	/* 15 volume size if total_sectors=0		*/
} BPB;			CHECK (VerifyBPBsize, sizeof (BPB) == 0x19);

/* Extended BPB for FAT32.						*/
/* Can be obtained via INT 21/440D/CX=4860.				*/

typedef struct BPB32 :BPB {
  dword	xFAT_sectors;	/* 19 sectors per FAT if FAT_sectors=0		*/
  word	mirror,		/* 1D mirroring flags				*/
	version;	/* 1F FS version (high byte=major, low=minor)	*/
  dword	root_cluster;	/* 21 first cluster of root dir			*/
  word	info_sector,	/* 25 # info sector (-1 for none)		*/
	backup_sector;	/* 27 # backup boot sector (-1 for none)	*/
} BPB32;		CHECK (VerifyBPB32size, sizeof (BPB32) == 0x29);

/*----------------------------------------------------------------------*/

typedef struct {
  byte	drive_no;	/* 00 */
  byte	reserved1;	/* 01 */
  byte	signature;	/* 02 =0x29					*/
  dword	serial_no;	/* 03 */
  char	label [11];	/* 07 */
  char	FS_type [8];	/* 12 ="FAT12   ", "FAT16   " or "FAT32   "	*/
} BOOTSECINFO;		CHECK (VerifyBSIsize, sizeof (BOOTSECINFO) == 0x1A);

typedef union {
    byte	dump [SEC_SZ];
  struct {
    byte	jmp_opcode [3];	/* 00 */
    char	OEM_name [8];	/* 03 */
    BPB		bpb;		/* 0B */
    BOOTSECINFO	i;		/* 24 */
  } _;
  struct {
    byte	jmp_opcode [3];	/* 00 */
    char	OEM_name [8];	/* 03 */
    BPB32	bpb;		/* 0B */
    byte	reserved [12];	/* 34 */
    BOOTSECINFO	i;		/* 40 */
  } _32;
} bootsector_t;		CHECK (VerifyBSsize,
				offsetof (bootsector_t, _  .i) == 0x24 &&
				offsetof (bootsector_t, _32.i) == 0x40);

/*----------------------------------------------------------------------*/

/* Fast ASCII tolower/toupper */

#define _upper(ch)	((byte)(~0x20 & (ch)))
#define _doupper(var)	((var) &= ~0x20)
