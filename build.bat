@echo off

REM Compile Here

SETLOCAL
set MODE=%1

set bin_int=.\bin-int\debug

IF NOT EXIST %bin_int% mkdir %bin_int%

set outputs=/Fp"%bin_int%\hd.pch" /Fo%bin_int%\ /Fd"%bin_int%\vc142.pdb"

set common_flags=/nologo  /diagnostics:caret /D _UNICODE /D UNICODE 
rem /GF
set debug_flags=/Z7 /JMC /W3 /WX- /Od /TP /FC /Gm- /EHsc /RTC1 /MTd /GS /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /std:c++17
set release_flags=/Ox

set debug_link_option=/DEBUG:FULL /NOLOGO


set flags=%common_flags% %debug_flags% 
IF /I "%1"=="release" ( set flags=%common_flags% %release_flags% ) 


set INCLUDE_PATHS=/I./source/meow_hash /I./source/
set source_files=.\source\base.cpp .\source\main.cpp
set object_files=.\base.obj .\main.obj

rem  /D _UNICODE /D UNICODE /Yu"pch.h"


REM Compile Base 
cl /c %INCLUDE_PATHS% %flags% .\source\base.cpp %outputs%

REM Compile And Link Meta
cl /c %INCLUDE_PATHS% %flags% .\source\meta.cpp %outputs%
IF %ERRORLEVEL% NEQ 0 (goto FINISH)
pushd %bin_int%
link .\base.obj .\meta.obj  /OUT:..\..\bin\meta.exe %debug_link_option% 
IF %ERRORLEVEL% NEQ 0 (goto FINISH)
popd
pushd .\source\
..\bin\meta.exe
popd


REM compile main
cl /c %INCLUDE_PATHS% %flags% .\source\main.cpp %outputs%

REM LINK 
pushd %bin_int%
link %object_files% /OUT:..\..\bin\hd.exe %debug_link_option%
popd

:FINISH

ENDLOCAL