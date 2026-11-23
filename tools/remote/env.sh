#!/bin/bash
# 远程构建环境变量，被 build_linux.sh / build_macos.sh source
# 分支由调用方（mkrelease.sh）通过 PROJECT_BRANCH 传入，默认 master
if [ -z "$PROJECT_BRANCH" ]; then
    export PROJECT_BRANCH=master
fi

export LINUX_TARGET=root@192.168.50.67
export LINUX_PROJECT_PATH=/root/projects/ademco_hb
export LINUX_JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64
export MACOS_TARGET=jack@192.168.50.231
export MACOS_PROJECT_PATH=/Users/jack/projects/ademco_hb
export MACOS_JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk1.8.0_291.jdk/Contents/Home
