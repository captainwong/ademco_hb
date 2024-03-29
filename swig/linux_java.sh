#!/bin/bash

mkdir -p com/hb3344/ademco
swig -java -package com.hb3344.ademco -outdir com/hb3344/ademco -o libademco_java_wrap.c libademco.swig
JAVA_INCLUDE=${JAVA_HOME}/include
JAVA_INCLUDE_OS=${JAVA_INCLUDE}/linux
cc -std=c99 -fpic -c ../ademco.c ../hb_com.c libademco_java_wrap.c -I"../" -I${JAVA_INCLUDE} -I${JAVA_INCLUDE_OS}
ld -shared ademco.o hb_com.o libademco_java_wrap.o -o libademco_java.so
javac JavaDemo.java
# java -Djava.library.path=./ JavaDemo