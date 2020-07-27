#!/bin/bash

mkdir -p build
cd build
g++ -std=c++11 ../bench/bench.cpp -I../../include -o bench
g++ -std=c++11 ../demo/demo.cpp -I../../include -o demo
g++ -std=c++11 ../server_demo/server_demo.cpp -I../../include -lpthread -o server_demo
g++ -std=c++11 ../server_demo_libevent/server_demo_libevent.cpp -I../../include `pkg-config --cflags --libs libevent` -levent_core -levent_pthreads -lpthread -o server_demo_libevent


