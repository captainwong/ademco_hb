#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

cd $CURRENT_DIR
ssh -t ${MACOS_TARGET} <<EOF
  cd ${MACOS_PROJECT_PATH}
  git checkout ${PROJECT_BRANCH} 
  git reset --hard HEAD
  git pull origin ${PROJECT_BRANCH} 
  cd swig 
  ./mac_java.sh 
  cd ../tools
  ./mac_java.sh
EOF

scp ${MACOS_TARGET}:${MACOS_PROJECT_PATH}/tools/mac_java.zip .
ssh -t ${MACOS_TARGET} "rm -f ${MACOS_PROJECT_PATH}/tools/mac_java.zip"
