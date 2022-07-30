#!/bin/bash

# server-demo
# ./clean.sh
# ./server_demo.bat
# ./server_demo_x64.bat
# cp ../examples/Win32/Release/server_demo.exe .
# cp ../examples/x64/Release/server_demo_x64.exe .
# rm -f server_demo.zip
# zip server_demo.zip server_demo.exe server_demo_x64.exe
# rm -f server_demo.exe server_demo_x64.exe


# win-java
# ./clean.sh
# ./win_java.bat
# mkdir -p win_java
# cp -r ../swig/com/ win_java/
# rm -f win_java/com/hb3344/ademco/*.class
# cp ../swig/*.java win_java/
# cp ../swig/*.dll win_java/
# rm -f win_java.zip
# cd win_java
# zip -r ../win_java.zip . *
# cd ..
# rm -rf win_java


# win-c#
# ./clean.sh
# ./win_csharp.bat
# mkdir -p win_csharp
# cp -r ../swig/com/ win_csharp/
# cp ../swig/*.cs win_csharp/
# cp ../swig/*.dll win_csharp/
# cp ../swig/*.exe win_csharp/
# rm -f win_csharp.zip
# cd win_csharp
# zip -r ../win_csharp.zip . *
# cd ..
# rm -rf win_csharp


# ubuntu20.04 java
# ssh -t root@192.168.2.107 "cd /root/projects/ademco_hb && git reset --hard HEAD && git pull origin c && cd swig && export JAVA_HOME='/usr/lib/jvm/java-8-openjdk-amd64' && ./linux_java.sh && cd ../tools && ./linux_java.sh"
# scp root@192.168.2.107:/root/projects/ademco_hb/tools/linux_java.zip .
# ssh -t root@192.168.2.107 "rm -f /root/projects/ademco_hb/tools/linux_java.zip"

# macOS java
ssh -t jack@wangjackdeMacBook-Pro "cd /Users/jack/projects/ademco_hb && git reset --hard HEAD && git pull origin c && cd swig && source ~/.zshrc && export JAVA_HOME='/Library/Java/JavaVirtualMachines/jdk-9.0.1.jdk/Contents/Home' && ./mac_java.sh && cd ../tools && ./mac_java.sh"
scp jack@wangjackdeMacBook-Pro:/Users/jack/projects/ademco_hb/tools/mac_java.zip .
ssh -t jack@wangjackdeMacBook-Pro "rm -f /Users/jack/projects/ademco_hb/tools/mac_java.zip"



