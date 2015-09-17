VMSMOUNT - A DOS redirector for mounting VMware's Shared Folders
(C) 2011 Eduardo Casino <eduardo.casino@gmail.com>

WARNING AND DISCLAIMER

 This should be considered a beta version and, as such, may contain
 bugs that could cause data loss. THIS PROGRAM IS PROVIDED "AS IS" WITHOUT
 WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED
 TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE.  THE ENTIRE RISK ASTO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS
 WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL
 NECESSARY SERVICING, REPAIR OR CORRECTION.
 
USAGE

 VMSMOUNT [/H][/V|/Q|/QQ] [/L:<drv>] [/B:<siz[K]>] [/LFN [/M:<n>] [/CI|/CS]]
 VMSMOUNT [/V|/Q|/QQ] /U
	/H                 - Prints help and exits
	/V                 - Verbose: Prints information on system resources
	/Q                 - Quiet: Omits copyright message
	/QQ                - Silent: Does not print any messages at all
	/L:<drive letter>  - Drive letter to assign
	                     (if omitted, use the first available)
	/B:<size[K]>       - Size of read/write buffer
	                     (4K default, higher values increase performance)
	/LFN               - Long File Name support. "Mangles" long file names
	                     (or those with illegal or unconvertibe characters)
	                     to valid 8.3 names, using a hash algorithm. For
	                     example, "This is a long file.name" will translate
	                     into "THIS~2BF.NAM"
	/M:<n>             - Number of mangling chars for short names
	                     (2 minimum, 6 maximum, 3 default) For example, the
	                     same "This is a long file.name" will translate into
	                     "THISI~02.NAM" if /M:2 or "TH~0BAC0.NAM" if /M:5.
	                     The default suits most use cases. Increase if the
	                     host file system has many files with similar long
	                     names.
	/CI                - Host file system is case insensitive, so
	                     "example.txt" and "ExaMPLe.Txt" are the same. This is
	                     the default behaviour. 
	/CS                - Host file system is case sensitive (non-Windows
	                     hosts) Mangles file names whith lower case chars. For
	                     example, "EXAMPLE.TXT" will be left unchanged, but
	                     "Example.txt" will be translated into "EXAM~4F0.TXT"
	/U                 - Uninstall

ENVIRONMENT

	TZ      - Valid POSIX timezone. If omitted, file times will be in UTC
	          (see http://www.gnu.org/s/hello/manual/libc/TZ-Variable.html)
	          Example: TZ=CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
	LANG    - Used by the Kitten library to show messages in the correct
	          language. Currently only available in English, Spanish and
	          Dutch.
	NLSPATH - Used by the Kitten library to find the message catalogs
	          (VMSMOUNT.EN, VMSMOUNT.ES, VMSMOUNT.NL, ...)
	PATH    - VMSMOUNT searchs in the PATH for the unicode conversion
	          tables.
	
	VMSMOUNT gets the current NLS settings from the kernel and translates
	VMware's UTF encoding to the correct code page, provided that the
	necessary conversion table is found.  It uses the same table format
	as Volkov Commander and DOSLFN. Please refer to TBL.TXT in the DOSLFN
	source package for details(http://adoxa.110mb.com/doslfn/index.html)
	For convenience, I'm distributing with this package the translation
	tables, which can be generated from the ASCII code tables provided at
	www.unicode.org using the MK_TABLE program from DOSLFN

RETURN CODES (ERRORLEVELS)

	If loaded successfully, VMSMOUNT returns the number of the assigned drive
	letter starting with 1 ( A == 1, B == 2, C == 3, ... )
	
	If not loaded, errorlevel is set according to the following table:
	
	ERRORLEVEL  Meaning
	~~~~~~~~~~  ~~~~~~~
	    0       Not loaded (help screen requested) or successfully uninstalled
	  245       Unable to uninstall
	  246       Driver not installed and tried to uninstall
	  247       Invalid buffer size
	  248       Invalid command line option(s)
	  249       Unsupported DOS version
	  250       Not running in a virtual machine
	  251       Shared folders not enabled
	  252       Redirector not allowed to install
	  253       Already installed
	  254       Invalid drive letter
	  255       Other system error
 
LIMITATIONS

 * Does not work with DOS < 5 (Tested with latest FreeDOS kernel,
   MS-DOS 6.22 and MS-DOS 7 (win95)
 * Does not support old versions of VMWare Workstation (Tested with
   VMWare Player 3 and 4) 
 * Shared folder names MUST be uppercase if /LFN is not set
 * Does not detect code page changes

ACKNOWLEDGEMENTS

 * "Undocumented DOS 2nd ed." by Andrew Schulman et al.
   Enlightening, but with some inaccuracies that have driven me mad
   
 * Ken Kato's VMBack info and Command Line Tools
                    (http://sites.google.com/site/chitchatvmback/)
   The HGFS code is fully based on his excellent work
   
 * VMware's Open Virtual Machine Tools
                    (http://open-vm-tools.sourceforge.net/)
   Info on the HGFS V3 protocol
   
 * Tomi Tilli's <aitotat@gmail.com> TSR example in Watcom C for the 
                    Vintage Computer Forum (www.vintage-computer.com)
   The idea and some code for a lightweight TSR written in C
 
 * Jason Hood/Henrik Haftmann for the unicode translation tables in
   DOSLFN. Jason again for SHSUCDX, it's source code has been very
   helpful.
 
 * Bernd Blaauw for his incredibly useful feedback and Dutch translation
 
 * Tom Ehlert for extensive feedback, code review, bug reports, ideas and
   actual code
 
 * And, of course, Pat Villani for the FreeDOS kernel.
   

BUILD

  VMSMOUNT was built with OpenWatcom 1.9, NASM 2.09.10 and GNU Make 3.81
  
BUGS

 * When used with DOSLFN in FreeDOS, "." and ".." appear corrupted
   in the long name listing with DIR /LFN. I don't know if it is a
   FreeDOS kernel, FreeCOM or DOSLFN bug. I assume it is not VMSMOUNT
   because it also occurs with SHSUCDX. It does not happen with MS-DOS 7.

TODO
 
 * Code Page change detection (maybe)
 * Full Long file names support
 
LICENSE

  VMSMOUNT is GPLd software. See LICENSE.TXT for the details.
