:: configure a shell for doing build jobs
:: To make sure such build jobs run in a well-defined environment, we create a custom one ...

@echo off

:: check whether env vars are set by user above and warn otherwise
if not defined GIT_INSTALL_DIR_BIN (echo Pls set env variable GIT_INSTALL_DIR_BIN - it should point to the 'bin' directory of your git install, only required if git is used as rev control.)
if not defined RUBY_INSTALL_DIR_BIN (echo Pls set env variable RUBY_INSTALL_DIR_BIN - it should point to the 'bin' directory of your ruby install, only required for ceedling testing.)
if not defined GCC_INSTALL_DIR_BIN (echo Pls set env variable GCC_INSTALL_DIR_BIN - it should point to the 'bin' directory of your gcc install, only required for ceedling testing.)
if not defined PYTHON_INSTALL_DIR (echo Pls set env variable PYTHON_INSTALL_DIR - it should point to the directory of your python install with python.exe .)

:: set the path to all tools which are supposed to be used in the command shell
@PATH=%GIT_INSTALL_DIR_BIN%;%ECLIPSE_DIR%;%cd%\tools\GNUmake;%RUBY_INSTALL_DIR_BIN%;%GCC_INSTALL_DIR_BIN%;%PYTHON_INSTALL_DIR%;%cd%\tools\zip;%cd%\tools\scripts;%MIKTEX_DIR%\miktex\bin\x64\;C:\Windows\system32;

:: the git command line tools are looking into the environment variable 'HOME' to find the .ssh directory that it needs. The git command line
:: tools and their shell scripts do this, too ...
@if not exist "%HOME%" @set HOME=%HOMEDRIVE%%HOMEPATH%

:: Start python venv
cd tools\python\
CALL start_venv_for_windows.bat
cd ..\..

