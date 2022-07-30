#!/bin/bash

cd ../tools/
mkdir -p mac_java
cd mac_java
cp -r ../../swig/com/ .
cp ../../swig/*.cs .
cp ../../swig/*.dylib .
cd ..
rm -f mac_java.zip
cd mac_java
zip -r ../mac_java.zip . *
cd ..
rm -rf mac_java/
