#!/bin/bash -e
cd `dirname $0`
NEWLIB_NAME=newlib-3.3.0

PWD=`pwd`
TMP=${PWD}/../tmp
mkdir -p ${TMP}
NEWLIB_TAR=${TMP}/${NEWLIB_NAME}.tar.gz
NEWLIB_SRC_DIR=${TMP}/${NEWLIB_NAME}
NEWLIB_BUILD_DIR=${TMP}/newlib_build
NEWLIB_OUT=${PWD}/../vendor/newlib

if [ ! -e ${NEWLIB_TAR} ]; then
	echo Downloading ${NEWLIB_NAME}.tar.gz
	curl -o ${NEWLIB_TAR} http://sourceware.org/pub/newlib/${NEWLIB_NAME}.tar.gz
fi
tar -xf ${NEWLIB_TAR} -C ${TMP}

mkdir -p ${NEWLIB_BUILD_DIR}

echo $CC

cd ${NEWLIB_BUILD_DIR}
${NEWLIB_SRC_DIR}/newlib/configure \
  --target=x86_64-elf --disable-multilib \
  --prefix=${NEWLIB_OUT} \
  CFLAGS="-g -nostdlibinc -O2 -target x86_64-unknown-none-elf -mcmodel=large"
make -j1
make install
