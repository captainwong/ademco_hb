#pragma once

#include "ademco_event.h"
#include "ademco_detail.h"
#include <vector>
#include <string>
#include <memory>
#include <iterator>
#include <algorithm>

namespace ademco
{

//! 解析数据包的返回值
enum class ParseResult
{
	//! 解析到正确、完整的一包
	RESULT_OK,
	//! 解析到正确、不完整的一包，需要缓存数据，下次继续处理
	RESULT_NOT_ENOUGH,
	//! 解析到错误的数据，需要清空缓冲区
	RESULT_DATA_ERROR,
};

//! 安定宝数据包 data 段
struct AdemcoData
{
	//! 数据
	std::vector<char> data_ = {};
	//! 安定宝ID
	AdemcoId ademco_id_ = 0;
	//! 安定宝事件码
	ADEMCO_EVENT ademco_event_ = ADEMCO_EVENT::EVENT_INVALID_EVENT;
	//! gg
	AdemcoGG gg_ = 0;
	//! zone
	AdemcoZone zone_ = 0;

	AdemcoData() { make(); }

	void clear() {
		data_.clear();
		ademco_id_ = 0;
		ademco_event_ = EVENT_INVALID_EVENT;
		gg_ = 0;
		zone_ = 0;
	}

	bool valid() const { return data_.size() >= 2; }
	size_t size() const { return data_.size(); }

	std::string toString() const {
		std::string str;
		std::copy(data_.begin(), data_.end(), std::back_inserter(str));
		return str;
	}

	void assignAdemcoId(AdemcoId ademco_id) {
		ademco_id %= AdemcoIdSentinel;
		char aid[7] = { 0 };
		snprintf(aid, sizeof(aid), "%06X", static_cast<int>(ademco_id));
		std::copy(aid, aid + 6, std::back_inserter(data_));
		ademco_id_ = ademco_id;
	}

	void assignAdemcoEvent(ADEMCO_EVENT ademco_event) {
		ademco_event = (ADEMCO_EVENT)(ademco_event % AdemcoEventSentinel);
		char aev[5] = { 0 };
		snprintf(aev, sizeof(aev), "%04d", static_cast<int>(ademco_event));
		std::copy(aev, aev + 4, std::back_inserter(data_));
		ademco_event_ = ademco_event;
	}

	void assignGG(AdemcoGG gg) {
		if (gg == 0xEE) {
			data_.push_back('E');
			data_.push_back('E');
		} else if (gg == 0xCC) {
			data_.push_back('C');
			data_.push_back('C');
		} else {
			data_.push_back(static_cast<char>(((gg / 10) & 0x0F) + '0'));
			data_.push_back(static_cast<char>(((gg % 10) & 0x0F) + '0'));
		}

		gg_ = static_cast<unsigned char>(gg);
	}

	void assignZone(AdemcoZone zone) {
		zone %= ZoneSentinel;
		char z[4] = { 0 };
		snprintf(z, sizeof(z), "%03d", static_cast<int>(zone));
		std::copy(z, z + 3, std::back_inserter(data_));
		zone_ = zone;
	}

	//! 生成可用于网络传输的数据
	void make(AdemcoId ademco_id, AdemcoGG gg, ADEMCO_EVENT ademco_event, AdemcoZone zone) {
		if (EVENT_INVALID_EVENT == ademco_event) { make(); return; }
		clear();

		data_.reserve(21); // [#000000|1400 00 000]
		data_.push_back('[');
		data_.push_back('#');
		assignAdemcoId(ademco_id);
		data_.push_back('|');
		assignAdemcoEvent(ademco_event);
		data_.push_back(' ');
		assignGG(gg);
		data_.push_back(' ');
		assignZone(zone);
		data_.push_back(']');

		assert(data_.size() == 21);
	}

	//! 生成可用于网络传输的数据
	void make(const char* acct, AdemcoGG gg, ADEMCO_EVENT ademco_event, AdemcoZone zone) {
		clear();

		data_.clear();
		data_.push_back('[');
		data_.push_back('#');
		std::copy(acct, acct + strlen(acct), std::back_inserter(data_));
		data_.push_back('|');

		assignAdemcoEvent(ademco_event);
		data_.push_back(' ');
		assignGG(gg);
		data_.push_back(' ');
		assignZone(zone);
		data_.push_back(']');
	}

	//! 生成可用于网络传输的空白data数据
	void make() {
		clear();
		data_.push_back('[');
		data_.push_back(']');
	}

	// parser
	bool parse(const char* pack, size_t pack_len) {
		//AUTO_LOG_FUNCTION;
		clear();
		const char* p = pack;
		do {
			if (*p++ != '[')
				break;

			size_t acct_len = 4;
			if (pack_len == 2 && *p == ']') {
				make();
				return true;
				//                 [   #  acct |   mt  s      q event  s   gg  s   zone ]   // 19
			} else if (pack_len == 1 + 1 + 4 + 1 + /*2 + 1 + */1 + 3 + 1 + 2 + 1 + 3 + 1) {
				acct_len = 4;
			} else if (pack_len == 1 + 1 + 6 + 1 + /*2 + 1 + */1 + 3 + 1 + 2 + 1 + 3 + 1) { // 21)
				acct_len = 6;
			} else if (pack_len == 1 + 1 + 14 + 1 + /*2 + 1 + */1 + 3 + 1 + 2 + 1 + 3 + 1) { // 29) // just ignore it
				acct_len = 14;
			} else if (pack_len > 15) {
				//break;
				acct_len = pack_len - 15; // just ignore it
			}

			if (*p++ != '#')
				break;
			if (acct_len == 4) {
				if (*(p + 4) != '|')
					break;
				ademco_id_ = static_cast<AdemcoId>(detail::HexCharArrayToDec(p, 4) % AdemcoIdSentinel);
				p += 5;
			} else if (acct_len == 6) {
				if (*(p + 6) != '|')
					break;
				ademco_id_ = static_cast<AdemcoId>(detail::HexCharArrayToDec(p, 6) % AdemcoIdSentinel);
				p += 7;
			} else if (acct_len == 14) { // just ignore it
				if (*(p + 14) != '|')
					break;
				ademco_id_ = 0;
				p += 15;
			} else { // just ignore it
				ademco_id_ = 0;
				p += acct_len + 1;
			}

			ademco_event_ = static_cast<ADEMCO_EVENT>(detail::NumCharArray2Dec(p, 4) % AdemcoEventSentinel);
			p += 4;
			if (*p++ != ' ')
				break;

			if (*p == 'E' && *(p + 1) == 'E') {
				gg_ = 0xEE;
			} else if (*p == 'C' && *(p + 1) == 'C') {
				gg_ = 0xCC;
			} else {
				gg_ = static_cast<AdemcoGG>((*p - '0') * 10 + (*(p + 1) - '0'));
			}

			p += 2;
			if (*p++ != ' ')
				break;

			zone_ = static_cast<AdemcoZone>(detail::NumCharArray2Dec(p, 3) % ZoneSentinel);
			std::copy(pack, pack + pack_len, std::back_inserter(data_));
			return true;
		} while (0);

		return false;
	}

	bool operator==(const AdemcoData& rhs) const {
		return data_ == rhs.data_
			&& ademco_id_ == rhs.ademco_id_
			&& ademco_event_ == rhs.ademco_event_
			&& gg_ == rhs.gg_
			&& zone_ == rhs.zone_;
	}
}; // AdemcoData

// 2016-11-4 16:47:51 for compatibility of congwin fe100 protocal
struct CongwinFe100Packet
{
	static constexpr size_t length = 31;
	char data_[length + 1] = { 0 };

	bool fromAdemcoData(const AdemcoData* data) {
		assert(data && data->valid());
		if (!data || !data->valid()) {
			return false;
		}

		auto acct = data->ademco_id_;
		auto evnt = data->ademco_event_;
		auto zone = data->zone_;

		int ndx = 0;
		data_[ndx++] = '\n'; // LF
		data_[ndx++] = ' ';

		data_[ndx++] = '0'; // RG
		data_[ndx++] = '0';
		data_[ndx++] = ' ';

		ndx += snprintf(data_ + ndx, sizeof(data_) - ndx, "%08d", static_cast<int>(acct)); // acct

		data_[ndx++] = ' ';
		data_[ndx++] = '1'; // 18
		data_[ndx++] = '8';
		data_[ndx++] = ' ';

		// E for open, R for close
		if (evnt / 1000 == 1 || evnt == 3456) {
			data_[ndx++] = 'E'; 
		} else {			
			data_[ndx++] = 'R';
		}

		// 2020年3月11日16:37:18 修改
		// 丛文使用 3B0 表示主机断线，3B2 表示主机上线
		if (evnt == ADEMCO_EVENT::EVENT_OFFLINE) {
			data_[ndx++] = '3';
			data_[ndx++] = 'B';			
			data_[ndx++] = '0';
		} else if (evnt == ADEMCO_EVENT::EVENT_ONLINE) {
			data_[ndx++] = '3';
			data_[ndx++] = 'B';			
			data_[ndx++] = '2';
		} else {
			ndx += snprintf(data_ + ndx, sizeof(data_) - ndx, "%03d", static_cast<int>(evnt % 1000)); // event
		}
		
		data_[ndx++] = ' ';
		data_[ndx++] = '0'; // gg is always 00
		data_[ndx++] = '0';
		data_[ndx++] = ' ';

		// 'C' for zone, 'U' for user
		if (isMachineStatusEvent(evnt)) {
			data_[ndx++] = 'U'; // U
		} else {
			data_[ndx++] = 'C'; // C
		}

		ndx += sprintf(data_ + ndx, "%03d", static_cast<int>(zone % ZoneSentinel));

		data_[ndx++] = ' ';
		data_[ndx++] = '\r';

		return true;
	}
}; // CongwinFe100Packet


//! 安定宝事件 xdata 段
struct XData
{
	enum class LengthFormat {
		TWO_HEX, // 以两个hex字节表示长度，如 \x00, \x06
		FOUR_DECIMAL, // 以4个ASCII字符表示长度，如 "0008"
	};

	enum class DataFormat {
		AS_IS, //! 原样打包
		TO_ASCII, //! 一个字节的HEX值转换为两个ASCII字符，如 6 转为 "06", '6' 转为 "36"
	};

	LengthFormat lengthFormat_ = LengthFormat::TWO_HEX;
	//! 刨去[ len ]后，真实有效数据的长度，用来程序调用
	std::vector<char> data_ = {};
	//! 包含 [ len ... ] 的全部数据长度，可以用来网络发送
	std::vector<char> rawData_ = {};

	XData() {}

	XData(const XData& rhs) {
		lengthFormat_ = rhs.lengthFormat_;
		data_ = rhs.data_;
		rawData_ = rhs.rawData_;
	}

	size_t size() const { return data_.size(); }
	size_t rawSize() const { return rawData_.size(); }
	bool valid() const {
		if (data_.empty() || rawData_.empty()) { return false; }
		size_t len = 0;
		if (lengthFormat_ == LengthFormat::TWO_HEX) {
			if (rawData_.size() < 4) { return false; }
			len = (rawData_[1] << 8) | rawData_[2];
			if (len != data_.size() || (data_.size() + 4 != rawData_.size())) { return false; }
		} else {
			if (rawData_.size() < 6) { return false; }
			len = ademco::detail::HexCharArrayToDec(rawData_.data() + 1, 4);
			if ((len != data_.size()) || (data_.size() + 6 != rawData_.size())) { return false; }
		}
		return true;
	}

	bool operator==(const XData& rhs) const {
		return lengthFormat_ == rhs.lengthFormat_
			&& data_ == rhs.data_;
	}

	bool convert(LengthFormat lengthFormat = LengthFormat::TWO_HEX) {
		if (!valid()) { return false; }
		if (lengthFormat_ == lengthFormat) { return true; }
		lengthFormat_ = lengthFormat;
		if (lengthFormat_ == LengthFormat::TWO_HEX) {
			std::vector<char> tmp(data_.size() / 2);
			detail::ConvertHiLoAsciiToHexCharArray(&tmp[0], data_.data(), data_.size());
			data_ = tmp;
			rawData_.resize(data_.size() + 4);
			rawData_[0] = '[';
			rawData_[1] = (char)((data_.size() >> 8) & 0xFF);
			rawData_[2] = (char)(data_.size() & 0xFF);
			memcpy(&rawData_[3], data_.data(), data_.size());
			rawData_.back() = ']';
		} else {
			auto tmp = detail::toString(data_, detail::ToStringOption::ALL_CHAR_AS_HEX, false, false);
			data_.clear();
			std::copy(tmp.begin(), tmp.end(), std::back_inserter(data_));
			rawData_.resize(data_.size() + 6);
			rawData_[0] = '[';
			detail::Dec2HexCharArray_4(data_.size(), &rawData_[1], false);
			memcpy(&rawData_[5], data_.data(), data_.size());
			rawData_.back() = ']';
		}
		return true;
	}
};

typedef std::shared_ptr<XData> XDataPtr;

inline bool operator==(const XDataPtr& lhs, const XDataPtr& rhs)
{
	if (lhs && rhs) {
		return *lhs == *rhs;
	} else if (!lhs && !rhs) {
		return true;
	} else {
		return false;
	}
}

//! 生成xdata
inline XDataPtr makeXData(const std::vector<char>& payload,
						  XData::LengthFormat lengthFormat = XData::LengthFormat::TWO_HEX, 
						  XData::DataFormat dataFormat = XData::DataFormat::AS_IS)
{
	if (payload.empty()) { return nullptr; }
	auto xdata = std::make_shared<XData>();
	xdata->lengthFormat_ = lengthFormat;
	if (dataFormat == XData::DataFormat::TO_ASCII) {
		auto tmp = detail::toString(payload, detail::ToStringOption::ALL_CHAR_AS_HEX, false, false);
		std::copy(tmp.begin(), tmp.end(), std::back_inserter(xdata->data_));
	} else {
		xdata->data_ = payload;
	}

	switch (lengthFormat)
	{
	case XData::LengthFormat::FOUR_DECIMAL:
	{
		xdata->rawData_.resize(1 + 4 + xdata->data_.size() + 1);
		xdata->rawData_[0] = '[';
		detail::Dec2HexCharArray_4(xdata->data_.size(), &xdata->rawData_[1], false);
		memcpy(&xdata->rawData_[5], xdata->data_.data(), xdata->data_.size());
		xdata->rawData_.back() = ']';
		break;
	}
	case XData::LengthFormat::TWO_HEX:
	default:
	{
		xdata->rawData_.resize(1 + 2 + xdata->data_.size() + 1);
		xdata->rawData_[0] = '[';
		xdata->rawData_[1] = static_cast<char>((xdata->data_.size() << 8) & 0xFF);
		xdata->rawData_[2] = static_cast<char>(xdata->data_.size() & 0xFF);
		memcpy(&xdata->rawData_[3], xdata->data_.data(), xdata->data_.size());
		xdata->rawData_.back() = ']';
	}
	break;
	}

	return xdata;
}

inline XDataPtr makeXData(const char* pack, size_t len,
						  XData::LengthFormat lengthFormat = XData::LengthFormat::TWO_HEX,
						  XData::DataFormat dataFormat = XData::DataFormat::AS_IS) {
	if (len == 0) { return nullptr; }
	std::vector<char> data(pack, pack + len);
	return makeXData(data, lengthFormat, dataFormat);
}

inline XDataPtr makeXData(const unsigned char* pack, size_t len,
						  XData::LengthFormat lengthFormat = XData::LengthFormat::TWO_HEX,
						  XData::DataFormat dataFormat = XData::DataFormat::AS_IS) {
	if (len == 0) { return nullptr; }
	std::vector<char> data((const char*)pack, (const char*)pack + len);
	return makeXData(data, lengthFormat, dataFormat);
}

inline XDataPtr parseXData(const char* pack, const char* pack_end)
{
	XDataPtr xdata;

	do {
		auto p = pack;
		if (*p++ != '[') { break; } // skip [ 

		size_t xdata_len = 0;
		XData::LengthFormat format = XData::LengthFormat::FOUR_DECIMAL;

		for (size_t i = 0; i < 4; i++) {
			if (!isalpha(*(p + i)) && !isdigit(*(p + i))) {
				format = XData::LengthFormat::TWO_HEX;
				break;
			}
		}

		if (format == XData::LengthFormat::FOUR_DECIMAL) {
			// special condition, use 4 char to represent length.
			xdata_len = detail::HexCharArrayToDec(p, 4);
			p += 4; // skip len
		} else {
			// normal condition, use 2 hex to represent length
			xdata_len = (*p * 256 + *(p + 1));
			p += 2; // skip len
		}

		const char* xdata_pos = p;
		p += xdata_len;
		if (p >= pack_end || *p++ != ']') { // skip ]
			JLOG_ERRO("AdemcoPacket::Parse parse xdata failed!");
			break;
		}

		xdata = std::make_shared<XData>();
		xdata->lengthFormat_ = format;
		// copy raw data
		std::copy(pack, p, std::back_inserter(xdata->rawData_));
		// copy payload data
		std::copy(xdata_pos, xdata_pos + xdata_len, std::back_inserter(xdata->data_));
	} while (0);

	return xdata;
} // parseXData

inline std::string xdataToString(const XDataPtr& xdata, detail::ToStringOption option = detail::ToStringOption::TRY_IS_ALNUM_FIRST) {
	std::string str;
	if (!xdata) { return str; }

	str += "[";

	// length
	if (xdata->lengthFormat_ == XData::LengthFormat::TWO_HEX) {
		str += "\\x";
		auto hi = (xdata->size() >> 8) & 0xFF;
		str.push_back(ademco::detail::Dec2Hex((hi >> 4) & 0x0F));
		str.push_back(ademco::detail::Dec2Hex(hi & 0x0F));
		str += "\\x";
		auto lo = xdata->size() & 0xFF;
		str.push_back(ademco::detail::Dec2Hex((lo >> 4) & 0x0F));
		str.push_back(ademco::detail::Dec2Hex(lo & 0x0F));
	} else {
		std::copy(&xdata->rawData_[1], &xdata->rawData_[5], std::back_inserter(str));
	}

	// data
	str += detail::toString(xdata->data_, option, true, false);

	str += "]";

	return str;
}

struct AdemcoTimeStamp
{
	// timestamp, ademco format is _23:59:59,12-31-2000, so its len is 20.
	static constexpr size_t length = 20;
	char data_[length + 1] = { 0 };
	time_t time_ = 0;

	AdemcoTimeStamp() { make(); }
	AdemcoTimeStamp(const AdemcoTimeStamp& rhs) {
		memcpy(data_, rhs.data_, length + 1);
		time_ = rhs.time_;
	}
	AdemcoTimeStamp& operator=(const AdemcoTimeStamp& rhs) {
		memcpy(data_, rhs.data_, length + 1);
		time_ = rhs.time_;
		return *this;
	}

	void make() {
		time_ = time(nullptr);
		struct tm tm;
		localtime_r(&time_, &tm);
		snprintf(data_, sizeof(data_), "_%02d:%02d:%02d,%02d-%02d-%04d",
				 tm.tm_hour, tm.tm_min, tm.tm_sec,
				 tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900);
		assert(length == strnlen(data_, sizeof(data_)));
	}

	// always return true
	bool parse(const char* pack, size_t pack_len) {
		if (pack_len < length) {
			return false;
		}

		memcpy(data_, pack, length);
		data_[length] = '\0'; // to string

		struct tm tm;
		int ret = sscanf(pack, "_%02d:%02d:%02d,%02d-%02d-%04d",
						 &tm.tm_hour, &tm.tm_min, &tm.tm_sec,
						 &tm.tm_mon, &tm.tm_mday, &tm.tm_year);
		if (ret != 6) {
			JLOG_WARN("AdemcoTimeStamp::Parse sscanf_s ret {} != 6", ret);
			JLOG_HEX(pack, pack_len);

			// ignore it
			//return false;
		}

		tm.tm_year -= 1900;
		tm.tm_mon--;
		tm.tm_isdst = -1;
		time_ = mktime(&tm);
		if (time_ < 0) {
			make();
			return true;
		}

		return true;
	}
}; // AdemcoTimeStamp

/**
* @brief 数值的字符串
*/
struct NumStr16
{
	static constexpr int length = 4;
	char data_[length + 1] = { '0', '0', '0', '0', '\0' };
	uint16_t value_ = 0;

	NumStr16() = default;	
	virtual ~NumStr16() = default;

	virtual void fromCharArray(const char[length]) = 0;
	virtual void fromUint16(uint16_t value) = 0;

	const char* toString() {
		data_[length] = '\0';
		return data_;
	}
};

/**
* @brief 16进制数字的字符串
* @note 可以表示的数字范围为 [0x0000, 0xFFFF]， 即一个 uint16
* @note 例：数字 0 表示为 "0000", 数字 256 表示为 "0100", 数字 65535 表示为 "FFFF"
*/
struct HexStr16 : public NumStr16
{
	HexStr16() = default;
	virtual ~HexStr16() = default;
	HexStr16(uint16_t value) { fromUint16(value); }
	HexStr16(const NumStr16& rhs) { fromCharArray(rhs.data_); }
	HexStr16& operator=(uint16_t value) { fromUint16(value); return *this;}
	HexStr16& operator=(const HexStr16& rhs) { fromCharArray(rhs.data_); return *this; }

	virtual void fromCharArray(const char data[length]) override {
		memcpy(data_, data, length);
		value_ = static_cast<uint16_t>(detail::HexCharArrayToDec(data, length));
		data_[length] = '\0';
	}

	virtual void fromUint16(uint16_t value) override {
		assert(0 <= value && value <= 0xFFFF);
		value_ = value;
		detail::Dec2HexCharArray_4(value, data_, false);
		data_[length] = '\0';
	}
};
typedef HexStr16 Crc16;

//! 16进制数字的字符串，与 Crc16不同的是，Length16 有最大值 0x0FFF
struct Length16 : public HexStr16
{
	Length16() = default;
	virtual ~Length16() = default;
	Length16(uint16_t value) { fromUint16(value); }
	Length16(const Length16& rhs) { fromCharArray(rhs.data_); }
	Length16& operator=(uint16_t value) { fromUint16(value); return *this; }
	Length16& operator=(const Length16& rhs) { fromCharArray(rhs.data_); return *this; }

	virtual void fromUint16(uint16_t value) override {
		assert(0 <= value && value <= 0x0FFF);
		value_ = value;
		detail::Dec2HexCharArray_4(value, data_, true);
		data_[length] = '\0';
	}
};

/**
* @brief 10进制数字的字符串
* @note 可以表示的数字范围为 [0, 9999]
* @note 例：数字 0 表示为 "0000", 数字 256 表示为 "0256"
*/
struct DecStr16 : public NumStr16
{
	DecStr16() = default;
	virtual ~DecStr16() = default;
	DecStr16(uint16_t value) { fromUint16(value); }
	DecStr16(const DecStr16& rhs) { fromCharArray(rhs.data_); }
	DecStr16& operator=(uint16_t value) { fromUint16(value); return *this; }
	DecStr16& operator=(const DecStr16& rhs) { fromCharArray(rhs.data_); return *this; }

	virtual void fromCharArray(const char data[length]) override {
		memcpy(data_, data, length);
		value_ = static_cast<uint16_t>(detail::NumCharArray2Dec(data, length) % 10000);
	}

	virtual void fromUint16(uint16_t value) override {
		assert(0 <= value && value <= 9999);
		value_ = value;
		snprintf(data_, sizeof(data_), "%04d", static_cast<int>(value % 10000));
	}
};
typedef DecStr16 AdemcoSeq;

static constexpr size_t ID_COUNT = 8;
static constexpr const char* ID_NULL	= "\"NULL\"";
static constexpr const char* ID_ACK		= "\"ACK\"";
static constexpr const char* ID_NAK		= "\"NAK\"";
static constexpr const char* ID_DUH		= "\"DUH\"";
static constexpr const char* ID_HB		= "\"HENG-BO\"";
static constexpr const char* ID_ADM_CID = "\"ADM-CID\"";
static constexpr const char* ID_MOD_REG = "\"MODULE-REG\"";
static constexpr const char* ID_REG_RSP = "\"REG-RSP\"";

static constexpr const char* AdemcoIds[ID_COUNT] = {
	ID_NULL,
	ID_ACK,
	ID_NAK,
	ID_DUH,
	ID_HB,
	ID_ADM_CID,
	ID_MOD_REG,
	ID_REG_RSP,
};

struct AdemcoMsgId
{
	enum Enum {
		id_null,
		id_ack,
		id_nak,
		id_duh,
		id_hb,
		id_admcid,
		id_modreg,
		id_regresp,
	};

	static bool isKnownId(const char* id) {
		for (auto p : AdemcoIds) { if (strncmp(id, p, strlen(p)) == 0) { return true; } }
		return false;
	}

	Enum eid_ = Enum::id_null;

	void clear() { eid_ = Enum::id_null; }
	const char* data() const { return AdemcoIds[eid_]; }
	size_t size() const { return strlen(AdemcoIds[eid_]); }

	bool checkAndSet(const char* id, size_t id_len) {
		for (Enum eid = Enum::id_null; eid <= Enum::id_regresp; eid = static_cast<Enum>(eid + 1)) {
			auto p = AdemcoIds[eid];
			if (strncmp(id, p, std::min(strlen(p), id_len)) == 0) {
				eid_ = eid; return true;
			}
		}
		return false;
	}
}; // AdemcoId

struct Rrcvr
{
	static constexpr const char* default_data_ = "R123ABC";
	std::vector<char> data_ = {};

	Rrcvr() { setDefault(); }

	void clear() { data_.clear(); }
	void setDefault() { data_.clear(); std::copy(default_data_, default_data_ + strlen(default_data_), std::back_inserter(data_)); }
	size_t size() const { return data_.size(); }
	void fromCharArray(const char* data, size_t len) {
		data_.clear(); std::copy(data, data + len, std::back_inserter(data_));
	}
};

struct Lpref
{
	static constexpr const char* default_data_ = "L456DEF";
	std::vector<char> data_ = {};
	Lpref() { setDefault(); }

	void clear() { data_.clear(); }
	void setDefault() { data_.clear(); std::copy(default_data_, default_data_ + strlen(default_data_), std::back_inserter(data_)); }
	size_t size() const { return data_.size(); }
	void fromCharArray(const char* data, size_t len) {
		data_.clear(); std::copy(data, data + len, std::back_inserter(data_));
	}
};

struct AdemcoAcct
{
private:
	std::vector<char> data_ = {};

public:
	AdemcoAcct() { clear(); }

	void clear() { setAcct(size_t(0)); }

	//! asString
	const char* acct() const { return &data_[1]; }

	const char* data() const { return &data_[0]; }
	size_t size() const { return data_.size() - 1; }

	// acct must be null-terminated
	void setAcct(const char* acct) { setAcct(acct, strlen(acct)); }

	void setAcct(const char* acct, size_t len) {
		data_.resize(1 + len + 1);  data_[0] = '#';
		std::copy(acct, acct + len, &data_[1]);
		data_[1 + len] = '\0';
	}

	void setAcct(size_t acct) {
		data_.resize(8); data_[0] = '#'; 
		snprintf(&data_[1], 7, "%06X", static_cast<int>(acct % 1000000));
	}

	void setAcct(const std::string& acct) {
		data_.resize(1 + acct.size() + 1); data_[0] = '#';
		std::copy(acct.begin(), acct.end(), &data_[1]);
		data_[1 + acct.size()] = '\0';
	}
}; // AdemcoAcct

struct AdemcoPacket
{
	static constexpr char LF = 0x0A;
	Crc16 crc_ = {};
	Length16 len_ = {};
	AdemcoMsgId id_ = {};
	AdemcoSeq seq_ = {};
	Rrcvr rrcvr_ = {};
	Lpref lpref_ = {};
	AdemcoAcct acct_ = {};
	AdemcoData ademcoData_ = {};
	XDataPtr xdata_ = {};
	AdemcoTimeStamp timestamp_ = {};
	static constexpr char CR = 0x0D;

	AdemcoPacket() {}
	AdemcoPacket(const AdemcoPacket& rhs) { deepCopy(rhs); }
	AdemcoPacket& operator=(const AdemcoPacket& rhs) { deepCopy(rhs); return *this; }

	void deepCopy(const AdemcoPacket& rhs) {
		crc_ = rhs.crc_;
		len_ = rhs.len_;
		id_ = rhs.id_;
		seq_ = rhs.seq_;
		rrcvr_ = rhs.rrcvr_;
		lpref_ = rhs.lpref_;
		acct_ = rhs.acct_;
		ademcoData_ = rhs.ademcoData_;
		if (rhs.xdata_) {
			xdata_ = std::make_shared<XData>(*rhs.xdata_);
		} else {
			xdata_.reset();
		}
		timestamp_ = rhs.timestamp_;
	}

	void clear() {
		crc_ = 0;
		len_ = 0;
		id_.clear();
		seq_ = 0;
		rrcvr_.clear();
		lpref_.clear();
		acct_.clear();
		ademcoData_.clear();
		xdata_.reset();
	}

	size_t calcLength() const {
		//    LF  CRC LEN               SEQ  
		return 1 + 4 + 4 + id_.size() + 4 + rrcvr_.size() + lpref_.size()
			+ acct_.size() + ademcoData_.size() + (xdata_ ? xdata_->rawSize() : 0) +
			timestamp_.length + 1; // CR
	}

	void copyData(char* dst, size_t length) {
		assert(length == calcLength());

		char* pos = dst;
		*pos++ = LF;

		char* crc_pos = pos; pos += 4;
		char* len_pos = pos; pos += 4;
		char* id_pos = pos;

		memcpy(id_pos, id_.data(), id_.size()); pos += id_.size();
		memcpy(pos, seq_.data_, seq_.length); pos += seq_.length;
		memcpy(pos, &rrcvr_.data_[0], rrcvr_.size()); pos += rrcvr_.size();
		memcpy(pos, &lpref_.data_[0], lpref_.size()); pos += lpref_.size();
		memcpy(pos, acct_.data(), acct_.size()); pos += acct_.size();
		memcpy(pos, &ademcoData_.data_[0], ademcoData_.size()); pos += ademcoData_.size();
		if (xdata_) {
			memcpy(pos, &xdata_->rawData_[0], xdata_->rawSize());
			pos += xdata_->rawSize();
		}
		memcpy(pos, timestamp_.data_, timestamp_.length); pos += timestamp_.length;

		*pos++ = CR;
		assert(static_cast<size_t>(pos - dst) == length);

		len_ = static_cast<uint16_t>(length - 9 - 1);
		crc_ = detail::CalculateCRC(id_pos, len_.value_);
		memcpy(crc_pos, crc_.data_, crc_.length);
		memcpy(len_pos, len_.data_, len_.length);
	}

	std::string toString(detail::ToStringOption xdataToStringOpt = detail::ToStringOption::TRY_IS_ALNUM_FIRST) {
		char buf[1024] = { 0 };
		auto sz = make(buf, sizeof(buf));
		buf[sz] = '\0';

		char* pos = buf;
		*pos++ = '\\';
		*pos++ = 'n';
		memcpy(pos, crc_.data_, crc_.length); pos += crc_.length;
		memcpy(pos, len_.data_, len_.length); pos += len_.length;
		memcpy(pos, id_.data(), id_.size()); pos += id_.size();
		memcpy(pos, seq_.data_, seq_.length); pos += seq_.length;
		memcpy(pos, &rrcvr_.data_[0], rrcvr_.size()); pos += rrcvr_.size();
		memcpy(pos, &lpref_.data_[0], lpref_.size()); pos += lpref_.size();
		memcpy(pos, acct_.data(), acct_.size()); pos += acct_.size();
		memcpy(pos, &ademcoData_.data_[0], ademcoData_.size()); pos += ademcoData_.size();

		if (xdata_) {
			for (auto c : xdataToString(xdata_, xdataToStringOpt)) {
				*pos++ = c;
			}
		}

		memcpy(pos, timestamp_.data_, timestamp_.length); pos += timestamp_.length;

		*pos++ = '\\';
		*pos++ = 'r';
		*pos++ = '\0';

		return buf;
	}

	// maker. set all data before call it.
	size_t make(char* pack,  size_t pack_len) {
		assert(pack);
		size_t length = calcLength();
		assert(length < pack_len);
		if (length < pack_len) {
			copyData(pack, length);
			return length;
		} else { return 0; }
	}

	// set acct to nullptr if you want to use ademco_id as acct
	size_t make(char* pack, size_t pack_len, 
				AdemcoMsgId::Enum id, 
				uint16_t seq, 
				const char* acct, 
				AdemcoId ademco_id, 
				AdemcoGG gg, 
				ADEMCO_EVENT aevnt, 
				AdemcoZone zone, 
				XDataPtr xdata = {})
	{
		assert(pack);
		id_.eid_ = id;
		seq_.fromUint16(seq);
		rrcvr_.setDefault(); lpref_.setDefault();
		acct ? acct_.setAcct(acct) : acct_.setAcct(ademco_id);
		ademcoData_.make(ademco_id, gg, aevnt, zone);
		xdata_ = xdata;
		timestamp_.make();
		size_t length = calcLength();
		assert(length < pack_len);
		if (length < pack_len) {
			copyData(pack, length);
			return length;
		} else { return 0; }
	}

	// set acct to nullptr if you want to use ademco_id as acct
	size_t make_null(char* pack, size_t pack_len, uint16_t seq, const char* acct, AdemcoId ademco_id)
	{
		id_.eid_ = AdemcoMsgId::Enum::id_null;
		seq_ = seq;
		rrcvr_.setDefault(); lpref_.setDefault();
		acct ? acct_.setAcct(acct) : acct_.setAcct(ademco_id);
		ademcoData_.make();
		xdata_.reset();
		timestamp_.make();
		size_t length = calcLength();
		assert(length < pack_len);
		if (length < pack_len) {
			copyData(pack, length);
			return length;
		} else { return 0; }
	}

	// set acct to empty if you want to use ademco_id as acct
	size_t make_null(char* pack, size_t pack_len, uint16_t seq, const std::string& acct, AdemcoId ademco_id) {
		return make_null(pack, pack_len, seq, acct.empty() ? nullptr : acct.data(), ademco_id);
	}

	// set acct to nullptr if you want to use ademco_id as acct
	size_t make_ack(char* pack, size_t pack_len, uint16_t seq, const char* acct, AdemcoId ademco_id)
	{
		id_.eid_ = AdemcoMsgId::Enum::id_ack;
		seq_ = seq;
		rrcvr_.setDefault(); lpref_.setDefault();
		acct ? acct_.setAcct(acct) : acct_.setAcct(ademco_id);
		ademcoData_.make();
		xdata_.reset();
		timestamp_.make();
		size_t length = calcLength();
		assert(length < pack_len);
		if (length < pack_len) {
			copyData(pack, length);
			return length;
		} else { return 0; }
	}

	// set acct to nullptr if you want to use ademco_id as acct
	size_t make_ack(char* pack, size_t pack_len, uint16_t seq, const std::string& acct, AdemcoId ademco_id) {
		return make_ack(pack, pack_len, seq, acct.empty() ? nullptr : acct.data(), ademco_id);
	}

	size_t make_nak(char* pack, size_t pack_len, uint16_t seq, const std::string& acct, AdemcoId ademco_id) {
		id_.eid_ = AdemcoMsgId::Enum::id_nak;
		seq_ = seq;
		rrcvr_.setDefault(); lpref_.setDefault();
		!acct.empty() ? acct_.setAcct(acct) : acct_.setAcct(ademco_id);
		ademcoData_.make();
		xdata_.reset();
		timestamp_.make();
		size_t length = calcLength();
		assert(length < pack_len);
		if (length < pack_len) {
			copyData(pack, length);
			return length;
		} else { return 0; }
	}

	// set acct to nullptr if you want to use ademco_id as acct
	size_t make_hb(char* pack, size_t pack_len, 
				   uint16_t seq, 
				   const char* acct, 
				   AdemcoId ademco_id,
				   AdemcoGG gg,
				   ADEMCO_EVENT evnt, 
				   AdemcoZone zone,
				   XDataPtr xdata = {})
	{
		id_.eid_ = AdemcoMsgId::Enum::id_hb;
		seq_ = seq;
		rrcvr_.setDefault(); lpref_.setDefault();
		acct ? acct_.setAcct(acct) : acct_.setAcct(ademco_id);
		(ademco_id == 0 && acct) ? ademcoData_.make(acct, gg, evnt, zone) : ademcoData_.make(ademco_id, gg, evnt, zone);
		
		xdata_ = xdata;
		timestamp_.make();
		size_t length = calcLength();
		assert(length < pack_len);
		if (length < pack_len) {
			copyData(pack, length);
			return length;
		} else { return 0; }
	}

	// set acct to empty if you want to use ademco_id as acct
	size_t make_hb(char* pack, size_t pack_len,
				   uint16_t seq,
				   const std::string& acct,
				   AdemcoId ademco_id,
				   AdemcoGG gg,
				   ADEMCO_EVENT evnt,
				   AdemcoZone zone,
				   XDataPtr xdata = {})
	{
		return make_hb(pack, pack_len, seq, acct.empty() ? nullptr : acct.data(), ademco_id, gg, evnt, zone, xdata);
	}

	// parser
	ParseResult parse(const char* pack, size_t pack_len, size_t& cb_commited) {
		//AUTO_LOG_FUNCTION;
#ifdef _DEBUG
		try {
#endif
			do {
				if (pack_len < 9) {
					JLOG_INFO("AdemcoPacket::Parse pack_len {} < 9", pack_len); JLOG_HEX(pack, pack_len);
					return ParseResult::RESULT_NOT_ENOUGH;
				}

				// check LF
				if (pack[0] != LF) {
					JLOG_ERRO("AdemcoPacket::Parse pack[0] {:d} 0x{:02X} is not _LF", (int)pack[0], (unsigned char)pack[0]);
#ifdef _WIN32
					return ParseResult::RESULT_DATA_ERROR;
#else
					break;
#endif
				}

				clear();

				// read crc & len
				crc_.fromCharArray(pack + 1);
				len_.fromCharArray(pack + 5);

				// read till CR
				size_t len_to_parse = 9u + len_.value_ + 1; // 1 for CR

				// check if packet is enough to parse
				if (pack_len < len_to_parse) {
					JLOG_ERRO("AdemcoPacket::Parse pack_len {} < len_to_parse {}", pack_len, len_to_parse);
					return ParseResult::RESULT_NOT_ENOUGH;
				}

				// check CR
				const char* id_pos = pack + 9;
				const char* CR_pos = id_pos + len_.value_;
				if (*CR_pos != CR) {
					JLOG_ERRO("AdemcoPacket::Parse ademco_len err!");
					break;
				}

				// check ademco CRC
				unsigned short crc_calc = detail::CalculateCRC(id_pos, len_.value_);
				if (crc_.value_ != crc_calc) {
					JLOG_ERRO("AdemcoPacket::Parse crc failed!");
					break;
				}

				// id
				if (*id_pos != '\"') { // find first " of "id".
					JLOG_ERRO("AdemcoPacket::Parse find left \" of \"id\" faild!");
					break;
				}
				const char* p = id_pos + 1; // find last " of "id".
				while (p < CR_pos && *p != '\"') { p++; } if (p >= CR_pos) { break; }
				if (*p++ != '\"') { // " not found.
					JLOG_ERRO("AdemcoPacket::Parse find right \" of \"id\" faild!");
					break;
				}
				if (!id_.checkAndSet(id_pos, p - id_pos)) {
					JLOG_ERRO("AdemcoPacket::Parse check id faild!");
					break;
				}

				// seq (and Rrcvr, it may not exists)
				const char* seq_pos = p;
				while (p < CR_pos && *p != 'R' && *p != 'L') { p++; } if (p >= CR_pos) { break; }
				if (p - seq_pos != 4) {
					JLOG_ERRO("AdemcoPacket::Parse seq len != 4");
					break;
				}
				seq_.fromCharArray(seq_pos);

				if (*p == 'R') { // Rrcvr exists
					const char* rrcvr_pos = p;
					while (p < CR_pos && *p != 'L' && *p != '#') { p++; } if (p >= CR_pos) { break; }
					//ASSIGH_SEG_DATA(rrcvr);
					rrcvr_.fromCharArray(rrcvr_pos, p - rrcvr_pos);
				} else if (*p == 'L') { // Rrcvr not exists, pass
					// pass
				} else {
					JLOG_ERRO("AdemcoPacket::Parse Lpref and Rrcvr not found!");
					break;
				}

				// Lpref
				if (*p != 'L') { // L of Lpref not found.
					JLOG_ERRO("AdemcoPacket::Parse Lpref not found!");
					break;
				}
				const char* lpref_pos = p;
				while (p < CR_pos && *p != '#') { p++; } if (p >= CR_pos) { break; }
				lpref_.fromCharArray(lpref_pos, p - lpref_pos);

				// acct
				if (*p++ != '#') { // # of #acct not found
					JLOG_ERRO("AdemcoPacket::Parse # of #acct not found!");
					break;
				}
				const char* acct_pos = p;
				while (p < CR_pos && *p != '[') {
					if (!isalnum(*p)) { // acct must be alphanumeric
						JLOG_ERRO("AdemcoPacket::Parse acct check isalnum failed!");
						p = nullptr;
						break;
					}
					p++;
				}
				if (p == nullptr) { break; }
				if (p >= CR_pos) { break; }
				acct_.setAcct(acct_pos, p - acct_pos);

				// data
				if (*p != '[') { // [ of [data] not found.
					JLOG_ERRO("AdemcoPacket::Parse [ of [data] not found!");
					break;
				}
				const char* data_pos = p;
				while (p < CR_pos && *p != ']') { p++; } if (p >= CR_pos) { break; }
				if (*p != ']') { // ] of [data] not found.
					JLOG_ERRO("AdemcoPacket::Parse ] of [data] not found!");
					break;
				}
				size_t ademco_data_len = ++p - data_pos;
				if (!ademcoData_.parse(data_pos, ademco_data_len)) {
					JLOG_ERRO("AdemcoPacket::Parse parse data failed!");
					break;
				}

				// [x...data...]
				if (*p == '[') { // xdata exists
					xdata_ = parseXData(p, CR_pos);
					if (!xdata_) { break; }
					p += xdata_->rawSize();
				}

				if (*p != '_') { // _ of _timestamp not found.
					JLOG_ERRO("AdemcoPacket::Parse _ of _timestamp not found!");
					break;
				}
				if (!timestamp_.parse(p, CR_pos - p)) {
					JLOG_ERRO("AdemcoPacket::Parse parse timestamp failed!");
					break;
				}
				p += timestamp_.length;

				// check CR
				if (p++ != CR_pos) { assert(0); break; }

				cb_commited = p - pack;
				return ParseResult::RESULT_OK;
			} while (0);

#ifdef _DEBUG
		} catch (std::exception& e) {
			printf("AdemcoPacket::parse EXCEPTION: %s\n", e.what());
			abort();
		}
#endif

		// dump data
		JLOG_HEX(pack, std::min(pack_len, jlib::MAX_INPUT_BUFF_LEN));
		JLOG_ASC(pack, std::min(pack_len, jlib::MAX_INPUT_BUFF_LEN));
		assert(0);

		return ParseResult::RESULT_DATA_ERROR;
	}
}; // AdemcoPacket

} // namespace ademco
