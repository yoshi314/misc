#!/bin/bash

builddir=${PWD}

if [ ! -d "pacman" ] ; then
    git clone git://projects.archlinux.org/pacman.git
else
    cd pacman
    git pull
    cd ${builddir}
fi

cd pacman
./autogen.sh
./configure --disable-doc --with-libcurl


make -j3 || exit 1
make DESTDIR=${builddir}/build-pacman install

cd ${builddir}

rm pacman.tcz
mksquashfs build-pacman pacman.tcz

rm pacman.tcz.dep.tmp
for file in build-pacman/usr/local/bin/pacman build-pacman/usr/local/lib/*.so ; do 
    ../commonscripts/findlibs.bash ${file} | xargs ../commonscripts/reducedeps.bash >> pacman.tcz.dep.tmp
done

sort pacman.tcz.dep.tmp | uniq > pacman.tcz.dep
rm pacman.tcz.dep.tmp
