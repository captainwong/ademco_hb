#!/bin/bash

mkdir -p build

JAVADEMO_SRC='./javademo/cpp'

# Ubuntu 16.04
JAVA_INCLUDE='/usr/lib/jvm/java-8-openjdk-amd64/include'
# Centos 7
#JAVA_INCLUDE='/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.252.b09-2.el7_8.x86_64/include'
# macOS Catalina
#JAVA_INCLUDE='/Library/Java/JavaVirtualMachines/jdk-9.0.1.jdk/Contents/Home/include'

# Ubuntu & Centos
JAVA_INCLUDE_OS=$JAVA_INCLUDE/linux
# macOS
#JAVA_INCLUDE_OS=$JAVA_INCLUDE/darwin

rm -f javademo/jin/*.class
# 编译Java类
javac -encoding utf-8 javademo/jni/AdemcoHbLibrary.java
# 生成C++头文件
javah -d javademo/cpp -jni javademo.jni.AdemcoHbLibrary
# 生成C++动态库，目标文件 ./build/libademcojava.so
g++ -std=c++11 $JAVADEMO_SRC/pch.cpp $JAVADEMO_SRC/dllmain.cpp -I$JAVADEMO_SRC -I../include -I$JAVA_INCLUDE -I$JAVA_INCLUDE_OS -fPIC -shared -o build/libademcojava.so
# 编译使用动态库的Java示例
javac -encoding utf-8 javademo/jni/SimpleServer.java
# 执行使用动态库的Java示例
java javademo.jni.SimpleServer 12345
