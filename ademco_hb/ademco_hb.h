#pragma once

#ifdef ADEMCOHB_EXPORTS 
#define DLL_FUNC extern "C" _declspec(dllexport)
#else
#define DLL_FUNC extern "C" _declspec(dllimport)
#endif

//! 返回值说明
//! 0 解析到正确、完整的一包,
//! 1 解析到正确、不完整的一包，需要缓存数据，下次继续处理
//! 2 解析到错误的数据，需要清空缓冲区
DLL_FUNC
int parse(const char* pack, size_t pack_len, size_t& cb_commited);

