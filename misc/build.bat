@echo off

MSBuild.exe Generated/HDLang.vcxproj -p:Configuration=%1
REM  -clp:ShowCommandLine;ErrorsOnly;WarningsOnly
