#/bin/bash

mkdir -p com/hb3344/ademco
swig -java -package com.hb3344.ademco -outdir com/hb3344/ademco -o libademco_java_wrap.c libademco.swig
JAVA_INCLUDE=/Library/Java/JavaVirtualMachines/jdk-9.0.1.jdk/Contents/Home/include
JAVA_INCLUDE_OS=${JAVA_INCLUDE}/darwin
cc -fpic -c ../ademco.c libademco_java_wrap.c -I"../" -I${JAVA_INCLUDE} -I${JAVA_INCLUDE_OS}
ld -shared ademco.o libademco_java_wrap.o -o libademco.so
javac JavaDemo.java
java -Djava.library.path=./ JavaDemo