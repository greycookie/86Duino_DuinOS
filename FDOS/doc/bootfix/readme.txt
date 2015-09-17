Bootfix is a bootsector checking and fixing tool.
Parts of it may end up in SYS, for example to check if we 
can overwrite the previous bootsector without messing up
a previous booting operating system.
(for example, bootsector might point to a file. If that file does not exist
 on disk, we assume we may overwrite without backing up previous bootsector)

Author of this excellent piece of software is Arkady V Belousov.

Bernd Blaauw