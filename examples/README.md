## ademco

Windows 下的 安定宝 `dll` 工程。

## server_demo

C++ 编写的简易版接警中心，可用来接收主机连接，进行简单交互：接收主机事件；布撤防。

## httprelay

C 编写的简易版 TCP 服务器与 HTTP 转发器，可以用来接收主机连接，并转发主机事件到指定的 URI.

依赖 `libuv, libcurl`.

示例：

```bash
./httprelay 12345 http://your-http-server.com:8080/ademco
```

httprelay 须运行在公网环境，报警主机配置直连接警中心IP、端口，主机有事件发生时将转发至 http://your-http-server.com:8080/ademco
