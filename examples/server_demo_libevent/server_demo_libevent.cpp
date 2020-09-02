#ifndef _WIN32
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#else
# ifndef  _CRT_SECURE_NO_WARNINGS
#  define  _CRT_SECURE_NO_WARNINGS
# endif
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <thread>
#include <vector>
#include <mutex>
#include <string>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <map>

#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

#if ENABLE_BREAKPAD 
#ifdef _WIN32
#include <client/windows/handler/exception_handler.h>
static bool dumpCallback(const wchar_t* dump_path,
						 const wchar_t* minidump_id,
						 void* context,
						 EXCEPTION_POINTERS* exinfo,
						 MDRawAssertionInfo* assertion,
						 bool succeeded)
{
	printf("Dump path: %ls\n", dump_path);
	return succeeded;
}

#else
#include <client/linux/handler/exception_handler.h>
static bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,
						 void* context, bool succeeded)
{
	printf("Dump path: %s\n", descriptor.path());
	return succeeded;
}
#endif
#endif


#define DISABLE_JLIB_LOG2
#include <ademco_packet.h>

#define ENABLE_COMMON_MACHINE_STATUS_TO_STRING
#define ENABLE_COMMON_MACHINE_TYPE_TO_STRING
#define ENABLE_COMMON_ZONE_PROPERTY_TO_STRING
#include <hb_detail.h>
#include <hb_helper.h>

using namespace ademco;
using namespace hb;
using namespace hb::common;

DECLARE_HB_COMMON_COM_CONSTEXPR_MEMBERS;


enum class QueryStage {
	None,

	WaitingSettingsMode,
	QueryingZones,
	QueryingLostConfig,

	WaitingSettingsMode2,
	QueryingTimer,
};

struct ZonePropertyAndLostConfig {
	ZoneProperty prop = ZoneProperty::InvalidZoneProperty;
	bool tamper_enabled = false; // 失联开关
};

struct Client {
	int fd = 0;
	evbuffer* output = nullptr;
	std::string acct = {};
	AdemcoId ademco_id = 0;
	uint16_t seq = 0;
	int type = -1;
	MachineStatus status = MachineStatus::Arm;
	MachineStatus status1 = MachineStatus::Arm;
	MachineStatus status2 = MachineStatus::Arm;
	MachineStatus status3 = MachineStatus::Arm;
	std::map<size_t, ZonePropertyAndLostConfig> zones = {};
	QueryStage queryStage = QueryStage::None;
	com::MachineTimer timer = {};

	uint16_t nextSeq() { if (++seq == 10000) { seq = 1; } return seq; }

	void printInfo() const;
	Client& operator=(const Client&) = default;
};

struct ThreadContext {
	int worker_id = -1;
	event_base* base = nullptr;
	std::mutex mutex = {};
	std::unordered_map<int, Client> clients = {};
	AdemcoPacket packet = {};
};

event_base* listen_thread_evbase = nullptr;
int thread_count = 4;
std::vector<ThreadContext*> worker_thread_contexts = {};

// Commands
// M
#define COMMAND_M_EGZ EVENT_INVALID_EVENT
// C
#define COMMAND_C_AEGZ ((ADEMCO_EVENT)(EVENT_INVALID_EVENT + 1))
// X
#define COMMAND_X_AEGZX ((ADEMCO_EVENT)(EVENT_INVALID_EVENT + 2))
// Y
#define COMMAND_Y_AEGZX ((ADEMCO_EVENT)(EVENT_INVALID_EVENT + 3))
// Z
#define COMMAND_Z_QUERY_ZONE ((ADEMCO_EVENT)(EVENT_INVALID_EVENT + 4))
// F 
#define COMMAND_F_QUERY_TIMER ((ADEMCO_EVENT)(EVENT_INVALID_EVENT + 5))


// events will be sent to all clients
std::vector<ADEMCO_EVENT> commands = {};
int threads_to_handled_event = 0;
std::mutex mutex = {};
struct UserInput {
	char pwd[1024] = { 0 };
	AdemcoId ademco_id = 0;
	int ev = 0;
	AdemcoGG gg = 0;
	AdemcoZone zone = 0;
	char xdata[1024] = { 0 };
}userInput = {};


int disable_data_print = 0;

void op_usage()
{
	printf("Press a key and hit <enter>:\n"
		   "A: Arm\n"
		   "D: Disarm, require 6 digits password\n"
		   "E: Emergency\n"
		   "T: Query machine type\n"
		   "M: Mannualy input [event gg zone], Exampel Input: 'M' <enter> 3400 1 0 <enter>\n"
		   "C: Like M, not send to all clients, but send to specific client with ademco_id: [ademco_id event gg zone]\n"
		   "X: Like C, with xdata: [ademco_id event gg zone xdata]\n"
		   "Y: Like X, with xdata, but xdata is hex: [ademco_id event gg zone xdata], example: [1 1704 0 0 EBAB3FA176]\n"
		   "Z: Query Zone info: [ademco_id]\n"
		   "F: Query Machine Timer: [ademco_id]\n"
		   "\n"
		   "I: Print clients info\n"
		   "P: Toggle enable/disable data print\n"
		   "Q: Quit\n"
		   );
}

void usage(const char* name)
{
	printf("Usage: %s [listening_port] [thread_count] [disable_data_print]\n", name);
}

void handle_com_passthrough(ThreadContext* context, Client& client, evbuffer* output)
{	
	if (!context->packet.xdata_) {
		printf("\tWARNING! 1704 NO XDATA!\n");
		return;
	}

	// GRPS cannot send us hex data, we need to convert data like "1234ABCDEF" to hex 0x1234ABCDEF 
	std::vector<char> xdata(context->packet.xdata_->data_.size() / 2);
	detail::ConvertHiLoAsciiToHexCharArray(&xdata[0], context->packet.xdata_->data_.data(), context->packet.xdata_->data_.size());
	printf("\t%s\n", detail::toString(xdata, detail::ToStringOption::ALL_CHAR_AS_HEX, false, false).data());

	char buf[1024];
	auto resp_type = com::ResponseParser::parse((const Char*)xdata.data(), xdata.size() & 0xFF);
	switch (resp_type) {
	case com::ResponseParser::ResponseType::A0_response:
		break;
	case com::ResponseParser::ResponseType::A2_response:
		{
			com::Resp_A2 resp; memcpy(resp.data, xdata.data(), xdata.size()); resp.len = xdata.size() & 0xFF;
			com::Resp_A2::ZoneAndProperties zps; bool hasMore = false;
			if (client.queryStage == QueryStage::QueryingZones && resp.parse(zps, hasMore)) {
				for (const auto& zp : zps) {
					ZonePropertyAndLostConfig zplc;
					zplc.prop = zp.prop;
					zplc.tamper_enabled = false;
					client.zones[zp.zone] = zplc;
					snprintf(buf, sizeof(buf), getZoneFormatString(machineTypeFromAdemcoEvent((ADEMCO_EVENT)client.type)), zp.zone);
					printf("\t\tZone:");
					printf("%s", buf);
					printf("  Prop:0x%02X %s\n", zp.prop, zonePropertyToStringEn(zp.prop));
				}
				XDataPtr xdata;
				if (hasMore) { // 继续索要剩余防区
					com::Req_A2 req;
					xdata = makeXData((const char*)req.data, req.len);					
				} else { // 开始索要防区失联设置
					com::Req_AC req;
					xdata = makeXData((const char*)req.data, req.len);
					client.queryStage = QueryStage::QueryingLostConfig;
				}

				auto n = context->packet.make_hb(buf, sizeof(buf), client.nextSeq(), client.acct, client.ademco_id, 0, 
												 EVENT_COM_PASSTHROUGH, 0, xdata);
				evbuffer_add(output, buf, n);
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
						   context->worker_id, client.fd, client.acct.data(), client.ademco_id,
						   context->packet.toString(detail::ToStringOption::ALL_CHAR_AS_HEX).data());
				}
			} else {
				auto n = context->packet.make_hb(buf, sizeof(buf), client.nextSeq(), client.acct, client.ademco_id, 0,
												 EVENT_EXIT_SET_MODE, 0);
				evbuffer_add(output, buf, n);
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
						   context->worker_id, client.fd, client.acct.data(), client.ademco_id,
						   context->packet.toString(detail::ToStringOption::ALL_CHAR_AS_HEX).data());
				}
			}
			break;
		}
	case com::ResponseParser::ResponseType::A3_response:
		break;
	case com::ResponseParser::ResponseType::A4_response:
		break;
	case com::ResponseParser::ResponseType::A6_response:
		{
			if (client.queryStage == QueryStage::QueryingTimer) {
				client.queryStage = QueryStage::None;
				com::Resp_A6 resp; memcpy(resp.data, xdata.data(), resp.len);
				client.timer = resp.getTimer();
				for (int i = 0; i < 2; i++) {
					if (com::isValidMachineTimer(client.timer.timer[i])) {
						printf("\t\tMachine Timer#%d: Arm at %02d:%02d, Disarm at %02d:%02d\n", i + 1,
							   client.timer.timer[i].armAt.hour, client.timer.timer[i].armAt.minute,
							   client.timer.timer[i].disarmAt.hour, client.timer.timer[i].disarmAt.minute);
					} else {
						printf("\t\tMachine Timer#%d: Not Set\n", i + 1);
					}
				}
			}
			auto n = context->packet.make_hb(buf, sizeof(buf), client.nextSeq(), client.acct, client.ademco_id, 0,
											 EVENT_EXIT_SET_MODE, 0);
			evbuffer_add(output, buf, n);
			if (!disable_data_print) {
				printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
					   context->worker_id, client.fd, client.acct.data(), client.ademco_id,
					   context->packet.toString(detail::ToStringOption::ALL_CHAR_AS_HEX).data());
			}
		}
		break;
	case com::ResponseParser::ResponseType::A7_response:
		break;
	case com::ResponseParser::ResponseType::A8_response:
		break;
	case com::ResponseParser::ResponseType::A9_response:
		break;
	case com::ResponseParser::ResponseType::AB_response:
		break;
	case com::ResponseParser::ResponseType::AD_response:
		{
			com::Resp_AD resp; memcpy(resp.data, xdata.data(), xdata.size()); resp.len = xdata.size() & 0xFF;
			bool hasMore = false;
			std::vector<size_t> zones;
			if (client.queryStage == QueryStage::QueryingLostConfig && resp.parse(zones, hasMore)) {
				for (const auto& zone : zones) {
					client.zones[zone].tamper_enabled = true;
					snprintf(buf, sizeof(buf), getZoneFormatString(machineTypeFromAdemcoEvent((ADEMCO_EVENT)client.type)), zone);
					printf("\t\tZone:");
					printf("%s", buf);
					printf("  Tamper Enabled: true\n");
				}
				XDataPtr xdata;
				if (hasMore) { // 继续索要剩余防区
					com::Req_A2 req;
					xdata = makeXData((const char*)req.data, req.len);
					auto n = context->packet.make_hb(buf, sizeof(buf), client.nextSeq(), client.acct, client.ademco_id, 0,
													 EVENT_COM_PASSTHROUGH, 0, xdata);
					evbuffer_add(output, buf, n);
					if (!disable_data_print) {
						printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
							   context->worker_id, client.fd, client.acct.data(), client.ademco_id,
							   context->packet.toString(detail::ToStringOption::ALL_CHAR_AS_HEX).data());
					}
					break;
				}
			} 
			auto n = context->packet.make_hb(buf, sizeof(buf), client.nextSeq(), client.acct, client.ademco_id, 0,
											 EVENT_EXIT_SET_MODE, 0);
			evbuffer_add(output, buf, n);
			if (!disable_data_print) {
				printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
					   context->worker_id, client.fd, client.acct.data(), client.ademco_id,
					   context->packet.toString(detail::ToStringOption::ALL_CHAR_AS_HEX).data());
			}
			break;
		}
	case com::ResponseParser::ResponseType::AE_response:
		break;
	case com::ResponseParser::ResponseType::B1_response:
		{			
			com::Resp_B1 b1; memcpy(b1.data.data, xdata.data(), xdata.size());
			client.status1 = b1.data.cmd.status1 == 0 ? MachineStatus::Arm : MachineStatus::Disarm;
			client.status2 = b1.data.cmd.status2 == 0 ? MachineStatus::Arm : MachineStatus::Disarm;
			client.status3 = b1.data.cmd.status3 == 0 ? MachineStatus::Arm : MachineStatus::Disarm;
			printf("\t\t status1: %d %s\n\t\t status2: %d %s\n\t\t status3: %d %s\n",
					(int)client.status1, machineStatusToString(client.status1),
					(int)client.status2, machineStatusToString(client.status2),
					(int)client.status3, machineStatusToString(client.status3));			
			
			break;
		}
	case com::ResponseParser::ResponseType::Invalid_response:
		break;
	default:
		break;
	}

}

void handle_ademco_msg(ThreadContext* context, bufferevent* bev)
{
	int fd = (int)bufferevent_getfd(bev);
	auto& client = context->clients[fd];
	if (!disable_data_print) {
		printf("T#%d C#%d acct=%s ademco_id=%06X :%s\n",
			   context->worker_id, fd, client.acct.data(), client.ademco_id, context->packet.toString().data());
	}
	auto output = bufferevent_get_output(bev);
	switch (context->packet.id_.eid_) {
	case AdemcoMsgId::id_null:
	case AdemcoMsgId::id_hb:
	case AdemcoMsgId::id_admcid:
		{
			char buf[1024];
			if (context->packet.id_.eid_ != AdemcoMsgId::id_null) {
				client.acct = context->packet.acct_.acct();
				client.ademco_id = context->packet.ademcoData_.ademco_id_;
				if (ademco::isMachineTypeEvent(context->packet.ademcoData_.ademco_event_)) {
					client.type = context->packet.ademcoData_.ademco_event_;
					if (client.type == EVENT_I_AM_3_SECTION_MACHINE) { // 三区段主机需要主动索要主机状态
						com::Req_B0 req;
						auto xdata = makeXData((const char*)req.data, req.len);
						auto n = context->packet.make_hb(buf, sizeof(buf), client.nextSeq(), client.acct, client.ademco_id, 0, 
														 EVENT_COM_PASSTHROUGH, 0, xdata);
						evbuffer_add(output, buf, n);
						if (!disable_data_print) {
							printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
								   context->worker_id, fd, client.acct.data(), client.ademco_id, 
								   context->packet.toString(detail::ToStringOption::ALL_CHAR_AS_HEX).data());
						}
						break;
					}
				} else if (ademco::isMachineStatusEvent(context->packet.ademcoData_.ademco_event_)) {
					auto status = machienStatusFromAdemcoEvent(context->packet.ademcoData_.ademco_event_);
					if (context->packet.ademcoData_.gg_ == 0) { client.status = status; }
					else if (context->packet.ademcoData_.gg_ == 1) { client.status1 = status; } 
					else if (context->packet.ademcoData_.gg_ == 2) { client.status2 = status; } 
					else if (context->packet.ademcoData_.gg_ == 3) { client.status3 = status; }
					else { break; }
				} else if (context->packet.ademcoData_.ademco_event_ == EVENT_COM_PASSTHROUGH) {
					handle_com_passthrough(context, client, output);
					break;
				} else if (context->packet.ademcoData_.ademco_event_ == EVENT_ENTER_SET_MODE) {
					if (client.queryStage == QueryStage::WaitingSettingsMode) {
						client.queryStage = QueryStage::QueryingZones;
						com::Req_A1 req;
						auto xdata = makeXData((const char*)req.data, req.len);
						auto n = context->packet.make_hb(buf, sizeof(buf), client.nextSeq(), client.acct, client.ademco_id, 0,
														 EVENT_COM_PASSTHROUGH, 0, xdata);
						evbuffer_add(output, buf, n);
						if (!disable_data_print) {
							printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
								   context->worker_id, fd, client.acct.data(), client.ademco_id,
								   context->packet.toString(detail::ToStringOption::ALL_CHAR_AS_HEX).data());
						}
					} else if (client.queryStage == QueryStage::WaitingSettingsMode2) {
						client.queryStage = QueryStage::QueryingTimer;
						com::Req_A5 req;
						auto xdata = makeXData((const char*)req.data, req.len);
						auto n = context->packet.make_hb(buf, sizeof(buf), client.nextSeq(), client.acct, client.ademco_id, 0,
														 EVENT_COM_PASSTHROUGH, 0, xdata);
						evbuffer_add(output, buf, n);
						if (!disable_data_print) {
							printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
								   context->worker_id, fd, client.acct.data(), client.ademco_id,
								   context->packet.toString(detail::ToStringOption::ALL_CHAR_AS_HEX).data());
						}
					}
					
				}
			}

			// ACK
			{
				size_t n = context->packet.make_ack(buf, sizeof(buf), context->packet.seq_.value_, 
													context->packet.acct_.acct(), context->packet.ademcoData_.ademco_id_);
				evbuffer_add(output, buf, n);
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
						   context->worker_id, fd, client.acct.data(), client.ademco_id, context->packet.toString().data());
				}
			}
			break;
		}
	default:
		break;
	}
}

// send all buffered events to all clients
void commandcb(evutil_socket_t, short, void* user_data)
{
	auto context = (ThreadContext*)user_data;
	std::vector<ADEMCO_EVENT> evs;

	{
		std::lock_guard<std::mutex> lg(mutex);
		if (--threads_to_handled_event == 0) {
			evs = std::move(commands);
		} else {
			evs = commands;
		}
	}

	char buf[1024];

	std::lock_guard<std::mutex> lg(context->mutex);
	for (auto& client : context->clients) {
		for (auto e : evs) {			
			size_t n = 0;
			if (e == COMMAND_M_EGZ) { // M
				n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id,
											userInput.gg, (ADEMCO_EVENT)userInput.ev, userInput.zone);
				evbuffer_add(client.second.output, buf, n);
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
						   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id, 
						   context->packet.toString().data());
				}
			} else if (e == (COMMAND_C_AEGZ)) { // C
				if (client.second.ademco_id != userInput.ademco_id) {
					continue;
				}
				n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id,
											userInput.gg, (ADEMCO_EVENT)userInput.ev, userInput.zone);
				evbuffer_add(client.second.output, buf, n);
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
						   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id, 
						   context->packet.toString().data());
				}
			} else if (e == (COMMAND_X_AEGZX)) { // X
				if (client.second.ademco_id != userInput.ademco_id) {
					continue;
				}
				auto xdata = makeXData(userInput.xdata, strlen(userInput.xdata));
				n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id,
											userInput.gg, (ADEMCO_EVENT)userInput.ev, userInput.zone, xdata);
				evbuffer_add(client.second.output, buf, n);
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
						   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id, 
						   context->packet.toString().data());
				}
			} else if (e == (COMMAND_Y_AEGZX)) { // Y
				if (client.second.ademco_id != userInput.ademco_id) {
					continue;
				}
				std::vector<char> data(strlen(userInput.xdata) / 2);
				ademco::detail::ConvertHiLoAsciiToHexCharArray(&data[0], userInput.xdata, strlen(userInput.xdata));
				auto xdata = makeXData(data);
				n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id,
											userInput.gg, (ADEMCO_EVENT)userInput.ev, userInput.zone, xdata);
				evbuffer_add(client.second.output, buf, n);
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
						   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id, 
						   context->packet.toString(ademco::detail::ToStringOption::ALL_CHAR_AS_HEX).data());
				}
			} else if (e == COMMAND_Z_QUERY_ZONE) {
				if (client.second.ademco_id != userInput.ademco_id) {
					continue;
				}
				n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id, 0, 
											EVENT_ENTER_SET_MODE, 0);
				evbuffer_add(client.second.output, buf, n);
				client.second.zones.clear();
				client.second.queryStage = QueryStage::WaitingSettingsMode;
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
						   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id,
						   context->packet.toString(ademco::detail::ToStringOption::ALL_CHAR_AS_HEX).data());
				}

			} else if (e == COMMAND_F_QUERY_TIMER) { // F
				if (client.second.ademco_id != userInput.ademco_id) {
					continue;
				}
				n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id, 0,
											EVENT_ENTER_SET_MODE, 0);
				evbuffer_add(client.second.output, buf, n);
				client.second.zones.clear();
				client.second.queryStage = QueryStage::WaitingSettingsMode2;
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
						   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id,
						   context->packet.toString(ademco::detail::ToStringOption::ALL_CHAR_AS_HEX).data());
				}
			} else if (client.second.type == EVENT_I_AM_3_SECTION_MACHINE && (e == EVENT_ARM || e == EVENT_DISARM)) {
				for (AdemcoGG gg = 1; gg <= 3; gg++) {
					if (e == EVENT_DISARM) {
						auto xdata = makeXData(userInput.pwd, 6);
						n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id, gg, e, 0, xdata);
					} else {
						n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id, gg, e, 0);
					}
					evbuffer_add(client.second.output, buf, n);
					if (!disable_data_print) {
						printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
							   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id, 
							   context->packet.toString().data());
					}
				}
			} else {
				if (e == EVENT_DISARM) {
					auto xdata = makeXData(userInput.pwd, 6);
					n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id, 0, e, 0, xdata);
				} else {
					n = context->packet.make_hb(buf, sizeof(buf), client.second.nextSeq(), client.second.acct, client.second.ademco_id, 0, e, 0);
				}
				evbuffer_add(client.second.output, buf, n);
				if (!disable_data_print) {
					printf("T#%d S#%d acct=%s ademco_id=%06X :%s\n",
						   context->worker_id, client.second.fd, client.second.acct.data(), client.second.ademco_id, 
						   context->packet.toString().data());
				}
			}
		}
	}
}

void readcb(struct bufferevent* bev, void* user_data)
{
	auto input = bufferevent_get_input(bev);
	auto context = (ThreadContext*)user_data;
	std::lock_guard<std::mutex> lg(context->mutex);

	while (1) {
		size_t len = evbuffer_get_length(input);
		if (len < 9) { return; }
		char buff[1024] = { 0 };
		evbuffer_copyout(input, buff, std::min(len, sizeof(buff)));
		size_t cb_commited = 0;
		auto res = context->packet.parse(buff, 1024, cb_commited);
		bool done = false;
		switch (res) {
		case ademco::ParseResult::RESULT_OK:
			evbuffer_drain(input, cb_commited);
			handle_ademco_msg(context, bev);
			break;
		case ademco::ParseResult::RESULT_NOT_ENOUGH:
			done = true;
			break;
		case ademco::ParseResult::RESULT_DATA_ERROR:
			fprintf(stderr, "error while parsing data\n");
			evbuffer_drain(input, len);
			done = true;
			break;
		}

		if (done) { break; }
	}
}

void eventcb(struct bufferevent* bev, short events, void* user_data)
{
	int fd = (int)bufferevent_getfd(bev);
	printf("eventcb events=%04X\n", events);
	if (events & BEV_EVENT_EOF) {
	} else if (events & (BEV_EVENT_WRITING)) {
		printf("Got an error while writing #%d.\n", fd);
	} else if (events & (BEV_EVENT_ERROR)) {
		printf("Got an error on the connection %d: %s\n",
			   fd, strerror(errno));
	}
	printf("Connection #%d closed.\n", fd); 
	auto context = (ThreadContext*)user_data;
	{
		std::lock_guard<std::mutex> lg(context->mutex);
		context->clients.erase(fd);
	}
	bufferevent_free(bev);
}

void accept_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* addr, int socklen, void* user_data)
{
	char str[INET_ADDRSTRLEN] = { 0 };
	auto sin = (sockaddr_in*)addr;
	inet_ntop(AF_INET, &sin->sin_addr, str, INET_ADDRSTRLEN);
	printf("accpet TCP connection #%d from: %s:%d\n", (int)fd, str, sin->sin_port);
	//evutil_make_socket_nonblocking(fd);

	static uint32_t worker_id = 0;
	auto context = worker_thread_contexts[worker_id];
	std::lock_guard<std::mutex> lg(context->mutex);
	auto bev = bufferevent_socket_new(context->base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!\n");
		event_base_loopbreak(context->base);
		return;
	}

	Client client;
	client.fd = (int)fd;
	client.output = bufferevent_get_output(bev);
	context->clients[(int)fd] = client;

	bufferevent_setcb(bev, readcb, nullptr, eventcb, context);
	bufferevent_enable(bev, EV_WRITE | EV_READ);
	worker_id = (worker_id + 1) % thread_count;	
}

void accpet_error_cb(evconnlistener* listener, void* context)
{
	auto base = evconnlistener_get_base(listener);
	int err = EVUTIL_SOCKET_ERROR();
	fprintf(stderr, "accpet_error_cb:%d:%s\n", err, evutil_socket_error_to_string(err));
	event_base_loopexit(base, nullptr);
}

void init_listener_thread(const sockaddr_in& addr)
{
	listen_thread_evbase = event_base_new();
	if (!listen_thread_evbase) {
		fprintf(stderr, "init libevent failed\n");
		exit(-1);
	}

	auto listener = evconnlistener_new_bind(listen_thread_evbase,
											accept_cb,
											nullptr,
											LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
											-1, // backlog, -1 for default
											(const sockaddr*)(&addr),
											sizeof(addr));
	if (!listener) {
		fprintf(stderr, "create listener failed\n");
		exit(-1);
	}
	evconnlistener_set_error_cb(listener, accpet_error_cb);
}

void dummy_cb_avoid_worker_exit(evutil_socket_t, short, void*)
{
}

ThreadContext* init_worker_thread(int i)
{
	ThreadContext* context = new ThreadContext();
	context->worker_id = i;
	context->base = event_base_new();
	if (!context->base) {
		fprintf(stderr, "init libevent failed\n");
		exit(-1);
	}
	timeval tv = { 1, 0 };
	event_add(event_new(context->base, -1, EV_PERSIST, dummy_cb_avoid_worker_exit, nullptr), &tv);
	printf("worker thread #%d created\n", i);
	return context;
}

void clear_stdin()
{
	int ret = scanf("%*[^\n]%*c");
	(void)ret;
}

int main(int argc, char** argv)
{
	usage(argv[0]);

#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
	if (0 != evthread_use_windows_threads()) {
		fprintf(stderr, "failed to init libevent with thread by calling evthread_use_windows_threads\n");
		return -1;
	}
#if ENABLE_BREAKPAD 
	google_breakpad::ExceptionHandler eh(L"./", // dump_path
										 nullptr, // FilterCallback 
										 dumpCallback, // MinidumpCallback 
										 nullptr, // callback_context
										 google_breakpad::ExceptionHandler::HANDLER_ALL // handler_types
	); // MINIDUMP_TYPE
#endif
#else 
	if (0 != evthread_use_pthreads()) {
		fprintf(stderr, "failed to init libevent with thread by calling evthread_use_pthreads\n");
		return -1;
	}
#if ENABLE_BREAKPAD 
	google_breakpad::ExceptionHandler eh(google_breakpad::MinidumpDescriptor("./"), nullptr, dumpCallback, nullptr, true, -1);
#endif
#endif

	int port = 12345; 
	if (argc > 1) {
		port = atoi(argv[1]);
		if (port <= 0 || port > 65535) {
			puts("Invalid port");
			return 1;
		}
	}

	if (argc > 2) {
		thread_count = atoi(argv[2]);
		if (thread_count <= 0) {
			puts("Invalid thread_count");
			return 1;
		}
	}

	if (argc > 3) {
		disable_data_print = atoi(argv[3]) == 1;
	}

	sockaddr_in sin = { 0 };
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons((uint16_t)(port & 0xFFFF));

	printf("using libevent %s\n", event_get_version());
	printf("%s is listening on port %d using %d threads, disable_data_print=%d\n",
		   argv[0], port, thread_count, disable_data_print);

	std::vector<std::thread> worker_threads;
	for (int i = 0; i < thread_count; i++) {
		auto context = init_worker_thread(i);
		worker_thread_contexts.push_back(context);
		worker_threads.emplace_back(std::thread([context]() {
			event_base_dispatch(context->base);
		}));
	}

	std::thread listener_thread([&sin]() {
		init_listener_thread(sin); 
		event_base_dispatch(listen_thread_evbase);
	});

	op_usage();

	auto fire_command = []() {
		struct timeval tv = { 0, 1000 };
		for (int i = 0; i < thread_count; i++) {
			event_add(event_new(worker_thread_contexts[i]->base, -1, 0, commandcb, worker_thread_contexts[i]), &tv);
		}
	};

	bool running = true;
	while (running) {
		int cmd = getchar(); 
		if ('a' <= cmd && cmd <= 'z') {
			cmd -= 32;
		}

		switch (cmd) {
		case 'A':
			{
				std::lock_guard<std::mutex> lg(mutex);
				commands.push_back(EVENT_ARM);
				threads_to_handled_event = thread_count;
			}
			fire_command();
			break;

		case 'D':
			{
				int ret = 0;
				do {
					printf("Input 6 digit password:");
					ret = scanf("%s", userInput.pwd);
					clear_stdin();
				} while (ret != 1 || strlen(userInput.pwd) != 6);
				{
					std::lock_guard<std::mutex> lg(mutex);
					commands.push_back(EVENT_DISARM);
					threads_to_handled_event = thread_count;
				}
				fire_command();
			break;
			}

		case 'E':
			{
				std::lock_guard<std::mutex> lg(mutex);
				commands.push_back(EVENT_EMERGENCY);
				threads_to_handled_event = thread_count;
			}
			fire_command();
			break;

		case 'T':
			{
				std::lock_guard<std::mutex> lg(mutex);
				commands.push_back(EVENT_WHAT_IS_YOUR_TYPE);
				threads_to_handled_event = thread_count;
			}
			fire_command();
			break;

		case 'M':
			{
				int ret = 0;
				do {
					printf("Input [event gg zone]:");
					ret = scanf("%d %hhd %d", &userInput.ev, &userInput.gg, &userInput.zone);
					clear_stdin();
				} while (ret != 3);
				{
					std::lock_guard<std::mutex> lg(mutex);
					commands.push_back(COMMAND_M_EGZ);
					threads_to_handled_event = thread_count;
				}
				fire_command();
				break;
			}

		case 'C':
			{
				int ret = 0;
				do {
					printf("Input [ademco_id event gg zone]:");
					ret = scanf("%d %d %hhd %d", &userInput.ademco_id, &userInput.ev, &userInput.gg, &userInput.zone);
					clear_stdin();
				} while (ret != 4);
				{
					std::lock_guard<std::mutex> lg(mutex);
					commands.push_back(COMMAND_C_AEGZ);
					threads_to_handled_event = thread_count;
				}
				fire_command();
				break;
			}

		case 'X':
		case 'Y':
			{
				int ret = 0;
				do {
					printf("Input [ademco_id event gg zone xdata]:");
					ret = scanf("%d %d %hhd %d %s", &userInput.ademco_id, &userInput.ev, &userInput.gg, &userInput.zone, userInput.xdata);
					clear_stdin();
				} while (ret != 5);
				{
					std::lock_guard<std::mutex> lg(mutex);
					commands.push_back(cmd == 'X' ? COMMAND_X_AEGZX : COMMAND_Y_AEGZX);
					threads_to_handled_event = thread_count;
				}
				fire_command();
				break;
			}

		case 'Z':
			{
				int ret = 0;
				do {
					printf("Input [ademco_id]:");
					ret = scanf("%d", &userInput.ademco_id);
					clear_stdin();
				} while (ret != 1);
				{
					std::lock_guard<std::mutex> lg(mutex);
					commands.push_back(COMMAND_Z_QUERY_ZONE);
					threads_to_handled_event = thread_count;
				}
				fire_command();
				break;
			}

		case 'F':
			{
				int ret = 0;
				do {
					printf("Input [ademco_id]:");
					ret = scanf("%d", &userInput.ademco_id);
					clear_stdin();
				} while (ret != 1);
				{
					std::lock_guard<std::mutex> lg(mutex);
					commands.push_back(COMMAND_F_QUERY_TIMER);
					threads_to_handled_event = thread_count;
				}
				fire_command();
				break;
			}


		case 'I':
			{
				std::vector<Client> copiedClients;
				for (auto context : worker_thread_contexts) {
					std::lock_guard<std::mutex> lg(context->mutex);
					for (const auto& client : context->clients) {
						copiedClients.push_back(client.second);
					}
				}
				printf("Total connnected %zd clients:\n", copiedClients.size());

				for (const auto& client : copiedClients) {
					client.printInfo();
				}
			}
			break;

		case 'P':
			disable_data_print = !disable_data_print;
			printf("Data print is %s\n", disable_data_print ? "Off" : "On");
			break;

		case 'Q':
			{
				timeval tv{ 0, 1000 };
				event_base_loopexit(listen_thread_evbase, &tv);
				listener_thread.join();
				printf("listener thread exited\n");
				for (int i = 0; i < thread_count; i++) {
					event_base_loopexit(worker_thread_contexts[i]->base, &tv);
					worker_threads[i].join();
					printf("worker_thread #%d exited\n", i);
				}
				running = false;
				break;
			}
			
		case '\r':
		case '\n':
			break;

		default:
			clear_stdin();
			printf("Invalid command\n");
			op_usage(); 
			break;
		}		
	}

	printf("Bye\n");

	return 0;
}

void Client::printInfo() const
{
	if (type == EVENT_I_AM_3_SECTION_MACHINE) {
		printf("  fd=#%d acct=%s ademco_id=%d type=%s\n",
			   fd, acct.data(), ademco_id,
			   machineTypeToString(machineTypeFromAdemcoEvent((ADEMCO_EVENT)type)));
		printf("    status1: %d %s    status2: %d %s    status3: %d %s\n",
			   (int)status1, machineStatusToString(status1),
			   (int)status2, machineStatusToString(status2),
			   (int)status3, machineStatusToString(status3));
	} else {
		printf("  fd=#%d acct=%s ademco_id=%d type=%s status=%d %s\n",
			   fd, acct.data(), ademco_id,
			   machineTypeToString(machineTypeFromAdemcoEvent((ADEMCO_EVENT)type)),
			   (int)status, machineStatusToString(status));
	}
	for (const auto& zp : zones) {
		char buf[512];
		snprintf(buf, sizeof(buf), getZoneFormatString(machineTypeFromAdemcoEvent((ADEMCO_EVENT)type)), zp.first);
		printf("    Zone:");
		printf("%s", buf);
		printf("  Prop:0x%02X %s     \tTamper Enabled:%s\n",
			   zp.second.prop, zonePropertyToStringEn(zp.second.prop),
			   zp.second.tamper_enabled ? "true" : "false");
	}
	for (int i = 0; i < 2; i++) {
		if (com::isValidMachineTimer(timer.timer[i])) {
			printf("    Machine Timer#%d: Arm at %02d:%02d, Disarm at %02d:%02d\n", i + 1,
				   timer.timer[i].armAt.hour, timer.timer[i].armAt.minute,
				   timer.timer[i].disarmAt.hour, timer.timer[i].disarmAt.minute);
		} else {
			printf("    Machine Timer#%d: Not Set\n", i + 1);
		}
	}
}
