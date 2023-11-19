#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

ssh -t ${MACOS_TARGET} <<EOF
  cd ${LINUX_PROJECT_PATH}
  git reset --hard HEAD
  git pull
  git checkout ${PROJECT_BRANCH} 
  git pull
  cd swig
  ./linux_mac_node.sh 
  cd ../tools
  ./linux_mac_node.sh mac_node
EOF

scp ${MACOS_TARGET}:${LINUX_PROJECT_PATH}/tools/mac_node.zip ./dist/mac_node.zip
ssh -t ${MACOS_TARGET} "rm -f ${LINUX_PROJECT_PATH}/tools/mac_node.zip"