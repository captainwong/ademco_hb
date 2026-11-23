#!/bin/bash
# usage: build_macos.sh java|node
# 本地打包源码(src/swig/tools)上传到 MACOS_TARGET -> 远端构建并打包(pack.sh) -> 拉回产物到 dist/
# 不依赖远端 git 仓库，本地未 push 的改动也可直接构建验证
# 注意：mac 构建机当前不可用，mkrelease.sh 中默认跳过
set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR
. ./env.sh

what=$1
if [ "$what" != "java" ] && [ "$what" != "node" ]; then
    echo "usage: $0 java|node"
    exit 1
fi

case $what in
java)
    swig_build=mac_java.sh
    pack_target=java-mac
    zip_name=mac_java
    export_java="export JAVA_HOME=${MACOS_JAVA_HOME}"
    ;;
node)
    swig_build=linux_mac_node.sh
    pack_target=node-mac
    zip_name=mac_node
    export_java=""
    ;;
esac

REPO_ROOT=$( cd ../.. ; pwd -P )
tarball=ademco_hb-src.tar.gz

# 1. 本地打包源码（排除构建产物与 .git、examples 等远端构建不需要的内容）
rm -f ${tarball}
tar -czf ${tarball} -C ${REPO_ROOT} \
    --exclude='.git' \
    --exclude='examples' \
    --exclude='swig/build' \
    --exclude='swig/com' \
    --exclude='tools/dist' \
    --exclude='tools/*.zip' \
    --exclude='tools/*.tar.gz' \
    src swig tools

# 2. 上传源码包
scp ${tarball} ${MACOS_TARGET}:/tmp/

echo "building macos ${what} on ${MACOS_TARGET} at ${MACOS_PROJECT_PATH}"
# 3. 远端解压、构建、打包
ssh -t ${MACOS_TARGET} <<EOF
  #!/bin/bash
  set -e
  ${export_java}

  mkdir -p ${MACOS_PROJECT_PATH}
  rm -rf ${MACOS_PROJECT_PATH}/*
  tar -xzf /tmp/${tarball} -C ${MACOS_PROJECT_PATH}
  rm -f /tmp/${tarball}
  # 源码可能携带 Windows CRLF，统一剥离以保证可执行
  find ${MACOS_PROJECT_PATH} -name '*.sh' -exec sed -i 's/\r\$//' {} +
  chmod +x ${MACOS_PROJECT_PATH}/swig/*.sh ${MACOS_PROJECT_PATH}/tools/*.sh
  cd ${MACOS_PROJECT_PATH}/swig
  ./${swig_build}
  cd ../tools
  ./pack.sh ${pack_target}
EOF

# 4. 拉回产物
mkdir -p ../dist
scp ${MACOS_TARGET}:${MACOS_PROJECT_PATH}/tools/${zip_name}.zip ../dist/${zip_name}.zip

# 5. 清理远端 zip 与本地源码包
ssh ${MACOS_TARGET} "rm -f ${MACOS_PROJECT_PATH}/tools/${zip_name}.zip"
rm -f ${tarball}
