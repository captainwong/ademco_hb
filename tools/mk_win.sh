#!/bin/bash
# usage: mk_win.sh java|csharp|node
# swig 生成绑定 -> VS 编译 -> 收集产物到 dist/<name>.zip
set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

name=win_$1
if [ "$1" != "java" ] && [ "$1" != "csharp" ] && [ "$1" != "node" ]; then
    echo "usage: $0 java|csharp|node"
    exit 1
fi

./clean.sh
./build_win.bat $1
mkdir -p dist/${name}

case $1 in
java)
    cp -r ../swig/com/ dist/${name}/
    rm -f dist/${name}/com/hb3344/ademco/*.class
    cp ../swig/*.java dist/${name}/
    cp ../swig/*.dll dist/${name}/
    ;;
csharp)
    cp -r ../swig/com/ dist/${name}/
    cp ../swig/*.cs dist/${name}/
    cp ../swig/*.dll dist/${name}/
    cp ../swig/*.exe dist/${name}/
    ;;
node)
    cp -r ../swig/build/ dist/${name}/
    cp ../swig/nodejs-demo.js dist/${name}/
    ;;
esac

rm -f dist/${name}.zip
cd dist/${name}
zip -r ../${name}.zip . *
cd ..
rm -rf ${name}
