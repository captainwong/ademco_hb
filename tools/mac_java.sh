#/bin/bash

cd ../swig
./mac_java.sh

cd ../tools/
mkdir -p mac_java
cd mac_java
cp -r ../../swig/com/ .
cp ../../swig/*.cs .
cp ../../swig/*.dylib .
cd ..
rm -f mac_java.zip
zip mac_java.zip mac_java/
