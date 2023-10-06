@echo off

echo prescript path %PATH%
:: Determine java install dir and save into JAVAHOME for later
for /f %%i in ('where java') do (
    ::set JAVAHOME=%%~dpi
    goto :javaset
)
:javaset

::set JAVAHOME="C:\Program Files (x86)\Common Files\Oracle\Java\javapath"

echo Found Java Home: %JAVAHOME%

:: User should use this to setup their local env, just edit paths to tool installs
CALL user_env.bat

echo git install %GIT_INSTALL_DIR_BIN%

:: start the command shell, set the CMD window title to tell the user that this shell is specially 
:: made for building the Shasta Firmware, run 'build_env.bat' to create the actual environment.
CALL .\tools\scripts\build_env.bat

echo post script path %PATH%

@PATH=%PATH%;%JAVAHOME%;
echo java prefix path %PATH%

start eclipse.exe -vm %JAVAHOME% %*
::start eclipse.exe %*

