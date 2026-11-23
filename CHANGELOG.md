# ademco_hb 项目变更记录

本项目（ademco_hb）的版本号自 3.8.1 起独立维护，标识 release 打包物整体（examples、绑定、httprelay、文档等）。`src/` 内各库版本由上游私有仓库自管，`ademco_version()`、`uvlib_version()` 等接口可查询组件实际版本。3.8.0 及之前的历史版本见 [releases](https://github.com/captainwong/ademco_hb/releases)。

## 3.9.0 2026-11-23

| 组件 | 版本 | 变化 |
| ---- | ---- | ---- |
| libademco | 3.8.0 | 无变化 |
| libuvlib | 1.2.2 | 无变化（3.8.1 已升级） |
| httprelay | 1.0.0 | **首个独立版本**。新增远程控制接口（布撤防）：`--control_port/--listen_addr/--token`，`POST /control` 白名单事件下发，fire-and-forget，执行结果以状态事件经回调返回；新增 `control.cpp`（C++）与 acct 索引；`--version` 参数与启动打印 |
| httprelay | 1.0.0 | 事件转发 JSON 改为单行格式（`cJSON_PrintUnformatted`） |
| tools | - | 新增端到端测试 `test_control.sh`（配合私有仓库 uvmachine，不随 release 发布） |

## 3.8.1 2026-11-23

| 组件 | 版本 | 变化 |
| ---- | ---- | ---- |
| libademco | 3.8.0 | 无变化 |
| libuvlib | 1.0.4 -> 1.2.2 | tcpserver 大量稳定性与安全修复（accept 失败导致进程退出、句柄泄漏、读死循环等） |
| httprelay | - | 适配 uvlib 1.1.0 连接回调新签名；构建系统补 uv_http.c |
| tools | - | httprelay 的 copy_from_hb.sh 修正（路径、README/CHANGELOG 同步） |

- 发版工具链：mkrelease.sh 版本号来源改为根目录 RELEASE_VERSION，与 libademco 库版本解耦

## 3.8.0 2026-11-23（追认）

| 组件 | 版本 | 变化 |
| ---- | ---- | ---- |
| libademco | 3.6.3 -> 3.8.0 | ABI 变更：结构体 1 字节对齐；time API 统一 emb_ 前缀；新增事件 1847/1900/3900/1574/3574；ZONE_TAMPER 改名 SENSOR_TAMPER；新增 libemb/emb_string |
| httprelay | - | JSON 事件转发新增 timestamp 字段；回调格式修正为 POST JSON |
| docs | - | 对接说明重写（httprelay 路线为主线）；协议文档补全（报警恢复事件码、ACK 应答规范、远程布撤防细则、FAQ） |
| tools | - | 构建脚本归并整理（22 -> 10），远程构建改为本地打包上传模式 |
