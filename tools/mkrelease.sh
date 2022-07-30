#/bin/bash

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
host=192.168.2.107
ssh -t root@${host} "cd /root/projects/ademco_hb && git pull origin c && cd swig && ./linux_java.sh && cd ../tools && ./linux_java.sh"
scp root@${host}:/root/projects/ademco_hb/tools/linux_java.zip .
ssh -t root@${host} "rm -f /root/projects/ademco_hb/tools/linux_java.zip"

# macOS java
host=192.168.2.107
ssh -t jack@${host} "cd /Users/jack/projects/ademco_hb && git pull origin c && cd swig && ./mac_java.sh && cd ../tools && ./linux_java.sh"
scp jack@${host}:/Users/jack/projects/ademco_hb/tools/mac_java.zip .
ssh -t jack@${host} "rm -f /Users/jack/projects/ademco_hb/tools/mac_java.zip"



