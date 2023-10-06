@echo off

Setlocal EnableDelayedExpansion

:: Replace forward slashes with backslashes
set param=%1
set dir=!param:/=\!

:: Make the directory
if not exist !dir! (
    mkdir !dir!
)

