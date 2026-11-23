#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

if [ $# -ne 1 ]; then
  export PROJECT_BRANCH=master
else
  export PROJECT_BRANCH=$1
fi

my_macbook_is_broken=1

major=`cat ../src/libademco/ademco_version.h | grep ADEMCO_VERSION_MAJOR | head -n1 | awk '{print $3}'`
minor=`cat ../src/libademco/ademco_version.h | grep ADEMCO_VERSION_MINOR | head -n1 | awk '{print $3}'`
patch=`cat ../src/libademco/ademco_version.h | grep ADEMCO_VERSION_PATCH | head -n1 | awk '{print $3}'`

version=$major.$minor.$patch

echo "version: $version"

rm -rf dist
mkdir -p dist

# examples (windows x86/x64)
./mk_examples.sh

# windows: java / c# / node
./mk_win.sh java
./mk_win.sh csharp
./mk_win.sh node

# ubuntu20.04: java / node（本地打包上传构建，不依赖远端 git）
./remote/build_linux.sh java
./remote/build_linux.sh node

# macOS: java / node（构建机当前不可用，默认跳过）
if [ $my_macbook_is_broken -eq 0 ]; then
  ./remote/build_macos.sh java
  ./remote/build_macos.sh node
fi

mv dist ademco_hb-$version
tar -zcvf ademco_hb-$version.tar.gz ademco_hb-$version
zip -r ademco_hb-$version.zip ademco_hb-$version
rm -rf ademco_hb-$version
