@echo off
if not exist bootfix.obj call wpp @watcomc.cfg -ew-oai+-s bootfix.cpp %1 %2 %3 %4 %5 %6 %7 %8 %9
:if not exist bootfix.obj call wcc @watcomc.cfg -oai-s bootfix.c %1 %2 %3 %4 %5 %6 %7 %8 %9
if not errorlevel 1 if exist bootfix.obj call wlink @bootfix.lnk
