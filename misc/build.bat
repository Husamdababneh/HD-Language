@echo off


MSBuild.exe Generated/HDLang.vcxproj -p:Configuration=%1  -clp:ShowCommandLine;ErrorsOnly;WarningsOnly
