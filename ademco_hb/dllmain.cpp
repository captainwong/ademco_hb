// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "ademco_hb.h"
#define DISABLE_JLIB_LOG2
#include <ademco_packet.h>


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

int parse(const char* pack, int pack_len, int* cb_commited)
{
	ademco::AdemcoPacket ap;
	size_t commited = 0;
	auto result = ap.parse(pack, static_cast<size_t>(pack_len), commited);
	*cb_commited = static_cast<int>(commited);
	return static_cast<int>(result);
}

int pack(char* buff, int buff_len, int seq, int ademco_id, int ademco_event, int zone, int gg)
{
    ademco::AdemcoPacket ap;
    auto res = ap.make_hb(buff, static_cast<size_t>(buff_len), static_cast<uint16_t>(seq), 
        nullptr, static_cast<size_t>(ademco_id), static_cast<unsigned char>(gg), 
        static_cast<ademco::ADEMCO_EVENT>(ademco_event), static_cast<size_t>(zone));
    return static_cast<int>(res);
}

int pack_ack(char* buff, int buff_len, int seq, int ademco_id)
{
    ademco::AdemcoPacket ap;
    auto res = ap.make_ack(buff, static_cast<size_t>(buff_len), static_cast<uint16_t>(seq),
        nullptr, static_cast<size_t>(ademco_id));
    return static_cast<int>(res);
}
