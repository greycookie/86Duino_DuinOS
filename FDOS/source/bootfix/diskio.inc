typedef enum { IO_READ = 0, IO_WRITE = 1 } IO_DIR;

static struct {
	dword sector;
	word sectors;
	void far *buffer;
} diskIOpacket;

int PROC_REG FAT_IO (byte drive, void *diskIOpacket, int dir);

#define diskIO(drive,sec,secs,buf,dir)			\
	(diskIOpacket.sector  = (sec),			\
	 diskIOpacket.sectors = (secs),			\
	 diskIOpacket.buffer  = (buf),			\
	 FAT_IO (drive, &diskIOpacket, dir))

/*----------------------------------------------------------------------*/

#ifdef __WATCOMC__

int PROC_REG FAT_IO_ (byte drive, void *diskIOpacket, int dir);

# if defined __TINY__ || defined __SMALL__ || defined __MEDIUM__

#  pragma aux FAT_IO parm [dx] [bx] [si];
#  pragma aux FAT_IO_ =					\
	"inc dx"	/* try new FAT32 disk IO */	\
	"mov cx,-1"					\
	"mov ax,0x7305"					\
	"stc"						\
	"int 0x21"					\
	"jnc doneIO"					\
	"xchg ax,dx"	/* try old INT25/26 disk IO */	\
	" dec ax"					\
	/*"mov cx,-1"*/					\
	"push bp"					\
	"test si,si"					\
	"jnz int26"					\
	"int 0x25"					\
	"jmp short done2x"				\
"int26:	 int 0x26"					\
"done2x: pop ax"					\
	"pop bp"					\
"doneIO: sbb ax,ax"					\
	parm [dl] [bx] [si]				\
	modify exact [ax cx dx bx si di]		\
	value [ax];

# else /* defined __COMPACT__ || defined __LARGE__ || defined __HUGE__ */

#  pragma aux FAT_IO parm [dx] [ds bx] [si];
#  pragma aux FAT_IO_ =					\
	"inc dx"	/* try new FAT32 disk IO */	\
	"mov cx,-1"					\
	"mov ax,0x7305"					\
	"stc"						\
	"int 0x21"					\
	"jnc doneIO"					\
	"xchg ax,dx"	/* try old INT25/26 disk IO */	\
	" dec ax"					\
	/*"mov cx,-1"*/					\
	"push bp"					\
	"test si,si"					\
	"jnz int26"					\
	"int 0x25"					\
	"jmp short done2x"				\
"int26:	 int 0x26"					\
"done2x: pop ax"					\
	"pop bp"					\
"doneIO: sbb ax,ax"					\
	parm [dl] [ds bx] [si]				\
	modify exact [ax cx dx bx si di]		\
	value [ax];

# endif

#endif

int PROC_REG FAT_IO (byte drive, void *diskIOpacket, int dir) {
#ifdef __WATCOMC__
	return FAT_IO_ (drive, diskIOpacket, dir);
#else
	drive++,
	r.h.dl = drive,
	r.x.bx = FP_OFF (diskIOpacket),
	r.x.si = dir,
	r.x.cx = -1,
	r.x.ax = 0x7305;
	/* now set carry flag or compare AX with 7300 later */
# if defined __TINY__ || defined __SMALL__ || defined __MEDIUM__
	if (int86 (0x21, &r, &r) != 0x7300 && r.x.cflag == 0)
		return r.x.cflag;
	r.x.ax = r.x.dx - 1u,
	/*r.x.bx = FP_OFF (diskIOpacket),*/
	/*r.x.cx = -1,*/
	int86 (0x25 + dir, &r, &r);
# else /* defined __COMPACT__ || defined __LARGE__ || defined __HUGE__ */
	segr.ds = FP_SEG (diskIOpacket);
	if (int86x (0x21, &r, &r, &segr) != 0x7300 && r.x.cflag == 0)
		return r.x.cflag;
	r.x.ax = r.x.dx - 1u,
	/*segr.ds = FP_SEG (diskIOpacket),*/
	/*r.x.bx = FP_OFF (diskIOpacket),*/
	/*r.x.cx = -1,*/
	int86x (0x25 + dir, &r, &r, &segr);
# endif
	return r.x.cflag;
#endif
} /* FAT_IO */
