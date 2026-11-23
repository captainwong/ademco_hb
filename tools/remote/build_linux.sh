#!/bin/bash
# usage: build_linux.sh java|node
# 本地打包源码(src/swig/tools)上传到 LINUX_TARGET -> 远端构建并打包(pack.sh) -> 拉回产物到 dist/
# 不依赖远端 git 仓库，本地未 push 的改动也可直接构建验证
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
    swig_build=linux_java.sh
    pack_target=java-linux
    zip_name=linux_java
    export_env="export JAVA_HOME=${LINUX_JAVA_HOME}"
    ;;
node)
    swig_build=linux_mac_node.sh
    pack_target=node
    zip_name=linux_node
    # node 装在 nvm 下，非交互 ssh 不加载 nvm，需显式指定 PATH（v22.22.2 为最高支持版本）
    export_env="export PATH=\$HOME/.nvm/versions/node/v22.22.2/bin:\$PATH"
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
scp ${tarball} ${LINUX_TARGET}:/tmp/

echo "building linux ${what} on ${LINUX_TARGET} at ${LINUX_PROJECT_PATH}"
# 3. 远端解压、构建、打包
ssh -t ${LINUX_TARGET} <<EOF
  #!/bin/bash
  set -e
  ${export_env}

  mkdir -p ${LINUX_PROJECT_PATH}
  rm -rf ${LINUX_PROJECT_PATH}/*
  tar -xzf /tmp/${tarball} -C ${LINUX_PROJECT_PATH}
  rm -f /tmp/${tarball}
  # 源码可能携带 Windows CRLF，统一剥离以保证可执行
  find ${LINUX_PROJECT_PATH} -name '*.sh' -exec sed -i 's/\r\$//' {} +
  chmod +x ${LINUX_PROJECT_PATH}/swig/*.sh ${LINUX_PROJECT_PATH}/tools/*.sh
  cd ${LINUX_PROJECT_PATH}/swig
  ./${swig_build}
  cd ../tools
  ./pack.sh ${pack_target}
EOF

# 4. 拉回产物
mkdir -p ../dist
scp ${LINUX_TARGET}:${LINUX_PROJECT_PATH}/tools/${zip_name}.zip ../dist/${zip_name}.zip

# 5. 清理远端 zip 与本地源码包
ssh ${LINUX_TARGET} "rm -f ${LINUX_PROJECT_PATH}/tools/${zip_name}.zip"
rm -f ${tarball}
