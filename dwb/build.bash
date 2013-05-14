#!/bin/bash

builddir=${PWD}

if [ ! -d "dwb" ] ; then
    git clone http://bitbucket.org/portix/dwb.git
else
    cd dwb
    git pull
    cd ${builddir}
fi

mkdir build-dwb

sed -i -e 's/^PREFIX=.*/PREFIX=\/usr\/local/g' dwb/config.mk

cd dwb
make -j3 || exit 1
make DESTDIR=${builddir}/build-dwb install
cp tools/grab_adblocker ${builddir}/build-dwb/usr/local/bin/dwb_grab_adblocker.bash

mkdir -p ${builddir}/build-dwb/usr/local/doc/dwb
cp ${builddir}/readme ${builddir}/usr/local/doc/dwb

cd ${builddir}

rm dwb.tcz
mksquashfs build-dwb dwb.tcz

../commonscripts/findlibs.bash build-dwb/usr/local/bin/dwb | xargs ../commonscripts/reducedeps.bash > dwb.tcz.dep.tmp
../commonscripts/findlibs.bash build-dwb/usr/local/bin/dwbem | xargs ../commonscripts/reducedeps.bash >> dwb.tcz.dep.tmp

sort dwb.tcz.dep.tmp | uniq > dwb.tcz.dep
rm dwb.tcz.dep.tmp