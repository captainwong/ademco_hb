#pragma once

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <assert.h>

using namespace std::chrono;

#include "../../c/ademco.h"


#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/ademco.lib")
#else
#pragma comment(lib, "../x64/Release/ademco.lib")
#endif
#else // WIN32
#ifdef _DEBUG
#pragma comment(lib, "../Debug/ademco.lib")
#else
#pragma comment(lib, "../Release/ademco.lib")
#endif
#endif


#ifdef _DEBUG
constexpr int TIMES = 10000;
#else
constexpr int TIMES = 100000;
#endif

uint8_t data[TIMES * 256];


void ademcoPacketBench()
{
	printf("\nademcoPacketBench...\n");
	printf("generating data...\n");
	//std::vector<char> data; data.reserve(TIMES * 100);
	//char tmp[256];
	size_t total_len = 0;
	for (int i = 0; i < TIMES; i++) {
		auto len = ademcoMakeHbPacket((data + total_len), sizeof(data) - total_len, 
									  (i + 1) % 10000, std::to_string(i).c_str(), (i + 1) % ADEMCO_ID_SENTINEL, EVENT_ARM, 0, 0, NULL);
		assert(len > 0);
		total_len += len;
		if ((i + 1) % 1000 == 0) {
			printf("\r%d/%d", i + 1, TIMES);
			//std::this_thread::sleep_for(milliseconds(15));
		}
	}

	printf("\ndone\ntesting parse...\n");

	auto start = steady_clock::now();
	int cb = 0;
	AdemcoPacket pkt;
	auto res = ademcoPacketParse(&data[0], total_len, &pkt, &cb);
	assert(res == AdemcoParseResult::RESULT_OK);

	size_t pos = 0;
	int packets = 0;
	while (res == AdemcoParseResult::RESULT_OK) {
		packets++;
		if (packets % 500 == 0) {
			printf("\r%d/%d", packets, TIMES);
		}
		pos += cb;
		if (total_len > pos) {
			res = ademcoPacketParse(&data[pos], total_len - pos, &pkt, &cb);
		} else { break; }
	}

	assert(res == AdemcoParseResult::RESULT_OK);
	assert(packets == TIMES);

	auto duration = steady_clock::now() - start;
	auto ms = duration_cast<milliseconds>(duration).count();
	auto us = duration_cast<microseconds>(duration).count();
	double avg = us * 1.0 / TIMES;
	printf("\ndone\nduration=%ldms\navg parse time=%.2lfus\n", (long)ms, avg);
	double avgpps = 1000.0 * TIMES / ms;
	printf("average packets per second=%.2lf\n", avgpps);
	double avgbps = total_len * 1000.0 / ms;
	printf("average bytes per second=%.2lfB/s, %.2lfKB/s\n", avgbps, avgbps / 1024.0);

	system("pause");
}


