# swig 绑定说明

## 0. 开始之前：你需要本文档吗？

**SWIG**（Simplified Wrapper and Interface Generator）是一个将 C/C++ 库自动包装成其他语言绑定的工具。本目录提供 `libademco`（安定宝协议解析库）的 **Java / C# / Node.js 语言绑定**，使你可以在不写一行协议解析代码的情况下，直接用熟悉的语言开发接警中心程序（解析主机上报、构造下发命令、处理 `ACK`/心跳）。

先回答三个问题：

**1. 你需要 TCP 直连吗？**

- 只需要**接收事件**，或**布撤防控制** → 不需要本目录，使用 `httprelay`，见[对接说明](../docs/对接说明.md)；
- 需要深度对接（自行管理连接、索要防区信息、定时器等完整协议能力）→ 继续往下。

**2. 能用预编译产物吗？**

优先从 [releases](https://github.com/captainwong/ademco_hb/releases) 下载对应平台的绑定包（`win_java.zip` / `win_csharp.zip` / `win_node.zip` / `linux_java.zip` / `linux_node.zip` 等），解压即可使用，无需编译。本文第 1~3 章的编译流程仅在以下情况需要：预编译包不覆盖你的平台、你修改了 `src/` 下的 C 源码、或你想自己验证。

**3. 绑定包里有什么、怎么装？**

每个绑定包含两部分：**动态库**（`Windows` 为 `dll`，`Linux` 为 `so`，`macOS` 为 `dylib`）和**语言侧源码**（`Java` 包 `com/hb3344/ademco` 下的 `.java`、`C#` 的 `.cs`、`Node.js` 的 `.node` + `nodejs-demo.js` 示例）。使用方式：

| 语言 | 装载方式 |
| ---- | ---- |
| Java | `System.loadLibrary("ademco_java")`，将 `.java` 编入工程 |
| C# | `DllImport("libademco")`，`.cs` 编入工程 |
| Node.js | `require('./build/Release/ademco_js.node')` |

各包内附演示程序（`JavaDemo` / `CSharpDemo` / `nodejs-demo.js`），先跑通演示再写自己的代码。

> 提示：绑定是 `libademco` C 接口的直接映射，函数语义与 [协议文档](../docs/README.md) 及 `src/libademco/ademco.h` 的注释一一对应，遇到不理解的参数先查头文件注释。

## 1. Java 绑定

### 1.1 Windows环境

下载 [swig](https://www.swig.org/download.html)，解压，修改 `win_java.bat`，将 `swig.exe` 路径修改为你的路径。

需要已安装 `Visual Studio`，已安装 `jdk` 并配置了 `JAVA_HOME` 环境变量。

打开 `x64 Native Tools Command Prompt for VS 2019`, 切换到当前目录，执行 `win_java.bat`。

### 1.2 类unix 环境

使用包管理工具安装 `swig` 如：

```bash
sudo apt-get install swig
# 或者
sudo yum install swig
# 或者
brew install swig
```

需要 `gcc`, `jdk`，并配置了 `JAVA_HOME` 环境变量。

`mac` 系统执行 `mac_java.sh`，其他执行 `linux_java.sh`。

## 2. C# 绑定

### 2.1 Windows环境

下载 [swig](https://www.swig.org/download.html)，解压，修改 `win_csharp.bat`，将 `swig.exe` 路径修改为你的路径。

需要已安装 `Visual Studio`。

打开 `x64 Native Tools Command Prompt for VS 2019`, 切换到当前目录，执行 `win_csharp.bat`。

### 2.2 其他系统

请自行研究。

## 3. Node.js 绑定

### Windows 环境

1. 安装 `node-gyp`: `npm install -g node-gyp`

2. 使用 `swig` 生成代码
   
    ```
    swig -c++ -javascript -node libademco.swig
    ```

    将生成 `libademco_wrap.cxx`.

3. 编译
   
   执行 `node-gyp configure build`，将以配置文件 `binding.gyp` 生成工程并编译得到 `libadmeco_js.node`，`Windows` 下应为 `build/Release/libadmeoc_js.node`

4. 运行示例

   `node nodejs-demo.js`

### Ubuntu20.04


#### 1. 更新 `swig`
   
`apt` 安装的 `swig` 版本是 `4.0.1`不匹配，需要卸载后编译安装.

```bash
apt update -y
# 卸载旧版 swig
apt purge swig -y
# 安装依赖库
apt install libpcre2-dev libbison-dev -y
# 下载解压 swig 4.1.1
wget https://sourceforge.net/projects/swig/files/swig/swig-4.1.1/swig-4.1.1.tar.gz/download -Oswig-4.1.1.tar.gz
tar -xzf swig-4.1.1.tar.gz
# 编译
cd swig-4.1.1
mkdir build && cd build
cmake ..
make && make install
```

#### 2. 安装 `node`

```bash
# 最高支持v22.22.2(LTS)，ubuntu22.04下测试 v24.14.1(LTS) 编译失败
curl -sL https://deb.nodesource.com/setup_18.x -o /tmp/nodesource_setup.sh
bash /tmp/nodesource_setup.sh
sudo apt-get install -y nodejs
npm install -g node-gyp
```

#### 3. 生成 `libademco_js.node` 动态库

```bash
# cd /path/to/ademco_hb/swig
node-gyp configure build
# node nodejs-demo.js
```

### macOS

`./linux_mac_node.sh`
