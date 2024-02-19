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


if exist %PYTHON_INSTALL_DIR%\lib\site-packages\openpyxl-2.5.1-py2.7.egg-info (
    rem file exists
	echo python appears to have required modules, skipping install
) else (
    rem file doesn't exist

:: If no proxy issues the user can use pip to install the packages with dependencies uncomment this and comment out everything else below this in the script
%PYTHON_INSTALL_DIR%\Scripts\pip install configparser==3.5.0
%PYTHON_INSTALL_DIR%\Scripts\pip install intelhex==1.5
%PYTHON_INSTALL_DIR%\Scripts\pip install openpyxl==2.5.1
%PYTHON_INSTALL_DIR%\Scripts\pip install pyelftools==0.23
%PYTHON_INSTALL_DIR%\Scripts\pip install xlrd==1.2.0
%PYTHON_INSTALL_DIR%\Scripts\pip install pandas==0.24.2

)
