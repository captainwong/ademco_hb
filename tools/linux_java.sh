#!/bin/bash

mkdir -p linux_java
cd linux_java
cp -r ../../swig/com .
rm -f com/hb3344/ademco/*.class
cp ../../swig/*.java .
cp ../../swig/*.so .
cd ..
rm -f linux_java.zip
cd linux_java
zip -r ../linux_java.zip . *
cd ..
rm -rf linux_java/
