#!/bin/sh
source ./setenv.sh
WINNAME="cygwin"
if [ "$OSTYPE" = "$WINNAME" ]
then `OS=""; TMP=""; TEMP=""; cygstart "${MAYA_DIR}\\bin\\maya.exe"`
else `open $MAYA_DIR/Maya.app`
fi