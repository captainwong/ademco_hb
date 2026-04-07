#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

cd $CURRENT_DIR
echo "building linux java on ${LINUX_TARGET} with path ${LINUX_PROJECT_PATH}, branch ${PROJECT_BRANCH}"
ssh -t ${LINUX_TARGET} <<EOF
  # test if '/root/projects/ademco_hb' exists
  if [ ! -d "${LINUX_PROJECT_PATH}" ]; then
    git clone https://github.com/captainwong/ademco_hb.git ${LINUX_PROJECT_PATH}
  fi
  cd ${LINUX_PROJECT_PATH}
  git reset --hard HEAD
  git pull
  git checkout ${PROJECT_BRANCH} 
  git pull origin ${PROJECT_BRANCH}
  chmod +x ./swig/*.sh ./tools/*.sh
  cd swig 
  LINUX_JAVA_HOME=$(java -XshowSettings:properties -version 2>&1 | grep 'java.home' | cut -d'=' -f2 | xargs)
  export JAVA_HOME=${LINUX_JAVA_HOME}
  ./linux_java.sh 
  cd ../tools
  ./linux_java.sh
EOF

# download linux_java.zip to local dist
scp ${LINUX_TARGET}:${LINUX_PROJECT_PATH}/tools/linux_java.zip ./dist/linux_java.zip
# clean linux_java.zip on linux server
ssh -t ${LINUX_TARGET} "rm -f ${LINUX_PROJECT_PATH}/tools/linux_java.zip"