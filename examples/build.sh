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


${cxx} -std=c++11 -O3 ../server_demo/server_demo.cpp ../../ademco.c -I../../ -lpthread -o server_demo

${cc} ../../ademco.c ../../hb_core.c ../httprelay/cJSON/cJSON.c ../httprelay/uvlib/*.c ../httprelay/uvlib/llhttp/src/*.c ../httprelay/*.c -I ../httprelay/uvlib/llhttp/include -luv -lcurl -o httprelay
