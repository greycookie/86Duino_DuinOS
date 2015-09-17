/* bit 0 set if function to use current BPB, clear if Device
         BPB field contains new default BPB
   bit 1 must be clear if CL=60h
   bit 2 set if all sectors in track same size (should be set) (RBIL)
*/
typedef enum { DEFAULT = 4, CURRENT = 4+1 } GETBPB_FUNC;

int PROC_STK get_BPB (byte drive, GETBPB_FUNC func, void *pb);

/*----------------------------------------------------------------------*/

#ifdef __WATCOMC__

void PROC_REG reset_drive (byte drive);
# pragma aux reset_drive =				\
	"mov ah,0xD"					\
	"int 0x21"					\
	"push ds"					\
	"mov ah,0x32" /* update the DPB by reading the disk */ \
	"int 0x21"					\
	"pop ds"					\
	parm [dl]					\
	modify exact [ax bx];

int PROC_REG IOCTL_getdev (byte drive, unsigned code, void *data);
# if defined __TINY__ || defined __SMALL__ || defined __MEDIUM__
#  pragma aux IOCTL_getdev =				\
	"mov ax,0x440d"					\
	"int 0x21"					\
	"sbb ax,ax"					\
	parm [bl] [cx] [dx]				\
	modify exact [ax]				\
	value [ax];
# else /* defined __COMPACT__ || defined __LARGE__ || defined __HUGE__ */
#  pragma aux IOCTL_getdev =				\
	"mov ax,0x440d"					\
	"int 0x21"					\
	"sbb ax,ax"					\
	parm [bl] [cx] [ds dx]				\
	modify exact [ax]				\
	value [ax];
# endif

#else

# define reset_drive(drive) (void)(			\
	r.h.dl = drive,					\
	r.h.ah = 0xD,  int86 (0x21, &r, &r),		\
	r.h.ah = 0x32, int86 (0x21, &r, &r))

#endif

/*----------------------------------------------------------------------*/

int PROC_STK get_BPB (byte drive, GETBPB_FUNC func, void *pb) {
	static struct {
		byte	func;
		byte	reserved1 [6];
		BPB32	bpb;
		byte	reserved2 [0x5C - sizeof (BPB32) - 7];
	} IOCTL_data;

	drive++;
	IOCTL_data.func = func;
	reset_drive (drive);
#ifndef __WATCOMC__
	r.h.bl = drive,
	r.x.dx = FP_OFF (&IOCTL_data);
# if defined __TINY__ || defined __SMALL__ || defined __MEDIUM__
#  define IOCTL_getdev(drive,code,data)		\
	(r.x.cx = code, r.x.ax = 0x440d,	\
	int86 (0x21, &r, &r), r.x.cflag)
# else /* defined __COMPACT__ || defined __LARGE__ || defined __HUGE__ */
#  define IOCTL_getdev(drive,code,data)		\
	(r.x.cx = code, r.x.ax = 0x440d,	\
	int86x (0x21, &r, &r, &segr), r.x.cflag)
	segr.ds = FP_SEG (&IOCTL_data);
# endif
#endif
	enum { FAT1xBPB = 0x860, FAT32BPB = 0x4860 };
	if (IOCTL_getdev (drive, FAT32BPB, &IOCTL_data) &&
	   (IOCTL_getdev (drive, FAT1xBPB, &IOCTL_data) ||
	    IOCTL_data.bpb.FAT_sectors == 0))
		return 1;
	memcpy (pb, &IOCTL_data.bpb, IOCTL_data.bpb.FAT_sectors
					? sizeof (BPB) : sizeof (BPB32));
	return 0;
}
