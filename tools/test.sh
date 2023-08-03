#!/bin/bash

# win-node
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
