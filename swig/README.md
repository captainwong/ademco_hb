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