#include "../ademco_hb/ademco_hb.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef _DEBUG
#pragma comment(lib, "../Debug/ademco_hbd.lib")
#else
#pragma comment(lib, "../Release/ademco_hb.lib")
#endif

int main()
{
	int cb_commited = 0;

	// 解析完整数据包
	{
		char raw[] = "\nC5C30053\"HENG-BO\"0000R000000L000000#90219125916578[#000000|1737 00 000]_09:11:19,08-05-2019\r";
		int res = parse(raw, strlen(raw), &cb_commited);
		assert(res == 0);
	}

	// 解析长度不足的数据包
	{
		char raw_not_enough[] = "\nC5C30053\"HENG-BO\"0000R000000L000000";
		int res = parse(raw_not_enough, strlen(raw_not_enough), &cb_commited);
		assert(res == 1);
	}

	// 解析到错误数据包
	{
		char raw_error[] = "abcdeadfasdfasdfasd";
		int res = parse(raw_error, strlen(raw_error), &cb_commited);
		assert(res == 2);
	}

	// 打包示例
	{
		char buff[1024];
		// 对安定宝ID为666666的主机进行撤防
		int res = pack(buff, sizeof(buff), 1, 666666, 1400, 0, 0);
		assert(res > 0);
		//res = parse(buff, res, &cb_commited);
		//assert(res == 0);
	}

	// ACK 示例
	{
		char buff[1024];
		// 对安定宝ID为666666的主机回应ACK
		int res = pack_ack(buff, sizeof(buff), 1, 666666);
		assert(res > 0);
		//res = parse(buff, res, &cb_commited);
		//assert(res == 0);
	}
}
