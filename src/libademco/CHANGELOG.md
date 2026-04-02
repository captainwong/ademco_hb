# Ademco Change Log


## 3.6.3 2026-04-02 15:14:05

- 增加主机类型 1827 门铃主机，1837 铁路预警主机


## 3.6.2 2025-7-4 03:23:29

- add compitability for `CX51`


## 3.6.1 2025-7-1 16:41:04

- add `bitrev*` functions


## 3.6.0 2025-6-3 15:26:52

控制源协议更新:

- 取消原 `96` 中继报警识别码，`96` 重新分配给 `SMART_HOME_MAX`
- 将保留的 `11-50` 拿出 `11-30` 作为中继报警的识别码


## 3.5.3 2025-5-26 21:48:16

- `ademco_is_valid_account` don't recognize all `0` or all `f` account as valid account anymore
- add some docs
- add `ademco_hilo_array_streq`


## 3.5.2 2025-5-24 15:39:50



## 3.5.1 2025-5-19 15:57:38



## 3.5.0 2025-5-16 16:19:24



## 3.4.13 2025-5-9 15:32:20



## 3.4.12 2025-5-8 20:32:49

- add event `POWER_STATE` 1708 for power state, add `hb_power.h`


## 3.4.11 2025-4-17 16:24:04



## 3.4.10 2025-3-15 18:24:26

- `CFLAGS` add `-Wpedantic -Wextra -Werror -std=gnu99 -O2`


## 3.4.9 2025-3-8 10:22:48

- fix gcc warnings
- refactor hb zone properties


## 3.4.8 2024-12-27 14:27:42

- add event `1796 SYS_READY`, 主机在自己认为合适的时机发出，可以进行防区同步了


## 3.4.7 2024-12-26 14:05:02

- add `tm_from_str`, `time_from_str`


## 3.4.6 2024-12-19 15:33:56



## 3.4.5 2024-12-16 23:40:10



## 3.4.4 2024-12-6 16:08:40

- add `hb_rf_code 66` for heart beat


## 3.4.3 2024-12-2 16:50:15

- add `ringbuf_linear_readable`, `ringbuf_linear_addr`


## 3.4.2 2024-11-25 10:00:26

- add `ringbuf_skip`
- a few doc updates


## 3.4.1 2024-11-20 13:54:16



## 3.4.0 2024-11-19 17:06:48



## 3.3.1 2024-10-25 21:34:39



## 3.3.0 2024-10-25 15:24:31



## 3.2.3 2024-10-18 15:11:53



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
- add config option `EMB_USE_STATIC_BUF, EMB_USE_UNIQUE_BUF` for embedded systems
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
