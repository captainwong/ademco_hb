#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

if [ $# -ne 1 ]; then
  dst_name=linux_node
else
  dst_name=$1
fi

rm -rf ${dst_name} ${dst_name}.zip
mkdir -p ${dst_name}/build/Release
cd ${dst_name}
cp -r ../../swig/build/Release/ademco_js.node ./build/Release/
cp ../../swig/nodejs-demo.js .
zip -r ../${dst_name}.zip . *
cd ..
rm -rf ${dst_name}/
