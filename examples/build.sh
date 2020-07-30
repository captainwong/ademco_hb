#!/bin/bash

mkdir -p build
cd build
opt="-std=c++11 -O3"
g++ $opt –DNDEBUG ../bench/bench.cpp -I../../include -o bench
g++ $opt –DNDEBUG ../demo/demo.cpp -I../../include -o demo
g++ $opt –DNDEBUG ../server_demo/server_demo.cpp -I../../include -lpthread -o server_demo
g++ $opt –DNDEBUG ../server_demo_libevent/server_demo_libevent.cpp -I../../include `pkg-config --cflags --libs libevent` -levent_core -levent_pthreads -lpthread -o server_demo_libevent
g++ $opt –DNDEBUG ../bench_client/bench_client.cpp -I../../include `pkg-config --cflags --libs libevent` -levent_core -levent_pthreads -lpthread -o bench_client


