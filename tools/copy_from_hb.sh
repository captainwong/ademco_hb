#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

repo=/h/dev_hb/libhb/libhb

cp -f ${repo}/ademco.h ../
cp -f ${repo}/ademco.c ../
cp -f ${repo}/hb_com.h ../
cp -f ${repo}/hb_com.c ../
cp -f ${repo}/ademco_version.h ../
cp -f ${repo}/ademco_config.h ../
cp -f ${repo}/CHANGELOG.md ../
sed -i '/\[hb\]/d' ../CHANGELOG.md
