@echo off

pushd source
python generator.py
popd


IF NOT EXIST .\bin-int mkdir bin-int
IF NOT EXIST .\bin mkdir bin

REM Clean

set IncludePaths=/I..\source\meow_hash
set LibPaths=/LIBPATH:"..\lib\"
set CommonFlags=/Fe:..\bin\main.exe /std:c++17   -D_WIN32=1
set ReleaseFlags=%CommonFlags% /MTd /Ob3x 
set DebugFlags=%CommonFlags% /W2 /Z7 /Od /DEBUG:FULL /MDd /D_DEBUG

REM   Wild card  not sure if we really want this ?? 
set Files=../source/*.cpp		
set DLibs=
set RLibs=



[ -z %1 ] && goto Usage
REM the /I after the IF means to do case insensitive compare
if /I [%1] == [Debug] CALL :Compile "%DebugFlags%" "%IncludePaths%" "%LibPaths%"  "%DLibs%" & goto :eof
if /I [%1] == [Release] CALL :Compile "%ReleaseFlags%" "%IncludePaths%" "%LibPaths%" "%RLibs%" & goto :eof
if /I [%1] == [Clean] goto Clean
if /I [%1] == [Usage] goto Usage

:Usage
echo Usage: Build.exe ^<Debug^|Release^|Clean^>
goto :eof

:Compile
pushd bin-int
REM This echo is important for emacs compilation mode
echo Entering directory `bin-int'
cl  %~1  %~2 %Files% %~4 /link  %~3
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
