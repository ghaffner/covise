@ECHO OFF
REM this script sets the COVISE env and then starts COVISE remote daemon
SETLOCAL

REM get the 8.3 filename of current installation
SET TMPFILE="%TMP%\~covtmpfile.txt"
CALL "%WINDIR%\system32\cscript.exe" /nologo "%~dp0get8dot3path.vbs" "%~dp0">%TMPFILE%
SET /P INSTDIR=<%TMPFILE%
DEL /Q %TMPFILE%

REM note: ARCHSUFFIX has to be set in common.local.bat
CALL %INSTDIR%\common.VISENSO.bat NONE %INSTDIR%
START "COVISE remote daemon" /min %INSTDIR%\%ARCHSUFFIX%\bin\RemoteDaemon.exe

ENDLOCAL