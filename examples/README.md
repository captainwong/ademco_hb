# 


## bench

性能测试

## demo

演示基本使用方法：正确性测试以及使用C++开发、直接引入头文件的示例

## server_demo（已停止更新，请参考 server_demo_libevent）

可用来接收主机连接，进行简单交互：接收主机事件；布撤防。

## server_demo_libevent

简易版接警中心，可用来接收主机连接，进行简单交互：接收主机事件；布撤防、查询主机类型或其他自定义消息等命令。

## bench_client

性能测试工具，用法 `bench_client ip port thread_count session_count timeout [print_data]`。使用 `thread_count` 个线程、发起 `session_count` 个客户端，对 `ip:port` 上运行的接警中心进行性能测试，在 `timeout` 秒后停止并输出测试结果, `print_data` 是否启用数据打印。

## bench_client_ex

批量测试工具，用法 `bench_client_ex ip port thread_count session_count acct_base machine_type heartbeat_gap timeout [print_data]`。使用 `thread_count` 个线程、发起 `session_count` 个客户端，客户端主机账号为以 `acct_base` 为基础累加，主机类型为指定的主机类型或指定 `-1` 以随机化类型，对 `ip:port` 上运行的接警中心进行批量测试，连接成功后可以按下 `ADE` 等按键进行模拟主机事件，在 `timeout` 秒后停止运行, `print_data` 是否启用数据打印。

## ademco_hb

动态链接库，仅提供对网络数据的解包、打包等接口。

调用者需要在解包后再次对数据包解析，以提取安定宝ID、事件码等信息，建议使用定址取值或正则取值。

## dll_demo

演示使用 C++ 调用 ademco_hb.dll

## csharp_dll_demo

演示使用 .net core 调用 ademco_hb.dll

*本人对 C# 不熟悉，欢迎提 PR 进行改进*

## javademo

请阅读 [javademo-README.md](javademo-README.md)
