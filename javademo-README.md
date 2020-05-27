现在已经支持 `Java` 通过 `JNI` 调用 `C++` 的动态链接库。

## Linux

*测试环境使用 `Ubuntu16.04` + `java-8-openjdk-amd64`，用户环境可以自行修改 `javademo.sh`。*

执行 `./javademo.sh`

## Windows

1. `javac -encoding utf-8 javademo/jni/AdemcoHbLibrary.java && javah -d javademo/cpp -jni javademo.jni.AdemcoHbLibrary`

2. 打开 `ademco_hb.sln`, 以 `Release/x64` 编译 `ademco_java` 项目，生成 `x64/Release/ademco_hb.dll`

3. `java javademo.jni.SimpleServer 12345` 进行测试

## 作者对 `Java` 开发不熟悉，有任何意见、建议欢迎提 `issue` 及 `pr`

