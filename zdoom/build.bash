#!/usr/local/bin/bash

#needs fmod 4.26 atm
#adapted from archlinux's pkgbuild


builddir=${PWD}

#make sure to load the following extensions prior to building first : 

#SDL-dev
#fluidsynth-dev
#compiletc
#cmake
#nasm
#yasm
#svn
#bzip2-dev [optional]

_fmodver=42636
_fmodarch=linux

wget -c http://www.fmod.org/index.php/release/version/fmodapi${_fmodver}${_fmodarch}.tar.gz

tar xvxf fmodapi${_fmodver}${_fmodarch}.tar.gz

_fmodlibver=${_fmodver:0:1}.${_fmodver:1:2}.${_fmodver:3:2}
_fmodlibname=libfmodex


[ ! -d zdoom ] && svn co http://mancubus.net/svn/hosted/zdoom/zdoom/trunk zdoom

svn up zdoom

cp fmodapi${_fmodver}${_fmodarch}/api/lib/${_fmodlibname}-${_fmodlibver}.so zdoom/libfmodex.so

cd zdoom

#patch -p1 < ../doom-share-dir.patch 
patch -p1 < ../stack-noexec.patch

# fallback fluidsynth-patches location (changed to /usr/local/share/sounds/sf2 )
sed -i -e 's/share\/sounds\/sf2/local\/share\/sounds\/sf2/g' src/sound/music_fluidsynth_mididevice.cpp

# or you could export fluid_patchset cvar to zdoom to make it look elsewhere

# GTK

#build it

cmake -DNASM_PATH=/usr/local/bin/nasm \
    -DFMOD_INCLUDE_DIR="../fmodapi${_fmodver}${_fmodarch}/api/inc" \
    -DFMOD_LIBRARY=libfmodex.so \
    -DCMAKE_SKIP_RPATH=TRUE . \
    -DCMAKE_BUILD_TYPE=Release 

#this is so stupid

cmake -DNASM_PATH=/usr/local/bin/nasm \
    -DFMOD_INCLUDE_DIR="../fmodapi${_fmodver}${_fmodarch}/api/inc" \
    -DFMOD_LIBRARY=libfmodex.so \
    -DCMAKE_SKIP_RPATH=TRUE . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -DSHARE_DIR=\"\\\"/usr/local/share/zdoom\\\"\""

make -j3

cd ${builddir}

mkdir -p build-gtk/usr/local/{bin,share/zdoom,lib}

strip zdoom/zdoom

cp zdoom/zdoom build-gtk/usr/local/bin
cp zdoom/libfmodex.so build-gtk/usr/local/lib
cp zdoom/*.pk3 build-gtk/usr/local/share/zdoom

rm zdoom.tcz
mksquashfs build-gtk zdoom-gtk.tcz
md5sum zdoom-gtk.tcz > zdoom-gtk.tcz.md5.txt
../commonscripts/findlibs.bash build-gtk/usr/local/bin/zdoom | xargs ../commonscripts/reducedeps.bash > zdoom-gtk.tcz.dep

# no-gtk
cd zdoom

make clean

# since cmake already ran a few times, only one invocation is necessary 

# libXcursor dep can be omitted by using
#     -DXCURSOR_HEADER="" -DXCURSOR_LIB="" \

# this will remove Xcursor from xorg-libs package from deps (600kb deps less)
# not sure what this lib is used for in zdoom exactly. let's keep it for now

cmake -DNO_GTK=ON \
    -DNASM_PATH=/usr/local/bin/nasm \
    -DFMOD_INCLUDE_DIR="../fmodapi${_fmodver}${_fmodarch}/api/inc" \
    -DFMOD_LIBRARY=libfmodex.so \
    -DCMAKE_SKIP_RPATH=TRUE . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -DSHARE_DIR=\"\\\"/usr/local/share/zdoom\\\"\""


make -j3

cd ${builddir}

mkdir -p build/usr/local/{bin,share/zdoom,lib}

strip zdoom/zdoom

cp zdoom/zdoom build/usr/local/bin
cp zdoom/libfmodex.so build/usr/local/lib
cp zdoom/*.pk3 build/usr/local/share/zdoom

rm zdoom.tcz
mksquashfs build zdoom.tcz
md5sum zdoom.tcz > zdoom.tcz.md5.txt
../commonscripts/findlibs.bash build/usr/local/bin/zdoom | xargs ../commonscripts/reducedeps.bash > zdoom.tcz.dep

