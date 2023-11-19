#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

cd $CURRENT_DIR
./clean.sh
mkdir -p win_node
./win_node.bat
cp -r ../swig/build/ win_node/
cp ../swig/nodejs-demo.js win_node/
rm -f win_node.zip
cd win_node
zip -r ../win_node.zip . *
cd ..
rm -rf win_node/
