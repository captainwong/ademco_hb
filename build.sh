#!/bin/bash

mkdir -p build
cd build
g++ -std=c++17 ../bench/bench.cpp -I../include -o bench
g++ -std=c++17 ../demo/demo.cpp -I../include -o demo
g++ -std=c++17 ../server_demo/server_demo.cpp -I../include -lpthread -o server_demo


