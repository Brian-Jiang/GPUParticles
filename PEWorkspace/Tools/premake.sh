#!/bin/bash
source ./setenv.sh
#this makes sure aliases from setenv can be used
shopt -s expand_aliases
current_dir=$(pwd)
echo "Current dir: $current_dir"
echo "Changing to Code"
cd ../Code
premake --scripts=premake-scripts "$@"
echo "Current dir: $(pwd)"
echo "Changing back"
cd $current_dir
echo "Current dir: $(pwd)"

