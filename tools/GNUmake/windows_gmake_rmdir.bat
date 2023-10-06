@echo off

Setlocal EnableDelayedExpansion

:: Replace forward slashes with backslashes
set param=%1
set dir=!param:/=\!

:: Remove directory including subdirectories and do it quietly (no confirmation)
if exist !dir! (
    rmdir /s /q !dir!
)