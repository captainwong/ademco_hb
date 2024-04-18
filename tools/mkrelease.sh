#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

if [ $# -ne 1 ]; then
  export PROJECT_BRANCH=master
else
  export PROJECT_BRANCH=$1
fi

export LINUX_TARGET=root@192.168.50.107
export LINUX_PROJECT_PATH=/root/projects/ademco_hb
export LINUX_JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
export MACOS_TARGET=jack@JackMacBook-Pro.local
export MACOS_PROJECT_PATH=/Users/jack/projects/ademco_hb
export MACOS_JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk1.8.0_291.jdk/Contents/Home

major=`cat ../ademco_version.h | grep ADEMCO_VERSION_MAJOR | head -n1 | awk '{print $3}'`
minor=`cat ../ademco_version.h | grep ADEMCO_VERSION_MINOR | head -n1 | awk '{print $3}'`
patch=`cat ../ademco_version.h | grep ADEMCO_VERSION_PATCH | head -n1 | awk '{print $3}'`

version=$major.$minor.$patch

echo "version: $version"

rm -rf dist
mkdir -p dist

# examples
./mk_examples.sh

# win-java
./mk_win_java.sh

# win-c#
./mk_win_csharp.sh

# ubuntu20.04 java
./mk_linux_java.sh

# macOS java
./mk_macos_java.sh

# win-node
./mk_win_node.sh

# linux-node
./mk_linux_node.sh

# mac-node
./mk_macos_node.sh

mv dist ademco_hb-$version
tar -zcvf ademco_hb-$version.tar.gz ademco_hb-$version
zip -r ademco_hb-$version.zip ademco_hb-$version
rm -rf ademco_hb-$version
