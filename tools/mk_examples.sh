#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

./clean.sh

./examples.bat
mkdir -p dist/examples/x86
cp "/h/dev/AlarmCenterTestTools/Release/直连型主机模拟器V1.15.exe" dist/examples/x86/
cp "/g/dev_libs/curl-7.87.0/builds/libcurl-vc14-x86-release-dll-ipv6-sspi-schannel/bin/libcurl.dll" dist/examples/x86/
cp ../examples/Win32/Release/server_demo.exe dist/examples/x86/
cp ../examples/Release/httprelay.exe dist/examples/x86/
cp ../examples/Release/ademco.dll dist/examples/x86/
cp ../examples/Release/ademco.lib dist/examples/x86/

./examples_x64.bat
mkdir -p dist/examples/x64
cp ../examples/x64/Release/server_demo_x64.exe dist/examples/x64/
cp ../examples/x64/Release/ademco.dll dist/examples/x64/
cp ../examples/x64/Release/ademco.lib dist/examples/x64/
cp ../examples/x64/Release/httprelay.exe dist/examples/x64/
cp "/g/dev_libs/curl-7.87.0/builds/libcurl-vc14-x64-release-dll-ipv6-sspi-schannel/bin/libcurl.dll" dist/examples/x64/

rm -f dist/examples.zip
cd dist/examples
zip -r ../examples.zip . *
cd ..
rm -rf examples
