#!/bin/bash

#---------sdk------------

die() {
    echo "$@"
    exit 1
}

thisdir=${PWD}


echo "loading development tools into the system"
tce-load -i compiletc || true


rm -rf ${PS2SDK}

mkdir -p ${PS2SDK}

echo "* ps2sdk *"
cd ps2sdk
make  > ~/build.log || die "failed to build ps2sdk"
make install > /dev/null

cd "${thisdir}"

echo "* smap *"
cd ps2eth
make >> ~/build.log || die "failed to build smap driver"
mkdir -p ${PS2DEV}/ps2eth/smap-new
cp smap-new/ps2smap.irx ${PS2DEV}/ps2eth/smap-new

echo "* gsKit *"
cd "${thisdir}/gsKit"
make >> ~/build.log || die "failed to build gsKit"
make install >> ~/build.log || die "failed to install gsKit"


echo "* zlib *"
cd "${thisdir}/ps2sdk-ports/zlib"
make install >> ~/build.log || die "failed to build zlib"

echo "* libpng *"
cd "${thisdir}/ps2sdk-ports/libpng"
make install >> ~/build.log || die "failed to build libpng"

echo "* libjpeg *"
cd "${thisdir}/ps2sdk-ports/libjpeg"
make install >> ~/build.log || die "failed to build libjpg"

echo "* freetype *"
cd "${thisdir}/ps2sdk-ports/freetype-2.4.12"
./SetupPS2.sh >> ~/build.log || die "failed to build freetype"

echo "* ps2-packer *"
cd "${thisdir}/ps2-packer"
make install >> ~/build.log || die "failed to build ps2-packer"


cd "${thisdir}"

echo "Seems things got built correctly"
echo "To preserve your changes between vm reboots, "
echo "shutdown the vm through menu->Exit"
echo "and make sure the 'backup' is enabled"
