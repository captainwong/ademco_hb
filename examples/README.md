## ademco

`Windows` 下的 安定宝 `dll` 工程，使用`Visual Studio 2019`打开 `ademco_hb_examples.sln`编译.

## server_demo

C++ 编写的简易版接警中心，可用来接收主机连接，进行简单交互：接收主机事件；布撤防。

`Windows` 编译同上，`linux` 执行 `build.sh`

## httprelay

C/C++ 编写的简易版 TCP 服务器与 HTTP 转发器，可以用来接收主机连接，并转发主机事件到指定的 URI；3.9.0 起内置远程控制接口，支持布撤防命令下发.

依赖 `libuv, libcurl`，用包管理器如 `apt, brew, yum`等安装 `dev` 版，执行 `build.sh` 编译.

运行示例：

```bash
./httprelay 12345 http://your-http-server.com:8080/ademco
```

httprelay 须运行在公网环境，报警主机配置直连接警中心IP、端口，主机有事件发生时将转发至 http://your-http-server.com:8080/ademco

转发格式为 `POST` + `application/json`（无 query 参数），如：

```json
{"acct":"861234567890","ademco_event":3400,"zone":0,"gg":0,"timestamp":1763893209}
```

远程控制（可选启用）：

```bash
./httprelay 12345 http://cb/ademco --control_port=8081 --listen_addr=0.0.0.0 --token=secret
```

`POST /control`（携带 `Authorization: Bearer <token>`）下发布防 `3400` / 留守布防 `3456` / 撤防 `1400`（须带 `password`），fire-and-forget，执行结果以状态事件经转发回调返回.

完整回调与控制接口规格、上线/掉线合成事件等说明见 `docs/对接说明.md`。
