#pragma once

#define DISABLE_JLIB_LOG2
#include "../../include/ademco_packet.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>

using namespace std::chrono;

#ifdef _DEBUG
constexpr int TIMES = 10000;
#else
constexpr int TIMES = 100000;
#endif

char data[TIMES * 256];



using namespace ademco;


void ademcoPacketBench()
{
	printf("\nademcoPacketBench...\n");
	printf("generating data...\n");
	//std::vector<char> data; data.reserve(TIMES * 100);
	AdemcoPacket ap;
	//char tmp[256];
	size_t total_len = 0;
	for (int i = 0; i < TIMES; i++) {
		auto len = ap.make_hb(data + total_len, sizeof(data) - total_len, (i + 1) % 10000, std::to_string(i), (i + 1) % AdemcoIdSentinel, 0, EVENT_ARM, 0);
		assert(len > 0);
		total_len += len;
		if ((i + 1) % 1000 == 0) {
			printf("\r%d/%d %s", i + 1, TIMES, ap.timestamp_.data_);
			std::this_thread::sleep_for(milliseconds(15));
		}
	}

	printf("\ndone\ntesting parse...\n");

	auto start = steady_clock::now();
	size_t cb = 0;
	auto res = ap.parse(&data[0], total_len, cb);
	assert(res == ParseResult::RESULT_OK);

	size_t pos = 0;
	int packets = 0;
	while (res == ParseResult::RESULT_OK) {
		packets++;
		if (packets % 500 == 0) {
			printf("\r%d/%d %s", packets, TIMES, ap.timestamp_.data_);
		}
		pos += cb;
		if (total_len > pos) {
			res = ap.parse(&data[pos], total_len - pos, cb);
		} else { break; }
	}

	assert(res == ParseResult::RESULT_OK);
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