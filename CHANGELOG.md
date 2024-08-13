# Ademco Change Log


## 3.2.2 2024-8-7 15:10:37

- 解析形为 [#861234567890|3400 00 000] 之类的以账号为安定宝ID的data段，解析结果为安定宝ID为0


## 3.2.1 2024-8-7 14:29:35

- 修复安定宝协议data段 #acct 生成错误的bug


## 3.2.0 2024-8-4 22:16:32

- 全面支持 C51/gcc-avr/msvc/gcc 等编译器


## 3.1.0 2024-7-11 02:24:14



## 3.0.0 2024-7-1 06:19:01

- 可在 windows/linux/c51/avr 环境下编译


## 2.7.3 2024-6-28 19:32:18

- change `XX` sequence


## 2.7.2 2024-6-25 10:18:16



## 2.7.1 2024-6-4 16:34:48



## 2.7.0 2024-6-4 15:24:15

- shrink `ademco_packet_t` size
- add config option for `time`
- add config option `ADEMCO_USE_STATIC_BUF, ADEMCO_USE_UNIQUE_BUF` for embedded systems
- add `ademco_append_data3` and `ademco_make_hb_packet3` for inplace make
- add `ademco_is_valid_password`
- remove `grinwich` related functions, they are shit due to doc lacking, machine's timer is UTC+0
- event 1384 changed to `EVENT_POWER_EXCEPTION`, 3384 changed to `EVENT_POWER_EXCEPTION_RECOVER`


## 2.6.0 2024-4-20 00:58:15



## 2.5.1 2024-4-19 23:39:51

- `ARM_STAY` related functions


## 2.5.0 2024-4-19 18:07:08

- `ARM` to `ARM_AWAY`
- `HALF_ARM` to `ARM_STAY`
- `resume` to `recover`
- add several recover events
- fix some typo


## 2.4.0 2024-4-18 04:28:43

- split some features from `hb_com` to `hb_core`


## 2.3.0 2024-4-17 17:38:01



## 2.2.2 2024-4-15 13:35:39



## 2.2.1



## 2.2.0



## 2.1.7



## 2.1.6

- fix typo of `hb_machine_timer_to_greenwich`, `hb_machine_timer_from_greenwich`
- fix com request `A0` doc, remove `p3`


## 2.1.5

- return type of `hb_get_available_zone_properties_by_type` changed from `int` to `size_t`


## 2.1.4

- add `ADEMCO_ID_MASK`, `ADEMCO_ZONE_MASK`, and `ADEMCO_GG_MASK`


## 2.1.3

- fix `uint8_t` typo


## 2.1.2

- fix `ademco_parse_packet` on met invalid timestamp, `pkt.timestamp` is not inited


## 2.1.1



## 2.1.0

- change `ademco_zone_t` to `uint16_t`
- change `ademco_gg_t` to `uint8_t`


## 2.0.0

- add `hb_com_zone_lost_data_iter_t`, `hb_com_zone_lost_data_iter_init` and `hb_com_zone_lost_data_iter_next`
- add `ADEMCO_PACKET_PWD_MIN_LEN`, `ADEMCO_PACKET_PWD_MAX_LEN`
- code styles changed
- when parse returns `ADEMCO_PARSE_RESULT_ERROR`, `*ate` will contains length to ignore, caller must drop `*ate` bytes of dat


## 1.5.1

- `account` minimum length is 6
- `ademco_is_valid_account` check length


## 1.5.0

- `hb_com_zone_data_iter_t.com` changed to pointer
- update parser for `timestamp`
- add `AdemcoControlSource` defs


## 1.4.0

- `hb_com reqeust, response` update


## 1.3.1

- compile with warning level 4
- change `static inline` function to micro


## 1.3.0

- add `file` for parse error
- fix `struct tm not initilized` on parse
- fix parsing pointer on parse `timestamp` failed


## 1.2.0

- add config
- add `offset` for parse error
- refactor parser


## 1.1.0

- `ADEMCO_PACKET_ACCT_MAX_LEN` changed from 64 to 18
- add `ADEMCO_PACKET_ACCT_MAX_LEN`
- `hb_com` 读写主机账号命令


## 1.0.0

- initial version
