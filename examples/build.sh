#!/bin/bash

mkdir -p build
cd build

opt="-std=c++11 -O3"

eb=0
if [ x$1 != x ] ;then
    if [ $1 -eq 1 ]; then
        eb=1
    fi
fi

g++ $opt ../server_demo/server_demo.cpp ../../ademco.c -I../../ -lpthread -o server_demo