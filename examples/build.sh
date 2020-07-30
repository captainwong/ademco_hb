#!/bin/bash

mkdir -p build
cd build
opt=`echo " -std=c++11 -O3 –DNDEBUG"`
g++ $opt ../bench/bench.cpp -I../../include -o bench
g++ $opt ../demo/demo.cpp -I../../include -o demo
g++ $opt ../server_demo/server_demo.cpp -I../../include -lpthread -o server_demo
g++ $opt ../server_demo_libevent/server_demo_libevent.cpp -I../../include `pkg-config --cflags --libs libevent` -levent_core -levent_pthreads -lpthread -o server_demo_libevent
g++ $opt ../bench_client/bench_client.cpp -I../../include `pkg-config --cflags --libs libevent` -levent_core -levent_pthreads -lpthread -o bench_client


