#!/bin/bash

mkdir -p com/hb3344/ademco
swig -java -package com.hb3344.ademco -outdir com/hb3344/ademco -o libademco_java_wrap.c libademco.swig
JAVA_INCLUDE=${JAVA_HOME}/include
echo "JAVA_INCLUDE: ${JAVA_INCLUDE}"
JAVA_INCLUDE_OS=${JAVA_INCLUDE}/linux
echo "JAVA_INCLUDE_OS: ${JAVA_INCLUDE_OS}"
cc -std=c99 -fpic -c ../src/libademco/ademco.c ../src/libhb/hb_core.c ../src/libhb/hb_core_ademco.c ../src/libhbiot/rf/hb_rf.c libademco_java_wrap.c -I"../src" -I${JAVA_INCLUDE} -I${JAVA_INCLUDE_OS}
ld -shared ademco.o hb_core.o hb_core_ademco.o hb_rf.o libademco_java_wrap.o -o libademco_java.so
javac JavaDemo.java
# java -Djava.library.path=./ JavaDemo