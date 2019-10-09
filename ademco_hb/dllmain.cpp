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

int parse(const char* pack, size_t pack_len, size_t& cb_commited)
{
	ademco::AdemcoPacket ap;
	auto result = ap.parse(pack, pack_len, cb_commited);
	return static_cast<int>(result);
}
