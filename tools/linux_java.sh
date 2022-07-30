#/bin/bash

cd ../swig
./linux_java.sh

cd ../tools/
mkdir -p linux_java
cd linux_java
cp -r ../../swig/com/ .
cp ../../swig/*.cs .
cp ../../swig/*.so .
cd ..
rm -f linux_java.zip
zip linux_java.zip linux_java