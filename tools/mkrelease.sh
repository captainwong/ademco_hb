#!/bin/bash

# examples
./clean.sh
./examples.bat
./examples_x64.bat
mkdir examples
mkdir examples/x86
mkdir examples/x64
cp "/e/dev/AlarmCenterTestTools/Release/直连型主机模拟器V1.13.exe" examples/x86/
cp "/g/dev_libs/libevent-2.1.12-stable-install/lib/event_core.dll" examples/x86/
cp ../examples/Win32/Release/server_demo.exe examples/x86/
cp ../examples/Release/ademco.dll examples/x86/
cp ../examples/Release/ademco.lib examples/x86/
cp ../examples/x64/Release/server_demo_x64.exe examples/x64/
cp ../examples/x64/Release/ademco.dll examples/x64/
cp ../examples/x64/Release/ademco.lib examples/x64/
rm -f examples.zip
cd examples
zip -r ../examples.zip . *
cd ..
rm -rf examples

# win-java
./clean.sh
./win_java.bat
mkdir -p win_java
cp -r ../swig/com/ win_java/
rm -f win_java/com/hb3344/ademco/*.class
cp ../swig/*.java win_java/
cp ../swig/*.dll win_java/
rm -f win_java.zip
cd win_java
zip -r ../win_java.zip . *
cd ..
rm -rf win_java


# win-c#
./clean.sh
./win_csharp.bat
mkdir -p win_csharp
cp -r ../swig/com/ win_csharp/
cp ../swig/*.cs win_csharp/
cp ../swig/*.dll win_csharp/
cp ../swig/*.exe win_csharp/
rm -f win_csharp.zip
cd win_csharp
zip -r ../win_csharp.zip . *
cd ..
rm -rf win_csharp


# ubuntu20.04 java
ssh -t root@192.168.2.107 "cd /root/projects/ademco_hb && git checkout master && git reset --hard HEAD && git pull origin master && cd swig && export JAVA_HOME='/usr/lib/jvm/java-8-openjdk-amd64' && ./linux_java.sh && cd ../tools && ./linux_java.sh"
scp root@192.168.2.107:/root/projects/ademco_hb/tools/linux_java.zip .
ssh -t root@192.168.2.107 "rm -f /root/projects/ademco_hb/tools/linux_java.zip"

# macOS java
ssh -t jack@JackMacBook-Pro.local "cd /Users/jack/projects/ademco_hb && git checkout master && git reset --hard HEAD && git pull origin master && cd swig && source ~/.zshrc && export JAVA_HOME='/Library/Java/JavaVirtualMachines/jdk-9.0.1.jdk/Contents/Home' && ./mac_java.sh && cd ../tools && ./mac_java.sh"
scp jack@JackMacBook-Pro.local:/Users/jack/projects/ademco_hb/tools/mac_java.zip .
ssh -t jack@JackMacBook-Pro.local "rm -f /Users/jack/projects/ademco_hb/tools/mac_java.zip"



