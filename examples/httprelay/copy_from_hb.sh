#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

mkdir -p ./uvlib
cp -r /g/dev_libs/ademco/uvlib/llhttp ./uvlib/
cp /g/dev_libs/ademco/uvlib/*.h ./uvlib/
cp /g/dev_libs/ademco/uvlib/*.c ./uvlib/