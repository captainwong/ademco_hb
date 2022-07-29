#/bin/bash

mkdir -p com/hb3344/ademco
swig -java -package com.hb3344.ademco -outdir com/hb3344/ademco -o libademco_java_wrap.c libademco.swig
gcc -fpic -c ../ademco.c libademco_java_wrap.c -I"../" -I"%JAVA_HOME%include" -I"%JAVA_HOME%include/win32"
gcc -shared ademco.o libademco_java_wrap.o -o libademco.so
