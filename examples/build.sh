#!/bin/bash

mkdir -p build
cd build
opt=-O3
g++ -std=c++11 $opt ../bench/bench.cpp -I../../include -o bench
g++ -std=c++11 $opt ../demo/demo.cpp -I../../include -o demo
g++ -std=c++11 $opt ../server_demo/server_demo.cpp -I../../include -lpthread -o server_demo
g++ -std=c++11 $opt ../server_demo_libevent/server_demo_libevent.cpp -I../../include `pkg-config --cflags --libs libevent` -levent_core -levent_pthreads -lpthread -o server_demo_libevent
g++ -std=c++11 $opt ../bench_client/bench_client.cpp -I../../include `pkg-config --cflags --libs libevent` -levent_core -levent_pthreads -lpthread -o bench_client


