#!/bin/bash

mkdir -p build

JAVADEMO_SRC='./javademo/cpp'
JAVA_INCLUDE='/usr/lib/jvm/java-8-openjdk-amd64/include'

javac -encoding utf-8 javademo/jni/AdemcoHbLibrary.java && javah -d javademo/cpp -jni javademo.jni.AdemcoHbLibrary
g++ -std=c++11 $JAVADEMO_SRC/pch.cpp $JAVADEMO_SRC/dllmain.cpp -I$JAVADEMO_SRC -Iinclude -I$JAVA_INCLUDE -I$JAVA_INCLUDE/linux -fPIC -shared -o build/libademcojava.so
javac -encoding utf-8 javademo/jni/SimpleServer.java
java javademo.jni.SimpleServer 12345
