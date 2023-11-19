#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

cd $CURRENT_DIR
./clean.sh
./win_csharp.bat
mkdir -p dist/win_csharp
cp -r ../swig/com/ dist/win_csharp/
cp ../swig/*.cs dist/win_csharp/
cp ../swig/*.dll dist/win_csharp/
cp ../swig/*.exe dist/win_csharp/
rm -f dist/win_csharp.zip
cd dist/win_csharp
zip -r ../win_csharp.zip . *
cd ..
rm -rf win_csharp
