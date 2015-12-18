@echo off

C:\TOOLS\CE_INIT.exe %1

IF "%1" == "ONE_VGA" goto VGA
IF "%1" == "ONE" goto NVGA
IF "%1" == "ZERO" goto NVGA
IF "%1" == "EDUCAKE" goto NVGA
goto ERROR

:VGA
	C:\FDOS\BIN\SRDISK /Y 0
	LOADCEPC /l:800x600x16 NK-VGA.BIN
	goto END

:NVGA
	C:\FDOS\BIN\SRDISK /Y 0
	LOADCEPC NK-NVGA.BIN
	goto END

:ERROR
	echo Need board information
	goto END

:END