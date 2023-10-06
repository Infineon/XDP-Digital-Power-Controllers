@echo off
rem  Origin: http://cplusplus.bordoon.com/GNU_MAKE_EXAMPLE/example.html, 21 Oct 2013

setlocal

if not "%:~2%" == "" goto doit

:error

echo windows_gmake_cp.bat:  error:  expected exactly 2 parameters
goto done

:doit
    
if not ("%3%4%5%6%8%9" == "") goto error

set input1=%1
set output1=%2

set input=%input1:/=\%
set output=%output1:/=\%

xcopy /q /i /e /y %input% %output%



:done
