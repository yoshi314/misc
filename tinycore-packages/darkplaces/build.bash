#!/usr/local/bin/bash

#libx11-xcb
#Xorg-7.6-dev
#compiletc
#svn
#SDL-dev

builddir=${PWD}

version=20121222

wget -c http://icculus.org/twilight/darkplaces/files/darkplacesengine${version}.zip
wget -c http://icculus.org/twilight/darkplaces/files/darkplacesmod${version}.zip




#if [ ! -d "darkplaces" ] ; then
#    svn co svn://svn.icculus.org/twilight/trunk/darkplaces
#else
#    cd darkplaces
#    svn up
#    cd ${builddir}
#fi



mkdir build-darkplaces

[ ! -e "darkplacesenginesource${version}.zip" ] && unzip darkplacesengine${version}.zip darkplacesenginesource${version}.zip
[ ! -d "darkplaces" ] && unzip darkplacesenginesource${version}.zip

cd darkplaces
make OPTIM_RELEASE="${CFLAGS}" DP_FS_BASEDIR=/usr/local/share/quake release

cd ${builddir}

rm -rf build-darkplaces
mkdir -p build-darkplaces/usr/local/{bin,share/quake}

cp darkplaces/darkplaces-{dedicated,glx,sdl} build-darkplaces/usr/local/bin


for size in 16 24 32 48 64 72; do
  mkdir -p build-darkplaces/usr/local/share/icons/hicolor/${size}x${size}/apps
  cp darkplaces/darkplaces${size}x${size}.png build-darkplaces/usr/local/share/icons/hicolor/${size}x${size}/apps/darkplaces.png
done


#---- the dpmod
rm -rf dpmod
unzip darkplacesmod${version}.zip
cp -a dpmod build-darkplaces/usr/local/share/quake


rm darkplaces.tcz
mksquashfs build-darkplaces darkplaces.tcz

rm darkplaces.tcz.dep.tmp
for binary in build-darkplaces/usr/local/bin/darkplaces* ; do 
    ../commonscripts/findlibs.bash ${binary} | xargs ../commonscripts/reducedeps.bash >> darkplaces.tcz.dep.tmp
done

sort darkplaces.tcz.dep.tmp | uniq -u > darkplaces.tcz.dep
rm darkplaces.tcz.dep.tmp

md5sum darkplaces.tcz > darkplaces.tcz.md5.txt
