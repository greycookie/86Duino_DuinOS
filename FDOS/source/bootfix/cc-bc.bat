@echo off
call bcc -mt -lt -N- -Z %1 %2 %3 %4 %5 %6 %7 %8 %9 bootfix.cpp
if exist bootfix.obj del bootfix.obj>nul
