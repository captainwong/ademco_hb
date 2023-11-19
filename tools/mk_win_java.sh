#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

cd $CURRENT_DIR
./clean.sh
./win_java.bat
mkdir -p dist/win_java
cp -r ../swig/com/ dist/win_java/
rm -f dist/win_java/com/hb3344/ademco/*.class
cp ../swig/*.java dist/win_java/
cp ../swig/*.dll dist/win_java/
rm -f dist/win_java.zip
cd dist/win_java
zip -r ../win_java.zip . *
cd ..
rm -rf win_java
