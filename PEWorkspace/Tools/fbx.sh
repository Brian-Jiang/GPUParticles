#!/bin/bash
source ./setenv.sh
#this makes sure aliases from setenv can be used
shopt -s expand_aliases

pepython -i FBX/ImportScene.py "$@"

