#!/bin/bash

mkdir -p build
cd build

cc=gcc
cxx=g++
if [ $(command -v clang++ &> /dev/null ; echo $?) -eq 0 ]
then
    cc=clang
    cxx=clang++
fi


${cxx} -std=c++11 -O3 ../server_demo/server_demo.cpp ../../src/libademco/ademco.c -I../../src -lpthread -o server_demo

${cc} -std=gnu99 ../../src/libademco/ademco.c \
    ../../src/libhb/hb_core.c \
    ../../src/libhb/hb_core_ademco.c \
    ../httprelay/cJSON/cJSON.c \
    ../httprelay/uvlib/*.c \
    ../httprelay/uvlib/llhttp/src/*.c \
    ../httprelay/*.c \
    -I../httprelay/uvlib/llhttp/include \
    -I../../src \
    -luv -lcurl \
    -o httprelay
