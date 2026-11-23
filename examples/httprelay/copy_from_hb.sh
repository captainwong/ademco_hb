#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

repo=/h/dev_hb/libhb/libuvlib
rm -rf ./uvlib
mkdir -p ./uvlib
cp -r ${repo}/llhttp ./uvlib/
cp ${repo}/*.h ./uvlib/
cp ${repo}/*.c ./uvlib/
cp ${repo}/README.md ${repo}/CHANGELOG.md ./uvlib/