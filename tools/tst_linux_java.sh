#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

if [ $# -ne 1 ]; then
  export PROJECT_BRANCH=master
else
  export PROJECT_BRANCH=$1
fi

export LINUX_TARGET=root@192.168.50.67
export LINUX_PROJECT_PATH=/root/projects/ademco_hb
export LINUX_JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64


major=`cat ../src/libademco/ademco_version.h | grep ADEMCO_VERSION_MAJOR | head -n1 | awk '{print $3}'`
minor=`cat ../src/libademco/ademco_version.h | grep ADEMCO_VERSION_MINOR | head -n1 | awk '{print $3}'`
patch=`cat ../src/libademco/ademco_version.h | grep ADEMCO_VERSION_PATCH | head -n1 | awk '{print $3}'`

version=$major.$minor.$patch

echo "version: $version"

./mk_linux_java.sh