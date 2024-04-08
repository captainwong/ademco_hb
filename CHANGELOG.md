# Ademco Change Log


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

- fix `hb_com_resp_a2_p1_t` typo


## 2.1.2

- fix `ademco_parse_packet` on met invalid timestamp, `pkt.timestamp` is not inited


## 2.1.1



## 2.1.0

- change `ademco_zone_t` to `uint16_t`
- change `ademco_gg_t` to `uint8_t`


## 2.0.0

- add `hb_com_resp_ad_iter_t`, `hb_com_resp_ad_iter_init` and `hb_com_resp_ad_iter_next`
- add `ADEMCO_PACKET_PWD_MIN_LEN`, `ADEMCO_PACKET_PWD_MAX_LEN`
- code styles changed
- when parse returns `ADEMCO_PARSE_RESULT_ERROR`, `*ate` will contains length to ignore, caller must drop `*ate` bytes of data


## 1.5.1

- `account` minimum length is 6
- `ademco_is_valid_account` check length


## 1.5.0

- `hb_com_resp_a2_iter_t.com` changed to pointer
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
