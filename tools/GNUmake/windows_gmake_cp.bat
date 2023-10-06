@echo off
rem This batch file copies one or multiple files to a a given destination.
rem The last call argument is taken as copy destination location,
rem all other call arguments are taken as input file list.
rem All files of the list are individually copies to the destination location.

setlocal

rem The script MUST be called with at least two arguments: 1) source & 2) destination
if not "%2" == "" goto doit

:error

echo windows_gmake_cp.bat:  error:  expected at least 2 parameters
exit /b

:doit

rem Find the last given argument in the list, because this is the destination
set lastarg=0
for %%x in (%*) do (
   set lastarg=%%~x
)

rem Copy each source file to the destination. Suppress output and overwrite existing files without promptin (/Y)
for %%x in (%*) do (
   if not %lastarg% == %%~x (
      copy /Y %%~x %lastarg% > nul
   )
)
