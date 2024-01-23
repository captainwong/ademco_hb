#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

mkdir -p ./uvlib
repo=/h/dev_hb/libhb
cp -r ${repo}/uvlib/llhttp ./uvlib/
cp ${repo}/uvlib/*.h ./uvlib/
cp ${repo}/uvlib/*.c ./uvlib/