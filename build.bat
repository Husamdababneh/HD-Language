@echo off

REM Compile Here

SETLOCAL
set MODE=%1

set bin_int=.\bin-int\debug

IF NOT EXIST %bin_int% mkdir %bin_int%

set outputs=/Fp"%bin_int%\hd.pch" /Fo%bin_int%\ /Fd"%bin_int%\vc142.pdb"

set common_flags=/nologo  /diagnostics:caret /D _UNICODE /D UNICODE 
rem /GF
set debug_flags=/Zi /JMC /W3 /WX- /Od /TP /FC /Gm- /EHs /RTC1 /MTd /GS /fp:precise /Zc:wchar_t   /Zc:forScope /Zc:inline /std:c++17 /fsanitize=address


set clang_outputs=-o%bin_int%\main-clang.obj
set clang_common_flags=-diagnostics:caret -D _UNICODE -D UNICODE 
set clang_debug_flags= -Wno-everything -JMC -W4 -WX- -O0 -TP -FC -Gm--RTC1 -MTd -GS
rem -fsanitize=address
rem  -EHs -fp:precise -Zc:inline -Zc:wchar_t -Zc:forScope -Zi
rem /d1reportSingleClassLayoutAst_
rem /d1reportSingleClassLayoutToken
rem /d1reportAllClassLayout
rem /d1reportSingleClassLayout<name>

REM Once we get rid of all c/c++ headers, we will enable this to see the layout of the struct
rem /d1reportAllClassLayout
set release_flags=/Ox /std:c++17 

set debug_link_option=/DEBUG:FULL /NOLOGO


set flags=%common_flags% %debug_flags%
set clang_flags=%clang_common_flags% %clang_debug_flags% 
IF /I "%1"=="release" ( set flags=%common_flags% %release_flags% ) 

rem /I./source/meow_hash
set INCLUDE_PATHS=/I./source/
set source_files=.\source\main.cpp
set object_files=.\main.obj
set clang_object_files=.\main-clang.obj

set clang_INCLUDE_PATHS=-I./source/


REM compile main
cl /c  %INCLUDE_PATHS% %flags% .\source\main.cpp %outputs%
rem clang -c  %clang_INCLUDE_PATHS% %clang_flags% .\source\main.cpp %clang_outputs%
IF ERRORLEVEL 1 GOTO errorHandling

REM LINK 
pushd %bin_int%
link %object_files% /NATVIS:..\..\misc\strings.natvis  /OUT:..\..\bin\hd.exe %debug_link_option%
rem link %clang_object_files% msvcrt.lib  /NATVIS:..\..\misc\strings.natvis  /OUT:..\..\bin\hd-clang.exe %debug_link_option%
popd

:FINISH
ENDLOCAL
exit /b 0


:errorHandling
echo The build Failed
ENDLOCAL
exit /b 1

