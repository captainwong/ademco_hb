#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#define DISABLE_JLIB_LOG2
#include <ademco_packet.h>
#include <ademco_detail.h>

using namespace ademco;

#define EXPECT_EQ(a, b) assert((a) == (b))
#define EXPECT_TRUE(a) assert(a)
#define EXPECT_STREQ(a, b) assert(strcmp((a), (b)) == 0)

void test()
{
	char pack[1024] = { 0 };
	AdemcoPacket ap = {};

	// parse data from self made
	{
		auto size = ap.make(pack, sizeof(pack));
		size_t cb_commited = 0;
		EXPECT_EQ(ap.parse(pack, size, cb_commited), ParseResult::RESULT_OK);
		EXPECT_EQ(cb_commited, size);
	}

	// parse data from production
	{
		char raw[] = "\nC5C30053\"HENG-BO\"0000R000000L000000#90219125916578[#000000|1737 00 000]_09:11:19,08-05-2019\r";
		size_t cb_commited = 0;
		EXPECT_EQ(ap.parse(raw, strlen(raw), cb_commited), ParseResult::RESULT_OK);
		EXPECT_EQ(cb_commited, strlen(raw));
		EXPECT_EQ(ap.crc_.value_, 0xC5C3);
		EXPECT_EQ(ap.len_.value_, 0x0053);
		EXPECT_EQ(ap.id_.eid_, AdemcoId::Enum::id_hb);
		EXPECT_EQ(ap.seq_.value_, 0); // 0 is not a valid seq, but...
		EXPECT_TRUE(strncmp("R000000", ap.rrcvr_.data_.data(), 7) == 0);
		EXPECT_TRUE(strncmp("L000000", ap.lpref_.data_.data(), 7) == 0);
		EXPECT_STREQ(ap.acct_.acct(), "90219125916578");
		EXPECT_EQ(ap.ademcoData_.ademco_id_, 0);
		EXPECT_EQ(ap.ademcoData_.ademco_event_, ADEMCO_EVENT::EVENT_I_AM_WIRE_MACHINE);
		EXPECT_EQ(ap.ademcoData_.gg_, 0);
		EXPECT_EQ(ap.ademcoData_.zone_, 0);
		EXPECT_STREQ(ap.timestamp_.data_, "_09:11:19,08-05-2019");
	}
}

void generate_network_data()
{
	char pack[1024];
	AdemcoPacket ap;
	ap.make_hb(pack, sizeof(pack), 1, "861234567890", 666666, 0, ademco::EVENT_ARM, 0);
	printf("generate_network_data:%s\n", ap.toString().data());
}

void handle_network_data(const char* data_from_network)
{
	AdemcoPacket ap;
	size_t cb_commited = 0;
	auto result = ap.parse(data_from_network, strlen(data_from_network), cb_commited);

	switch (result) {
	case ademco::ParseResult::RESULT_OK:
	{
		printf("parse ok:\n%s\n", ap.toString().data());
		switch (ap.id_.eid_) {
		case AdemcoId::id_ack:
			// success
			break;

		case AdemcoId::id_hb: // event report
			// reply ack
			{
				char ack[1024];
				ap.make_ack(ack, sizeof(ack), ap.seq_.value_, ap.acct_.acct(), ap.ademcoData_.ademco_id_);
				// send to machine via network
			}

			// handle event
			printf("%s\n", ap.ademcoData_.toString().data());
			break;

		default:
			break;
		}
		break;
	}

	case ademco::ParseResult::RESULT_NOT_ENOUGH: 
		printf("not enough\n");
		break;

	case ademco::ParseResult::RESULT_DATA_ERROR: 
	default:
		printf("cannot parse\n");
		break;
	}
}

int main()
{
	test();

	generate_network_data();

	char data_from_network[] = "\nC5C30053\"HENG-BO\"0000R000000L000000#90219125916578[#000000|1737 00 000]_09:11:19,08-05-2019\r";
	handle_network_data(data_from_network);
	handle_network_data("\n593F0034\"ACK\"0209R123ABCL456DEF#000001[]_09:00:00,04-18-2020\r");
	


	char buff[1024];
	AdemcoPacket ap; 

	ap.make(buff, sizeof(buff));
	handle_network_data(buff);

	ap.make_ack(buff, sizeof(buff), 1, "861234567890", 123321);
	handle_network_data(buff);

	size_t ademco_id = 123456; // 主机标识
	size_t zone = 123; // 防区标识
	auto evnt = EVENT_FIRE; // 事件码
	ap.make_hb(buff, sizeof(buff), 1, nullptr, ademco_id, 0, evnt, zone);
	handle_network_data(buff);

}
