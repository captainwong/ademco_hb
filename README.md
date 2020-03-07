# Hengbo Protocal Library

恒博协议库

欢迎提 issue 及 pr 。

## docs

文档

## tools 测试及演示工具，已编译的dll等

* bench.exe 性能测试
* demo.exe 正确性测试以及使用C++开发、直接引入头文件的示例
* server_demo.exe 简易版服务器，可用来接收主机报告、远程控制
* 直连型主机模拟器
* dll
  * x64
    * ademco_hb.lib
    * ademco_hb.dll
  * x86
    * ademco_hb.lib
    * ademco_hb.dll

## include

* `ademco_event.h` 

    定义了安定宝事件码及所需的结构体

* `ademco_detail.h` 

    定义了各种辅助函数

* `ademco_packet.h` 

    定义了可用于安定宝协议封包、解包的结构体与相应函数

## demo

演示基本使用方法：正确性测试以及使用C++开发、直接引入头文件的示例

## server_demo

可用来接收主机连接，进行简单交互：接收主机事件；布撤防。

## ademco_hb

动态链接库，仅提供对网络数据的解包、打包等接口。

调用者需要在解包后再次对数据包解析，以提取安定宝ID、事件码等信息，建议使用定址取值或正则取值。

## dll_demo

演示使用 C++ 调用 ademco_hb.dll

## csharp_dll_demo

演示使用 .net core 调用 ademco_hb.dll

*本人对 C# 不熟悉，欢迎提 PR 进行改进*


