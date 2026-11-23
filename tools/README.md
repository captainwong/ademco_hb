# tools

本目录脚本仅维护者本人使用，用于编译发版与源码同步。

## 脚本一览

| 脚本 | 说明 |
| ---- | ---- |
| `mkrelease.sh [branch]` | **发版总入口**。串联 examples 与 windows/linux 各语言构建，产出 `ademco_hb-<version>.tar.gz/.zip`（版本号读自根目录 `RELEASE_VERSION`，项目版本与 libademco 库版本独立维护，变更记录见根目录 `CHANGELOG.md`），产物手动上传 github release |
| `mkrelease.bat` | Windows 下以 Git Bash 调起 `mkrelease.sh` 的快捷方式 |
| `mk_examples.sh` | 调 `build_examples.bat` 编译 examples sln（x86/x64），收集 exe/dll 及模拟器等工具为 `dist/examples.zip` |
| `build_examples.bat x86\|x64` | devenv 编译 examples 解决方案 |
| `mk_win.sh java\|csharp\|node` | Windows 绑定构建：swig 生成 + VS 编译（`build_win.bat`）+ 收集产物为 `dist/win_<name>.zip` |
| `build_win.bat java\|csharp\|node` | 调用 `swig/win_<name>.bat` 的薄包装（设置 vcvars/JAVA_HOME） |
| `pack.sh java-linux\|java-mac\|node\|node-mac` | swig 产物收集打包，通常由远端服务器在构建后调用 |
| `clean.sh` | 清理 swig 与 tools 的构建产物 |
| `copy_from_hb.sh` | 从本机 libhb 源码仓库同步 `src/` 各库文件 |

## 测试

| 脚本 | 说明 |
| ---- | ---- |
| `test_control.sh` | httprelay 远程控制端到端测试（配合 libhb 私有仓库 `vsprojects/Release/uvmachine.exe`，维护者专用不随 release 发布） |

## remote/

远程构建（本地打包源码上传，不依赖远端 git 仓库，未 push 的本地改动可直接构建验证）：

| 脚本 | 说明 |
| ---- | ---- |
| `remote/env.sh` | 远程构建机环境变量（LINUX/MACOS_TARGET 等） |
| `remote/build_linux.sh java\|node` | 打包 src/swig/tools 上传 LINUX_TARGET，远端构建，产物拉回 `dist/` |
| `remote/build_macos.sh java\|node` | 同上，目标 MACOS_TARGET（构建机当前不可用，mkrelease 默认跳过） |

## 发版流程

1. 改根目录 `RELEASE_VERSION` 并在根目录 `CHANGELOG.md` 记录变更（含组件版本矩阵）；
2. 执行 `./mkrelease.sh`（或双击 `mkrelease.bat`）；
3. 产物上传 github release。
