#!/bin/sh

# config
#

sqfs=/dev/shm/portage
mkdir -p ${sqfs}
mkdir -p ${sqfs}-work
mkdir -p ${sqfs}-ro

tempmount=/data/build/ovfs
# place where the sqfs files are
location=/data/build

# /config

mkdir ${tempmount}

squashfile=${1:-$(ls -1t ${location}/portage-20* | head -1)}

# in case location was overwritten
# re-assign it
location=$(dirname ${squashfile})


[ ! -e "${squashfile}" ] && cleanup

# check for recent archives
#
# if there is an archive from last 30 minutes, don't sync again
#

howmany=$(find $location -maxdepth 1 -name "portage*.sqfs" -type f -mmin -30 | wc -l)

[ "$howmany" -gt 0 ] && exit 0



tempdir=$(mktemp -d)
#mount -o loop -t auto $squashfile $tempdir || exit 1



umount /usr/portage
mount -o loop -t auto $squashfile ${sqfs}-ro || exit 1

#overlayfs
#
# /usr/portage -r/o
# /dev/shm/portage  - r/w
# /dev/shm/portage-work - r/w for tracking changes
# ${tempmount} - final mount dir that's the resulting merged mount
#


mount -t overlay overlay -olowerdir=${sqfs}-ro,upperdir=${sqfs},workdir=${sqfs}-work /usr/portage

#
# rsync that shit
#

#rsync --stats --times --omit-dir-times --compress --whole-file -avr --delete-during rsync://rsync.pl.gentoo.org/gentoo-portage/ ${tempmount}/

emerge --sync

#find ${tempmount} -name "ChangeLog" -delete

#rsync --stats --times --omit-dir-times --compress --whole-file -avr --delete-during --exclude="*ChangeLog" rsync://rsync.pl.gentoo.org/gentoo-portage/ ${tempmount}/

#
# pack that shit
#

date=$(date +%F-%H-%M-%S)
#mksquashfs ${tempmount} ${location}/portage-${date}.sqfs -comp lz4 -Xhc -progress
mksquashfs /usr/portage ${location}/portage-${date}.sqfs -comp xz -progress

#
# clean up that shit
#

umount ${tempmount} && rm -rf ${sqfs} && rm -rf ${sqfs}-work
umount ${tempdir} && rm -rf ${tempdir} 
umount /usr/portage || exit 1

rm -rf ${tempmount}


#
# most recent file linked as portage-latest.sqfs
# last good file as portage-prev.sqfs
# 

cd ${location}
#mv portage-latest.sqfs portage-prev.sqfs
ln -sf ${squashfile}  portage-prev.sqfs
ln -sf portage-${date}.sqfs portage-latest.sqfs
mount /usr/portage

#
# clean up old sqfs files from 7+ days
#

find ${location} -maxdepth 1 -name "portage*sqfs" -mtime +7 -delete


# this is gentoo specific, remove if needed
mv /var/cache/eix/portage.eix /var/cache/eix/portage.eix.old

layman -s ALL 

eix-update
eix-diff /var/cache/eix/portage.eix.old /var/cache/eix/portage.eix

