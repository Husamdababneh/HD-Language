@echo off

REM HDLang.vcxproj

MSBuild.exe Generated/HD-Project.sln -p:Configuration=%1

REM  -clp:ShowCommandLine;ErrorsOnly;WarningsOnly
