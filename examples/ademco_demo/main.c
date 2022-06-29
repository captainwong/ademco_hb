#include "../../c/ademco.h"
#include <assert.h>
#include <string.h>

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

int main()
{
	{
		uint16_t crc = ademcoCRC16("0", 1);
		crc = ademcoCRC16("123456789", 9);

		char data[] = "\xEB\xBA";

		char data2[] = { 0xEB, 0xBA };
		int ret = memcmp("\xEB\xBA", data2, 2);

		ret == 0;
	}


	int cb_commited = 0;
	AdemcoPacket pkt;

	// 解析完整数据包
	{
		char raw[] = "\nC5C30053\"HENG-BO\"0000R000000L000000#90219125916578[#000000|1737 00 000]_09:11:19,08-05-2019\r";
		AdemcoParseResult res = ademcoPacketParse(raw, (int)strlen(raw), &pkt, &cb_commited);
		assert(res == RESULT_OK);
	}

	// 解析长度不足的数据包
	{
		char raw_not_enough[] = "\nC5C30053\"HENG-BO\"0000R000000L000000";
		AdemcoParseResult res = ademcoPacketParse(raw_not_enough, (int)strlen(raw_not_enough), &pkt, &cb_commited);
		assert(res == RESULT_NOT_ENOUGH);
	}

	// 解析到错误数据包
	{
		char raw_error[] = "abcdeadfasdfasdfasd";
		AdemcoParseResult res = ademcoPacketParse(raw_error, (int)strlen(raw_error), &pkt, &cb_commited);
		assert(res == RESULT_ERROR);
	}

	// 打包示例
	{
		char buff[1024];
		// 对安定宝ID为666666的主机进行撤防
		int len = ademcoMakeHbPacket(buff, sizeof(buff), 1, "861234567890", 666666, 1400, 0, 0, NULL);
		assert(len > 0);
		AdemcoParseResult res = ademcoPacketParse(buff, len, &pkt, &cb_commited);
		assert(res == RESULT_OK);
		assert(cb_commited == len);
	}

	// ACK 示例
	{
		char buff[1024];
		// 对安定宝ID为666666的主机回应ACK
		int len = ademcoMakeAckPacket(buff, sizeof(buff), 1, "861234567890", 666666);
		assert(len > 0);
		AdemcoParseResult res = ademcoPacketParse(buff, len, &pkt, &cb_commited);
		assert(res == RESULT_OK);
		assert(cb_commited == len);
	}

	
}

