#!/bin/bash
# usage: pack.sh java-linux|java-mac|node|node-mac
# 在 swig 构建完成后运行（本脚本通常由远端服务器调用）：
# 从 ../swig 产物收集打包为 <name>.zip
set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

target=$1
case $target in
java-linux)  name=linux_java; libext=so ;;
java-mac)    name=mac_java;   libext=dylib ;;
node)        name=linux_node ;;
node-mac)    name=mac_node ;;
*)
    echo "usage: $0 java-linux|java-mac|node|node-mac"
    exit 1
    ;;
esac

rm -rf ${name} ${name}.zip

case $target in
java-linux|java-mac)
    mkdir -p ${name}
    cd ${name}
    cp -r ../../swig/com/ .
    rm -f com/hb3344/ademco/*.class
    cp ../../swig/*.java .
    cp ../../swig/*.${libext} .
    cd ..
    ;;
node|node-mac)
    mkdir -p ${name}/build/Release
    cd ${name}
    cp -r ../../swig/build/Release/ademco_js.node ./build/Release/
    cp ../../swig/nodejs-demo.js .
    cd ..
    ;;
esac

cd ${name}
zip -r ../${name}.zip . *
cd ..
rm -rf ${name}/
