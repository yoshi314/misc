#!/bin/bash


rm versions.txt
wget http://finger.openttd.org/versions.txt


version=$(grep releases versions.txt | grep stable | awk ' { print $1 }' | egrep '[0-9]')
gfxver=$(grep opengfx versions.txt  | awk ' { print $1 } ')
rm versions.txt


wget -c http://binaries.openttd.org/releases/${version}/openttd-${version}-linux-generic-i686.tar.gz


wget -c http://binaries.openttd.org/extra/opengfx/${gfxver}/opengfx-${gfxver}-all.zip

mkdir -p openttd/usr/local/openttd

tar xvzpf openttd-${version}-linux-generic-i686.tar.gz --strip-components=1 -C openttd/usr/local/openttd
unzip opengfx-${gfxver}-all.zip

mv opengfx-${gfxver}/* openttd/usr/local/openttd/data
rm -rf opengfx-${gfxver}


mkdir -p openttd/usr/local/share/{applications,pixmaps}
cp openttd/usr/local/openttd/media/openttd.desktop openttd/usr/local/share/applications
cp openttd/usr/local/openttd/media/openttd*.png openttd/usr/local/share/pixmaps
cp openttd/usr/local/openttd/media/openttd*.xpm openttd/usr/local/share/pixmaps

sed -i -e 's|\(Exec=\).*|\1/usr/local/openttd/openttd|g' openttd/usr/local/share/applications/openttd.desktop


[ -e "openttd.tcz" ] && rm openttd.tcz
mksquashfs openttd openttd.tcz -force-uid 1001 -force-gid 50
md5sum openttd.tcz > openttd.tcz.md5.txt
