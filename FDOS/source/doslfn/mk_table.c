#include <stdio.h>
#include <string.h>
	// A second command-line option (value ignored) is
	// to create GB instead of GBK from cp936.txt,
	// ignore all lead and trail bytes below 0xA1
	// ONLY GIVE WHEN COMPILING CP936.TXT!
	// Maybe, the same "shortcut" works at Korean and Taiwan files too.
/* This program converts ASCII code tables provided at www.unicode.org
 * to binary Unicode tables usable for (Volkov Commander and) DOSLFN.
 * These ASCII code tables are at
 * http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/
 *
 * To use this program, give the text file name, e.g.
 * mk_table cp737.txt
 * That will produce a file named "cp737uni.tbl".
 *
 * Compiled and linked with Borland C++ 3.1, but should work
 * with any compiler (however, target must have Intel byte order).
 *
 * New version with DBCS support. But without "disk full" check!
 * Format of DBCS .TBL file: see TBL.TXT
 * h#s 01/03
 */
typedef unsigned short wchar_t;	// Bug in BC - wchar_t is defined as a char!
char dbcs=0;			// DBCS flag
unsigned char minmaxtrail[2];	// minimum and maximum trail byte
wchar_t sb_uni[0x80];		// single-byte unicodes, or lead indices
wchar_t db_uni[0x80][0xC0];	// double-byte unicodes, size: 48K

int main(int argc,char**argv) {
 FILE *f;
 unsigned int i,db_index=0;
 unsigned int cp;
 char line[256],cpname[64];

 if (argc<2) {
  fputs("You must provide an input file name!\n",stderr);
  fputs("(Optionally any switch to ignore all DBCS lead and trail bytes below 0xA1)\n",
    stderr);
  return 3;
 }
 f=fopen(argv[1],"r");
 if (!f) {
  fprintf(stderr,"Cannot open input file %s!\n",argv[1]);
  return 3;
 }
 sscanf(argv[1],"%*[^0-9]%d",&cp);

 cpname[0]=0;

 memset(sb_uni,0xFF,sizeof(sb_uni));
 minmaxtrail[0]=0xFF; minmaxtrail[1]=0x00;
 memset(db_uni,0xFF,sizeof(db_uni));

 while (fgets(line,sizeof(line),f)) {
  wchar_t j,c;
  unsigned char lead,trail;
  unsigned int index;
  if (line[0]=='#') {
  /* for C learners, please look at that sscanf() template string,
   * and check out your documentation what "%*[^_]" does.
   * Therefore, a lesson about scanf should take two weeks(!!)
   * (including memory models), for printf another two weeks.
   */
   if (!cpname[0]) sscanf(line,"%*[^_]_DOS%s ",cpname);
   continue;
  }
  c=0xFFFF;		/* if there is an unused entry (e.g. Turkish) */
  if (sscanf(line,"%X %X",&j,&c)<1) continue;
  if (j<0x80) {
   if (c!=j) printf("Code 0x%02X is not ASCII, it's 0x%02X\n",j,c);
  }else if (j<0x100){
   if (sscanf(line,"%*[^#]#DBCS LEAD BYTE%c",&c)) {
    c=0;
    if (!dbcs) {
     puts("MESSAGE: Input file is DBCS");
     dbcs=1;
    }
   }
   sb_uni[j-0x80]=c;
  }else{
   if (!dbcs) {
    puts("MESSAGE: Input file is DBCS");
    dbcs=1;
   }
   lead=j>>8; trail=j&0xFF;
   if (lead<0x80) {
    printf("ERROR: Lead byte is 0x%02X, below 80h, cannot handle!\n",lead);
    continue;
   }
   if (trail<0x40) {
    printf("ERROR: trail byte is 0x%02X, below 40h, cannot handle!\n",trail);
    continue;
   }
   if (argc>=3) {
    if (lead <0xA1) continue;
    if (trail<0xA1) continue;
   }
   if (minmaxtrail[0]>trail) minmaxtrail[0]=trail;
   if (minmaxtrail[1]<trail) minmaxtrail[1]=trail;
   index=sb_uni[lead-0x80];	// should be 0 at first
   if (index==0xFFFF) {
    printf("MESSAGE: lead byte 0x%02X not declared, will use it automatically\n",lead);
    index=0;
   }else if (index>=0x80) {
    printf("ERROR: lead byte 0x%02X is already assigned to U+0x%04X!\n",lead,index);
    continue;
   }
   if (!index) {
    index=++db_index;
    sb_uni[lead-0x80]=index;
   }
   db_uni[index-1][trail-0x40]=c;
  }
 }
 fclose(f);

 if (dbcs) {
  printf("DBCS Summary: lead bytes used=%d, trail byte range=0x%02X..0x%02X\n",
    db_index,minmaxtrail[0],minmaxtrail[1]);
  minmaxtrail[1]-=minmaxtrail[0]-1;	// calculate length
 }
 if (!cpname[0]) {
  puts("Missing header (should contain _DOSlanguage)!");
  printf("Give a short name: "); scanf("%15s",cpname);
  if (!cpname[0]) return 1;
 }
 sprintf(line,"%05duni.tbl",cp);
 if (cp<10000) line[0]='c';
 if (cp<1000)  line[1]='p';
 f=fopen(line,"wb");
 if (!f) {
  printf("ERROR: Cannot create output file %s!\n",line);
  return 3;
 }
 fprintf(f,"Unicode (%s)\r\n%c",cpname,dbcs+1);
 if (dbcs) {
  fwrite(minmaxtrail,2,1,f);
 }
 fwrite(sb_uni,sizeof(sb_uni),1,f);
 if (dbcs) {
  for(i=0;i<db_index;i++) {
   fwrite(db_uni[i]+(minmaxtrail[0]-0x40),
     minmaxtrail[1],2,f);
  }
 }
 if (fclose(f)) {
  puts("ERROR: Cannot close file!");
  return 3;
 }
 return 0;
}
