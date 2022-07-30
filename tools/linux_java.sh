#!/bin/bash

mkdir -p linux_java
cd linux_java
cp -r ../../swig/com/ .
cp ../../swig/*.cs .
cp ../../swig/*.so .
cd ..
rm -f linux_java.zip
cd linux_java
zip -r ../linux_java.zip . *
cd ..
rm -rf linux_java/
