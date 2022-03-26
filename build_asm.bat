@echo off
call c:\NASM\nasm -fwin64 first.asm
call link first.obj /subsystem:console /entry:func.main kernel32.lib legacy_stdio_definitions.lib  msvcrt.lib

rem call ml64.exe first.asm /Zd /Zi /link /subsystem:console /defaultlib:kernel32.lib /defaultlib:user32.lib  /defaultlib:legacy_stdio_definitions.lib /defaultlib:msvcrt.lib /entry:Start	
