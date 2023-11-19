#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

ssh -t ${LINUX_TARGET} <<EOF
  cd ${LINUX_PROJECT_PATH}
  git reset --hard HEAD
  git pull
  git checkout ${PROJECT_BRANCH} 
  git pull origin ${PROJECT_BRANCH}
  chmod +x ./swig/*.sh ./tools/*.sh
  cd swig 
  ./linux_mac_node.sh 
  cd ../tools
  ./linux_mac_node.sh
EOF

scp ${LINUX_TARGET}:${LINUX_PROJECT_PATH}/tools/linux_node.zip ./dist/linux_node.zip
ssh -t ${LINUX_TARGET} "rm -f ${LINUX_PROJECT_PATH}/tools/linux_node.zip"