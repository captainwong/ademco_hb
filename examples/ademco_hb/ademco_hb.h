#pragma once

#ifdef ADEMCOHB_EXPORTS 
#define DLL_FUNC extern "C" _declspec(dllexport)
#else
#define DLL_FUNC extern "C" _declspec(dllimport)
#endif

/*
* @brief 解析网络数据
* @param[in] pack 网络数据流
* @param[in] pack_len 网络数据流有效长度
* @param[in|out] cb_commited 可以正确解析的包长（单次函数调用最多解析一包）
* @return  0 解析到正确、完整的一包；1 解析到正确、不完整的一包，需要缓存数据，下次继续处理；2 解析到错误的数据，需要清空缓冲区
*/
DLL_FUNC int parse(const char* pack, int pack_len, int* cb_commited);

/*
* @brief 将远程控制命令打包成网络传输数据
* @param[in|out] buff 缓冲区
* @param[in] buff_len 缓冲区长度
* @param[in] seq 序号
* @param[in] ademco_id 安定宝ID
* @param[in] ademco_event 安定宝事件码
* @param[in] zone 防区号
* @param[in] gg 分防区号
* @return 大于0 成功，返回值代表包长；0 buff空间不足
*/
DLL_FUNC int pack(char* buff, int buff_len, int seq, int ademco_id, int ademco_event, int zone, int gg);

/*
* @brief 将远程控制命令打包成网络传输数据
* @param[in|out] buff 缓冲区
* @param[in] buff_len 缓冲区长度
* @param[in] seq 序号
* @param[in] acct 主机账号
* @param[in] ademco_id 安定宝ID
* @param[in] ademco_event 安定宝事件码
* @param[in] zone 防区号
* @param[in] gg 分防区号
* @return 大于0 成功，返回值代表包长；0 buff空间不足
*/
DLL_FUNC int pack2(char* buff, int buff_len, int seq, const char* acct, int ademco_id, int ademco_event, int zone, int gg);

/*
* @brief 打包ACK
* @param[in|out] buff 缓冲区
* @param[in] buff_len 缓冲区长度
* @param[in] seq 序号
* @param[in] ademco_id 安定宝ID
* @return 大于0 成功，返回值代表包长；0 buff空间不足
*/
DLL_FUNC int pack_ack(char* buff, int buff_len, int seq, int ademco_id);


/*
* @brief 打包ACK
* @param[in|out] buff 缓冲区
* @param[in] buff_len 缓冲区长度
* @param[in] seq 序号
* @param[in] acct 主机账号
* @return 大于0 成功，返回值代表包长；0 buff空间不足
*/
DLL_FUNC int pack_ack2(char* buff, int buff_len, int seq, const char* acct);
