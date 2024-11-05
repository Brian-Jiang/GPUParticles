#!/bin/sh
source ./setenv.sh
WINNAME="cygwin"
if [ "$OSTYPE" = "$WINNAME" ]
then `OS=""; TMP=""; TEMP=""; cygstart -v "$VSDIR/devenv.exe"`
else `open ../Code/PEWorkspace-ios.xcworkspace`
fi