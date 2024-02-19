pushd %~dp0

GenProcs.exe --path=../../ServerExample/GameDB.xml --output=GenProcedures.h

IF ERRORLEVEL 1 PAUSE

XCOPY /Y GenProcedures.h "../../ServerExample"

DEL /Q /F *.h

PAUSE