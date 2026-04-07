#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

cd $CURRENT_DIR
echo "building linux java on ${LINUX_TARGET} with path ${LINUX_PROJECT_PATH}, branch ${PROJECT_BRANCH}"
ssh -t ${LINUX_TARGET} <<EOF
  #!/bin/bash
  echo "getting java home on linux server"
  # 方法1: 尝试通过update-alternatives
  if command -v update-alternatives >/dev/null 2>&1; then
      JAVA_PATH=$(update-alternatives --list java 2>/dev/null | head -1)
      if [ -n "$JAVA_PATH" ]; then
          JAVA_HOME=$(dirname "$(dirname "$JAVA_PATH")")
          echo "$JAVA_HOME"
          exit 0
      fi
  fi
  exit 0
  java -XshowSettings:properties -version 2>&1 | grep 'java.home' | cut -d'=' -f2 | xargs
  # cat java_home.txt
  # LINUX_JAVA_HOME=$(cat java_home.txt)
  # rm -f java_home.txt
  LINUX_JAVA_HOME=$(java -XshowSettings:properties -version 2>&1 | grep 'java.home' | cut -d'=' -f2 | xargs)
  echo "LINUX_JAVA_HOME: ${LINUX_JAVA_HOME}"
  # check if LINUX_JAVA_HOME is empty
  if [ -z "$LINUX_JAVA_HOME" ]; then
    echo "LINUX_JAVA_HOME is empty, please set it manually"
    exit 1
  fi
  export LINUX_JAVA_HOME
  echo "LINUX_JAVA_HOME: ${LINUX_JAVA_HOME}"
  export JAVA_HOME=${LINUX_JAVA_HOME}

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
  ./linux_java.sh 
  cd ../tools
  ./linux_java.sh
EOF

# download linux_java.zip to local dist
scp ${LINUX_TARGET}:${LINUX_PROJECT_PATH}/tools/linux_java.zip ./dist/linux_java.zip
# clean linux_java.zip on linux server
ssh -t ${LINUX_TARGET} "rm -f ${LINUX_PROJECT_PATH}/tools/linux_java.zip"