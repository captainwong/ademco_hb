#pragma once

#include "ademco_event.h"
#include <assert.h>
#include <string.h> // memset
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>

#if defined(DISABLE_JLIB_LOG2) && !defined(JLIB_DISABLE_LOG)
#define JLIB_DISABLE_LOG
#endif

#ifndef JLIB_DISABLE_LOG
#include <jlib/log2.h>
#else // JLIB_DISABLE_LOG
#define init_logger(...)
#define JLOG_DBUG(...)
#define JLOG_INFO(...)
#define JLOG_WARN(...)
#define JLOG_ERRO(...)
#define JLOG_CRTC(...)
#define JLOG_ALL(...)

class range_log {
public:
	range_log() {}
	range_log(const char*) {}
};

#define AUTO_LOG_FUNCTION

#define dump_hex(...)
#define dump_asc(...)
#define JLOG_HEX(...)
#define JLOG_ASC(...)
#endif // JLIB_DISABLE_LOG

namespace ademco
{

// for crossplatform
#ifndef __GNUG__
static inline int localtime_r(time_t* t, struct tm* tm_time) {
	return localtime_s(tm_time, t);
}

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#endif // __GNUG__


namespace detail
{

/**
* @brief 计算一个字节（char）的crc16值
* @param c 要计算crc16的字节
* @param crc 初始crc值
* @return crc16
*/
static unsigned short CalculateCRC_char(char c, unsigned short crc)
{
	static constexpr unsigned short crcTable[] = {
		/* DEFINE THE FIRST ORDER POLYINOMIAL TABLE */
		0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
		0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
		0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
		0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
		0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
		0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
		0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
		0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
		0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
		0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
		0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
		0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
		0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
		0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
		0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
		0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
		0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
		0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
		0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
		0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
		0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
		0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
		0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
		0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
		0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
		0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
		0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
		0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
		0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
		0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
		0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
		0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040,
	};

	unsigned short CRC = crc;
	auto tmp = static_cast<unsigned short>(CRC >> 8) ^ (crcTable[static_cast<unsigned char>(c) ^ static_cast<unsigned char>(CRC & 0xFF)]);
	CRC = static_cast<unsigned short>(tmp);
	return CRC;
}

/**
* @brief 计算一段数据包的crc16值
* @param buff 要计算crc16的数据包指针
* @param len 要计算crc16的数据包长度
* @param crc 初始crc值
* @return crc16
*/
static unsigned short CalculateCRC(const char* buff, size_t len, unsigned short crc = 0)
{
	unsigned short CRC = crc;
	for (size_t i = 0; i < len; ++i) {
		CRC = CalculateCRC_char(buff[i], CRC);
	}

	return CRC;
}

/**
* @brief 计算一段数据包的crc16值
* @param a 要计算crc16的数据包
* @param crc 初始crc值
* @return crc16
*/
static unsigned short CalculateCRC(const std::vector<char>& a, unsigned short crc)
{
	unsigned short CRC = crc;
	for (auto c : a) {
		CRC = CalculateCRC_char(c, CRC);
	}
	return CRC;
}

/**
* @brief 将'0' ~ '9', 'a' ~ 'f', 'A' ~ 'F' 的字节转换为十进制数字，范围0~15
* @param hex 要计算的字节
* @return 字节表示的十进制数字
* @note 示例：'f' 返回十进制数字15
*/
static char HexChar2Dec(char hex)
{
	if (hex >= '0' && hex <= '9')
		return static_cast<char>(hex - '0');
	else if (hex >= 'A' && hex <= 'F')
		return static_cast<char>(hex - 'A' + 10);
	else if (hex >= 'a' && hex <= 'f')
		return static_cast<char>(hex - 'a' + 10);
	else if (hex == 'l' || hex == 'L')
		return 0;
	else {
		JLOG_WARN("HexChar2Dec: not a hex char. ({}) (0x{:02X})", (int)hex, (unsigned char)hex);
		//assert(0);
		return 0;
	}
}

/**
* @brief 将十六进制数字的字符串转换为十进制数字
* @param hex 要计算的字符串地址，单个字节范围为'0' ~ '9', 'a' ~ 'f', 'A' ~ 'F'
* @param len 要计算的字符串长度
* @return 十进制数字
* @note 示例: 字符串"FF"返回十进制数字255
* @note 仅用于安定宝数据包内的16进制字符串解析，长度较小，返回的十进制数字不能超过size_t范围
*/
static size_t HexCharArrayToDec(const char* hex, size_t len)
{
	size_t dec = 0;
	for (size_t i = 0; i < len; i++) {
		dec = dec * 0x10 + HexChar2Dec(hex[i]);
	}
	return dec;
}

/**
* @brief 将十进制数字字符串转换为十进制数字
* @param str 要计算的字符串地址，单个字节范围为'0' ~ '9'
* @param len 要计算的字符串长度
* @return 十进制数字
* @note 示例: 字符串"99"返回十进制数字99
* @note 仅用于安定宝数据包内的16进制字符串解析，长度较小，返回的十进制数字不能超过size_t范围
*/
static size_t NumCharArray2Dec(const char* str, size_t str_len)
{
	size_t dec = 0;
	for (size_t i = 0; i < str_len; i++) {
		dec = dec * 10 + HexChar2Dec(str[i]);
	}
	return dec;
}

/**
* @brief 将范围为0~15的十进制数字字节转换为16进制字节
* @param d 要计算的10进制数字，范围0~15
* @return 十六进制字节，大写
* @note 示例: 数字10 返回字节 'A'
*/
static char Dec2Hex(char d)
{
	if (0 <= d && d <= 9) {
		return static_cast<char>((unsigned char)d + (unsigned char)'0');
	} else if (0x0A <= d && d <= 0x0F) {
		return char(d - 0x0A + (char)'A');
	} else {
		JLOG_ERRO("Dec2Hex: not a 0-f value. ({}) (0x{:02X})", (int)d, (unsigned char)d);
		assert(0);
		return '0';
	}
}

/**
* @brief 将一串以高低字节表示的十六进制数组转换为字符串
* @dst 直接将结果写入到dst
* @param hex 十六进制字符串地址
* @param len 十六进制字符串长度
* @mask 掩码 扫描到掩码后立即停止
* @note 示例：输入数组：0x18 0x24 0x08 0x88 0x10 0x1f，以0x0F为掩码，输出字符串"18240888101"
*/
static const char* HexCharArrayToStr(char* dst, const char* hex, size_t len, unsigned char mask = (unsigned char)0x0F)
{
	//AUTO_LOG_FUNCTION;
	//jlib::dump_hex(hex, len, spdlog::level::debug);
	memset(dst, 0, len * 2);
	char tmp[8] = { 0 };
	unsigned char high = 0, low = 0;

	for (size_t i = 0; i < len; i++) {
		high = (unsigned char)((hex[i] >> 4) & mask);
		if (high >= mask) { break; }
		snprintf(tmp, sizeof(tmp), "%d", high);

		strncat(dst, tmp, sizeof(tmp));

		low = (unsigned char)(hex[i] & mask);
		if (low >= mask) { break; }

		snprintf(tmp, sizeof(tmp), "%d", low);
		strncat(dst, tmp, sizeof(tmp));
	}
	return dst;
}

/**
* @brief 将数字转换为十六进制数组
* @param dec 10进制数字
* @param hex 将结果写入到此地址，定长4字节
* @param bMax0FFF 输入的数字是否有最大值 0x0FFF
* @note 示例：输入 256， 其hex为 0x0100, 输出 "0100"
*/
static void Dec2HexCharArray_4(size_t dec, char* hex, bool bMax0FFF = true)
{
	if (dec < 0) {
		JLOG_ERRO("Dec2HexCharArray_4: 0LLL can't be negative.");
		return;
	} else if (dec == 0) {
		memset(hex, '0', 4); return;
	} else if (bMax0FFF && dec > 0x0FFF) {
		JLOG_WARN("Dec2HexCharArray_4: 0LLL {} is bigger than 0x0fff.", dec);
		memcpy(hex, "0FFF", 4); return;
	} else {
		char tmp[5] = { 0 };
		snprintf(tmp, sizeof(tmp), "%04X", static_cast<unsigned int>(dec & 0xFFFF));
		memcpy(hex, tmp, 4);
	}
}

/**
* @brief 将字符串表示的数字转换为以高低字节表示的十六进制数组
* @param str 字符串，字面值为 10 进制数字
* @param hexarr 将结果写入到此地址，长度为字符串长度的 1/2
* @param max_hex_len 结果数组是否有最大长度 9
* @note 限制了字符串最大长度为 32，因此 max_hex_len 最大为 16
* @note 当字符串转换后长度若不足max_hex_len则补 0x0F
* @note 示例：输入 "18240888101"，max_hex_len为9，输出 0x18 0x24 0x08 0x88 0x10 0x1f 0xff 0xff 0xff
*/
static void NumStr2HexCharArray_N(const char* str, char* hexarr, size_t max_hex_len = 9)
{
	//AUTO_LOG_FUNCTION;
	if (str == nullptr) {
		JLOG_ERRO("NumStr2HexCharArray_N: memory access denied.");
		return;
	}

	auto len = strlen(str);
	max_hex_len = std::min(max_hex_len, size_t(16));
	if (len > max_hex_len * 2) {
		JLOG_ERRO("NumStr2HexCharArray_N: length too long.");
		return;
	}

	size_t i = 0;
	for (i = 0; i < len; i++) {
		if (!isdigit(str[i])) {
			JLOG_ERRO("NumStr2HexCharArray_N: not all character is digit.");
			return;
		}
	}

	const size_t full_str_len = max_hex_len * 2;
	char full_str[32] = { 0 };
	memset(full_str, 0, sizeof(full_str));
	strncpy(full_str, str, len);
	full_str[len] = '\0';
	while (strlen(full_str) < full_str_len) {
		strncat(full_str, "f", 1);
	}

	for (i = 0; i < max_hex_len; i++) {
		char ch = static_cast<char>(HexChar2Dec(full_str[i * 2]) & 0x0F);
		ch = static_cast<char>(ch << 4);
		ch = static_cast<char>(ch | (HexChar2Dec(full_str[i * 2 + 1]) & 0x0F));
		hexarr[i] = ch;
	}
}

/**
* @brief 将字符串形式的高低字节转换为真正的高低字节数组
* @param dst 结果地址，调用方需确保 dst 可写长度至少为 len / 2
* @param src 字符串形式的高低字节， 如 "ffffabcd1234", 输出 0xff 0xff 0xab 0xcd 0x12 0x34
* @param len 字符串长度
* @note 起因是钱工的GPRS模块无法传输hex，只能传输ascii字节
*/
static void ConvertHiLoAsciiToHexCharArray(char* dst, const char* src, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		if (!isxdigit((int)(unsigned char)src[i])) {
			JLOG_ERRO("NumStr2HexCharArray_N: not all character is xdigit.");
			return;
		}
	}

	const size_t dst_len = len / 2;
	memset(dst, 0, dst_len);

	for (size_t i = 0; i < dst_len; i++) {
		char ch = HexChar2Dec(src[i * 2]) & 0x0F;
		ch = static_cast<char>(ch << 4);
		ch = static_cast<char>(ch | (HexChar2Dec(src[i * 2 + 1]) & 0x0F));
		dst[i] = ch;
	}
}

enum class ToStringOption
{
	//! 所有字节按照16进制表示，如 '0' 表示为 "\x30"
	ALL_CHAR_AS_HEX,
	//! 如果字节是isalnum的直接输出，否则输出16进制格式
	TRY_IS_ALNUM_FIRST,
};

/**
* @brief 常规数组转字符串
* @param data 数据
* @param len 数据长度
* @param option 选项
* @param show_x_for_hex 为 hex 显示 \x
* @param show_space_between_hex 在 hex 之间插入空格
* @return std::string
*/
static std::string toString(const char* data, size_t len, // 数据
							ToStringOption option = ToStringOption::TRY_IS_ALNUM_FIRST, // 选项
							bool show_x_for_hex = true,	// 为 hex 显示 \x
							bool show_space_between_hex = false) // 在 hex 之间插入空格
{
	std::string str;
	for (size_t i = 0; i < len; i++) {
		auto c = data[i];
		if (option == ToStringOption::TRY_IS_ALNUM_FIRST && std::isalnum(static_cast<unsigned char>(c))) {
			str.push_back(c);
		} else {
			if (show_x_for_hex) { str += "\\x"; }
			str.push_back(ademco::detail::Dec2Hex((c >> 4) & 0x0F));
			str.push_back(ademco::detail::Dec2Hex(c & 0x0F));
			if (show_space_between_hex) { str.push_back(' '); }
		}
	}
	return str;
}

/**
* @brief 常规数组转字符串
* @param data 数据
* @param option 选项
* @param show_x_for_hex 为 hex 显示 \x
* @param show_space_between_hex 在 hex 之间插入空格
* @return std::string
*/
static std::string toString(const std::vector<char>& data, // 数据
							ToStringOption option = ToStringOption::TRY_IS_ALNUM_FIRST, // 选项
							bool show_x_for_hex = true,	// 为 hex 显示 \x
							bool show_space_between_hex = false) // 在 hex 之间插入空格
{
	return toString(data.data(), data.size(), option, show_x_for_hex, show_space_between_hex);
}

} // namespace detail
} // namespace ademco
