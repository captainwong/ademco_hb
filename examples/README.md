## ademco

`Windows` 下的 安定宝 `dll` 工程，使用`Visual Studio 2019`打开 `ademco_hb_examples.sln`编译.

## server_demo

C++ 编写的简易版接警中心，可用来接收主机连接，进行简单交互：接收主机事件；布撤防。

`Windows` 编译同上，`linux` 执行 `build.sh`

## httprelay

C 编写的简易版 TCP 服务器与 HTTP 转发器，可以用来接收主机连接，并转发主机事件到指定的 URI.

依赖 `libuv, libcurl`，用包管理器如 `apt, brew, yum`等安装 `dev` 版，执行 `build.sh` 编译.

运行示例：

```bash
./httprelay 12345 http://your-http-server.com:8080/ademco
```

httprelay 须运行在公网环境，报警主机配置直连接警中心IP、端口，主机有事件发生时将转发至 http://your-http-server.com:8080/ademco
