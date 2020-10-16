@echo off

IF NOT EXIST .\bin-int mkdir bin-int
IF NOT EXIST .\bin mkdir bin

REM Clean
set CommonFlags=/Fe:..\bin\main.exe /std:c++17  -DWIN32=1
set ReleaseFlags=%CommonFlags% /MTd /Ob3x
set DebugFlags=%CommonFlags% /W2 /Z7 /Od /DEBUG:FULL /MDd /D_DEBUG

REM   Wild card  not sure if we really want this ?? 
set Files=../source/*.cpp		
set DLibs=
set RLibs=

REM the /I after the IF means to do case insensitive compare

[ -z %1 ] && goto Usage

if /I [%1] == [Debug] CALL :Compile "%DebugFlags%" "%DLibs%" & goto :eof
if /I [%1] == [Release] CALL :Compile "%ReleaseFlags%" "%RLibs%" & goto :eof
if /I [%1] == [Clean] goto Clean
if /I [%1] == [Usage] goto Usage

:Usage
echo Usage: Build.exe ^<Debug^|Release^|Clean^>
goto :eof

:Compile
pushd bin-int
REM This echo is important for emacs compilation mode
echo Entering directory `bin-int'
cl  %~1  %Files% %~2 /link /LIBPATH:"..\lib\"
REM This echo is important for emacs compilation mode
echo Leaving  directory `bin-int'
popd
EXIT %ERRORLEVEL%

:Clean
REM Todo
echo test
goto :eof

:eof
EXIT %ERRORLEVEL%
