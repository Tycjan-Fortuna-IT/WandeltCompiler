@ECHO OFF

REM
PUSHD %~dp0

REM
ECHO Deleting .sln files...
DEL /S /Q *.sln

REM
ECHO Deleting .vcxproj* files...
DEL /S /Q *.vcxproj*

REM
ECHO Deleting top-level bin directory...
IF EXIST bin (
    RMDIR /S /Q bin
)

REM
ECHO Deleting top-level bin-int directory...
IF EXIST bin-int (
    RMDIR /S /Q bin-int
)

REM
POPD

PAUSE
