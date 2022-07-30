#!/bin/bash

# win-java
./clean.sh
./win_java.bat
mkdir -p win_java
cd win_java
cp -r ../../swig/com/ .
cp ../../swig/*.java .
cp ../../swig/*.dll .
cd ..
rm -f win_java.zip
zip win_java.zip win_java
rm -rf win_java

# win-c#
./clean.sh
./win_csharp.bat
mkdir -p win_csharp
cd win_csharp
cp -r ../../swig/com/ .
cp ../../swig/*.cs .
cp ../../swig/*.dll .
cp ../../swig/*.exe .
cd ..
rm -f win_csharp.zip
zip win_csharp.zip win_csharp
rm -rf win_csharp

# ubuntu20.04 java
ssh -t root@192.168.2.107 "cd /root/projects/ademco_hb && git reset --hard HEAD && git pull origin c && cd swig && export JAVA_HOME='/usr/lib/jvm/java-8-openjdk-amd64' && ./linux_java.sh && cd ../tools && ./linux_java.sh"
scp root@192.168.2.107:/root/projects/ademco_hb/tools/linux_java.zip .
ssh -t root@192.168.2.107 "rm -f /root/projects/ademco_hb/tools/linux_java.zip"

# macOS java
ssh -t jack@wangjackdeMacBook-Pro "cd /Users/jack/projects/ademco_hb && git reset --hard HEAD && git pull origin c && cd swig && source ~/.zshrc && export JAVA_HOME='/Library/Java/JavaVirtualMachines/jdk-9.0.1.jdk/Contents/Home' && ./mac_java.sh && cd ../tools && ./mac_java.sh"
scp jack@wangjackdeMacBook-Pro:/Users/jack/projects/ademco_hb/tools/mac_java.zip .
ssh -t jack@wangjackdeMacBook-Pro "rm -f /Users/jack/projects/ademco_hb/tools/mac_java.zip"



