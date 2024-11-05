REM Set path to fxc
PATH=%CD%\..\Python26;%PATH%;%DXSDK_DIR%\Utilities\bin\x86;

REM Set path to pyengine worspace
REM You need to modify this line when you install the engine
set PYENGINE_WORKSPACE_DIR=%CD%\..
echo %PYENGINE_WORKSPACE_DIR%

REM Set path to python scripts for maya
REM Add more folders for different scripts
set PYTHONPATH=%PYENGINE_WORKSPACE_DIR%\Tools\XParser

REM Set path to python scripts for maya
REM Add more folders for different scripts
set MAYA_SCRIPT_PATH=%MAYA_SCRIPT_PATH%;%PYENGINE_WORKSPACE_DIR%\Tools\LevelTools

set MAYA_SHELF_PATH=%MAYA_SHELF_PATH%;%PYENGINE_WORKSPACE_DIR%\Tools\XParser

set OLDPATH=%CD%

CD /D %OLDPATH%

IF NOT DEFINED MAYA_DIR set MAYA_DIR=C:\Program Files\Autodesk\Maya2011
