#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

cd $CURRENT_DIR
./clean.sh
mkdir -p dist/win_node
./win_node.bat
cp -r ../swig/build/ dist/win_node/
cp ../swig/nodejs-demo.js dist/win_node/
rm -f dist/win_node.zip
cd dist/win_node
zip -r ../win_node.zip . *
cd ..
rm -rf win_node/
