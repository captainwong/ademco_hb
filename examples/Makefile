
DEBUG ?= 0
ENABLE_BREAKPAD ?= 0

CXXFLAGS += -gdwarf -g -Wall -Wno-unused-variable -Wno-unused-function -std=c++11
CXX = g++

INCS = -I../include \
	`pkg-config --cflags libevent`

LIBS = `pkg-config --libs libevent` \
	-lpthread \
	-levent_pthreads

ifeq ($(DEBUG),1)
CXXFLAGS += -DDEBUG -O0
else
CXXFLAGS += -DNDEBUG -O3
endif

ifeq ($(ENABLE_BREAKPAD),1)
INCS += `pkg-config --cflags breakpad`
LIBS += `pkg-config --libs breakpad`
endif

OUT_DIR = ./build
OBJ_DIR = $(OUT_DIR)/objs

all: make_dir bench bench_client demo server_demo server_demo_libevent


.PHONY : make_dir
make_dir:
	-mkdir -p $(OBJ_DIR)


$(OBJ_DIR)/bench.o : bench/bench.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $(OBJ_DIR)/bench.o $(INCS)

$(OBJ_DIR)/bench_client.o : bench_client/bench_client.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $(OBJ_DIR)/bench_client.o $(INCS)

$(OBJ_DIR)/demo.o : demo/demo.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $(OBJ_DIR)/demo.o $(INCS)

$(OBJ_DIR)/server_demo.o : server_demo/server_demo.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $(OBJ_DIR)/server_demo.o $(INCS)

$(OBJ_DIR)/server_demo_libevent.o : server_demo_libevent/server_demo_libevent.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $(OBJ_DIR)/server_demo_libevent.o $(INCS)


bench: $(OBJ_DIR)/bench.o
	$(CXX) $^ $(LDFLAGS) -o $(OUT_DIR)/bench $(LIBS)

bench_client: $(OBJ_DIR)/bench_client.o
	$(CXX) $^ $(LDFLAGS) -o $(OUT_DIR)/bench_client $(LIBS)

demo: $(OBJ_DIR)/demo.o
	$(CXX) $^ $(LDFLAGS) -o $(OUT_DIR)/demo $(LIBS)

server_demo: $(OBJ_DIR)/server_demo.o
	$(CXX) $^ $(LDFLAGS) -o $(OUT_DIR)/server_demo $(LIBS)

server_demo_libevent: $(OBJ_DIR)/server_demo_libevent.o
	$(CXX) $^ $(LDFLAGS) -o $(OUT_DIR)/server_demo_libevent $(LIBS)



.PHONY : clean
clean:
	-rm -rf $(OUT_DIR)

