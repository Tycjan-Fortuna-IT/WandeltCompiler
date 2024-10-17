@ECHO OFF

PUSHD $~dp0\..\

vendor\FancyBuildSystem\bin\windows\premake5.exe --file=Build.lua vs2022

POPD

PAUSE
