# swig 绑定说明

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

下载 [swig](https://www.swig.org/download.html)，解压，修改 `win_java.bat`，将 `swig.exe` 路径修改为你的路径。

需要已安装 `Visual Studio`。

打开 `x64 Native Tools Command Prompt for VS 2019`, 切换到当前目录，执行 `win_csharp.bat`。

### 2.2 其他系统

请自行研究。
