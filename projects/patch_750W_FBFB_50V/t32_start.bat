@echo off
setlocal
rem python %~dp0\..\..\tools\scripts\ram_download.py 

rem tell us the path to the T32 bin dir, the actual name depends on
rem whether we sit in a Win7 64bit or Win7 32bit world
rem for the time being, use the default configuration file that comes with the installation
if exist C:\T32_uTrace\bin\windows64 (
    echo found T32 64bit version
    set T32_BIN_PATH=C:\T32_uTrace\bin\windows64
    set T32_CONF_FILE=C:\T32_uTrace\config.t32
) else if exist C:\T32_uTrace\bin\windows (
    echo found T32 32bit version
    set T32_BIN_PATH=C:\T32_uTrace\bin\windows
    set T32_CONF_FILE=C:\T32_uTrace\config.t32
) else if exist C:\Programs\T32\bin\windows (
    echo found T32 32bit version
    set T32_BIN_PATH=C:\Programs\T32\bin\windows
    set T32_CONF_FILE=C:\Programs\T32\config.t32
) else if exist C:\T32\bin\windows (
    echo found T32 32bit version
    set T32_BIN_PATH=C:\T32\bin\windows
    set T32_CONF_FILE=C:\T32\config.t32
) else if exist C:\T32\bin\windows64 (
    echo found T32 64bit version
    set T32_BIN_PATH=C:\T32\bin\windows64
    set T32_CONF_FILE=C:\T32\config.t32
) else (
    echo no Lauterbach installation found at expected locations
    echo Please update t32_start.bat script for our Lauterbach installation directory
    GOTO:EOF
)

rem start the ARM debugger, using a common target setup and a local FW setup file
start %T32_BIN_PATH%\t32marm.exe -c %T32_CONF_FILE% -s ..\..\tools\T32\Windows\setup_shasta.cmm setup_fw.cmm