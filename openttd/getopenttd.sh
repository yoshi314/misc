#!/bin/bash

builddir=${PWD}

#required
#liblzma-dev
#lzo-dev
#SDL-dev


rm versions.txt
wget http://finger.openttd.org/versions.txt


version=$(grep releases versions.txt | grep stable | awk ' { print $1 }' | egrep '[0-9]')
gfxver=$(grep opengfx versions.txt  | awk ' { print $1 } ')
rm versions.txt


wget -c http://binaries.openttd.org/releases/${version}/openttd-${version}-source.tar.gz || exit 1
wget -c http://binaries.openttd.org/extra/opengfx/${gfxver}/opengfx-${gfxver}-all.zip || exit 1

mkdir openttd

unzip opengfx-${gfxver}-all.zip
tar xvf opengfx-${gfxver}.tar

tar xzf openttd-${version}-source.tar.gz

cd openttd-${version}
    ./configure  --prefix=/usr/local --binary-dir=bin --data-dir=share/openttd
    make -j3
    make INSTALL_DIR=${builddir}/openttd install
cd ${builddir}

mkdir openttd/usr/local/share/openttd/data
mv opengfx-${gfxver}/* openttd/usr/local/share/openttd/data
rm -rf opengfx-${gfxver}

[ -e "openttd.tcz" ] && rm openttd.tcz
mksquashfs openttd openttd.tcz -force-uid 1001 -force-gid 50
md5sum openttd.tcz > openttd.tcz.md5.txt


../commonscripts/findlibs.bash openttd/usr/local/bin/openttd | xargs ../commonscripts/reducedeps.bash > openttd.tcz.dep


