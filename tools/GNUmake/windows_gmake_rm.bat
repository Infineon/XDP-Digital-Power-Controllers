@echo off

Setlocal EnableDelayedExpansion

:: Replace forward slashes with backslashes
set param=%1
set file=!param:/=\!

:: Remove directory including subdirectories and do it quietly (no confirmation)
if exist !file! (
    del /F /Q !file!
)