@echo off
rem Assuming PostINST checks for wattcp.cfg file and adds variable to autoexec.bat
rem Whatever happened to DNS servers? Not seeing any listed/asked
rem Google has generic 8.8.8.8, right?
REM echo SET WATTCP.CFG=%%DOSDIR%%>>%autofile%
cls
echo.
echo        Wattcp.cfg Setup Menu
echo 1) Use default DHCP for wattcp.cfg (might hang system)
echo 2) Set up wattcp.cfg according to user input (recommended if not using DHCP)
echo 3) Edit wattcp.cfg manually
echo 4) Don't do anything right now.
echo.
choice /n /t1,10 /c:1234
if "%errorlevel%"=="1" goto end
if "%errorlevel%"=="3" goto editwattcp
if "%errorlevel%"=="4" goto end
echo What is your IP (DHCP for DHCP configurations)?
set /P ip=
if /I "%ip%"=="DHCP" goto end
if /I "%ip%"=="" set ip=DHCP
echo my_ip = %ip% > %dosdir%\wattcp.cfg
echo What is your netmask (default is 255.255.255.0)?
set /P ip=
if "%ip%"=="" set ip=255.255.255.0
echo netmask = %ip% >> %dosdir%\wattcp.cfg
echo What is your domain?
set /P ip=
if "%ip%"=="" set ip=your.domain.com
echo domain_list = %ip% >> %dosdir%\wattcp.cfg
echo What is your gateway? (optional)
set /P ip=
if not "%ip%"=="" echo gateway = %ip% >> %dosdir%\wattcp.cfg
goto end
:editwattcp
choice /c:yn View a sample first
if "%errorlevel%"=="1" edit %dosdir%\bin\wattcp.sam
edit %dosdir%\wattcp.cfg
:end
