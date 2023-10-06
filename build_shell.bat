@echo off 

:: User should use this to setup their local env, just edit paths to tool installs
CALL user_env.bat

:: start the command shell, set the CMD window title to tell the user that this shell is specially 
:: made for building the Shasta Firmware, run 'build_env.bat' to create the actual environment.
start "Shasta Firmware Build Shell" .\tools\scripts\build_env.bat
