# Tools 测试及演示工具，已编译的dll等

* `ademco_hb.dll` 供 `dll_demo` 和 `csharp_dll_demo` 调用
* `ademco_java.dll` 供 `Java SimpleServer` 调用 
* `bench.exe` 本机性能测试工具
* `demo.exe` 正确性测试以及使用C++开发、直接引入头文件的示例
* `server_demo.exe` 简易版接警中心，可用来接收主机报告、远程控制（已停止维护）
* `server_demo_libevent` 简易版接警中心，可用来接收主机连接，进行简单交互：接收主机事件；布撤防、查询主机类型或其他自定义消息等命令。

    主机连接成功后可以发送的命令列表：
    * `A`: 对所有上线的主机布防
    * `D`: 对所有上线的主机撤防，要求输入6位数字密码
    * `E`: 对所有上线的主机发送紧急报警命令
    * `T`: 对所有上线的主机发送 `1798` 查询主机类型
    * `M`: 对所有上线的主机，手动输入 [事件码 gg zone]
    * `C`: 对指定安定宝ID的主机发送：[ademco_id 事件码 gg zone]
    * `X`: 对指定安定宝ID的主机发送，附带字符串形式的 XDATA: [ademco_id 事件码 gg zone xdata]，如 [6668888 1400 0 0 123456]
    * `Y`: 对指定安定宝ID的主机发送，附带十六进制形式的 XDATA: [ademco_id 事件码 gg zone xdata]，如 [666888 1704 0 0 EBBA3FA176]
    * `Z`: 对指定安定宝ID的主机发送，开启查询防区信息流程

    * `I`: 打印统计信息
    * `P`: 打印数据包开关
    * `Q`: 退出

* `bench_client` 性能测试工具，用法 `bench_client ip port thread_count session_count timeout`。使用 `thread_count` 个线程、发起 `session_count` 个客户端，对 `ip:port` 上运行的接警中心进行性能测试，在 `timeout` 秒后停止并输出测试结果。

## 直连型主机模拟器 

*说明：本程序仅用来模拟测试，如果有与真实主机不一致的情况，以真实主机为准。*

注意：界面左侧修改主机配置后需要点击左上角菜单 文件--保存 后才能被程序读取生效。