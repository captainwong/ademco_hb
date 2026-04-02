#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

repo_emb=/h/dev_hb/libhb/libemb
repo_adm=/h/dev_hb/libhb/libademco
repo_hb=/h/dev_hb/libhb/libhb
repo_hbiot=/h/dev_hb/libhb/libhbiot
repo_check=/h/dev_hb/libhb/libhbcheck
repo_crypto=/h/dev_hb/libhb/libhbcrypto

mkdir -p ../src/libemb
cp -f ${repo_emb}/emb_config.h ../src/libemb/
cp -f ${repo_emb}/emb_bitrev.h ../src/libemb/
cp -f ${repo_emb}/emb_version.h ../src/libemb/
cp -f ${repo_emb}/libemb.h ../src/libemb/

mkdir -p ../src/libhb
cp -f ${repo_hb}/hb_config.h ../src/libhb/
cp -f ${repo_hb}/hb_version.h ../src/libhb/
cp -f ${repo_hb}/hbversion.h ../src/libhb/
cp -f ${repo_hb}/hb_core.h ../src/libhb/
cp -f ${repo_hb}/hb_core.c ../src/libhb/
cp -f ${repo_hb}/hb_core_ademco.h ../src/libhb/
cp -f ${repo_hb}/hb_core_ademco.c ../src/libhb/

mkdir -p ../src/libhbiot/rf
cp -f ${repo_hbiot}/hb_iot_config.h ../src/libhbiot/
cp -f ${repo_hbiot}/rf/hb_rf.c ../src/libhbiot/rf/
cp -f ${repo_hbiot}/rf/hb_rf.h ../src/libhbiot/rf/

mkdir -p ../src/libhbcheck
cp -f ${repo_check}/libhbcheck.h ../src/libhbcheck/
cp -f ${repo_check}/hb_check.h ../src/libhbcheck/
cp -f ${repo_check}/hbcheck_config.h ../src/libhbcheck/
cp -f ${repo_check}/hbcheck_version.h ../src/libhbcheck/

mkdir -p ../src/libhbcrypto
cp -f ${repo_crypto}/libhbcrypto.h ../src/libhbcrypto/
cp -f ${repo_crypto}/hbcrypto.h ../src/libhbcrypto/
cp -f ${repo_crypto}/hbcrypto.c ../src/libhbcrypto/
cp -f ${repo_crypto}/hbcrypto_config.h ../src/libhbcrypto/
cp -f ${repo_crypto}/hbcrypto_version.h ../src/libhbcrypto/

mkdir -p ../src/libademco
cp -f ${repo_adm}/libademco.h ../src/libademco/
cp -f ${repo_adm}/ademco.h ../src/libademco/
cp -f ${repo_adm}/ademco.c ../src/libademco/
cp -f ${repo_adm}/ademco_version.h ../src/libademco/
cp -f ${repo_adm}/ademco_config.h ../src/libademco/
cp -f ${repo_adm}/CHANGELOG.md ../src/libademco/
sed -i '/\[hb\]/d' ../src/libademco/CHANGELOG.md

