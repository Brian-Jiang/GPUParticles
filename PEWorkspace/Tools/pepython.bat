set OLDPATH=%CD%

CD /D %OLDPATH%

@echo on
"%PYENGINE_WORKSPACE_DIR%\Python26\python.exe" %1 %2 %3 %4 %5 %6 %7 %8 %9
