#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

cp -f /g/dev_libs/ademco/hb/ademco.h ../
cp -f /g/dev_libs/ademco/hb/ademco.c ../
cp -f /g/dev_libs/ademco/hb/hb_com.h ../
cp -f /g/dev_libs/ademco/hb/hb_com.c ../
cp -f /g/dev_libs/ademco/hb/ademco_version.h ../
cp -f /g/dev_libs/ademco/hb/ademco_config.h ../
cp -f /g/dev_libs/ademco/hb/CHANGELOG.md ../
sed -i '/\[hb\]/d' ../CHANGELOG.md
