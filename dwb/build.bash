#!/bin/bash

builddir=${PWD}

if [ ! -d "dwb" ] ; then
    hg clone http://bitbucket.org/portix/dwb
else
    cd dwb
    hg pull
    hg update
fi

mkdir build-dwb

sed -i -e 's/^PREFIX=.*/PREFIX=\/usr\/local/g' dwb/config.mk

cd dwb
make -j3
make DESTDIR=${builddir}/build-dwb install
cp tools/grab_adblocker ${builddir}/build-dwb/usr/local/bin/dwb_grab_adblocker.bash

cd ${builddir}

mksquashfs build-dwb dwb.tcz

../commonscripts/findlibs.bash build-dwb/usr/local/bin/dwb | xargs ../commonscripts/reducedeps.bash > dwb.tcz.dep.tmp
../commonscripts/findlibs.bash build-dwb/usr/local/bin/dwbem | xargs ../commonscripts/reducedeps.bash >> dwb.tcz.dep.tmp

uniq -u dwb.tcz.dep.tmp > dwb.tcz.dep
rm dwb.tcz.dep.tmp