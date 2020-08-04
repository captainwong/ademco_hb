#!/bin/bash

mkdir -p build
cd build
opt="-std=c++11 -O3"
g++ $opt ../bench/bench.cpp -I../../include -o bench
g++ $opt ../demo/demo.cpp -I../../include -o demo
g++ $opt ../server_demo/server_demo.cpp -I../../include -lpthread -o server_demo
g++ -std=c++11 -O3 -DNDEBUG -DENABLE_BREAKPAD=$1 -g -gdwarf ../server_demo_libevent/server_demo_libevent.cpp -I../../include `pkg-config --cflags --libs libevent breakpad` -levent_core -levent_pthreads -lpthread -lbreakpad_client -o server_demo_libevent
g++ $opt ../bench_client/bench_client.cpp -I../../include `pkg-config --cflags --libs libevent` -levent_core -levent_pthreads -lpthread -o bench_client


