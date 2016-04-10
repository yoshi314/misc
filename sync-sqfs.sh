#!/bin/sh


if [ $# -lt 1 ] ; then 
	echo "$0 <squashfs>"
	exit 1
fi


squashfile=$1

tempdir=$(mktemp -d)


mount -o loop -t auto $squashfile $tempdir


#
# work dirs
#

sqfs=/dev/shm/portage
mkdir -p ${sqfs}
mkdir -p ${sqfs}-work

tempmount=/mnt/storage/ovfs

mkdir ${tempmount}



# check for recent archives

#
# if there is an archive from last 30 minutes, don't sync again
#
where=$(dirname $0)

howmany=$(find $where -maxdepth 1 -name "portage*.sqfs" -type f -mmin -30 | wc -l)

[ "$howmany" -gt 0 ] && exit 0




#overlayfs
#
# /usr/portage -r/o
# /dev/shm/portage  - r/w
# /dev/shm/portage-work - r/w for tracking changes
# ${tempmount} - final mount dir that's the resulting merged mount
#

mount -t overlay overlay -olowerdir=${tempdir},upperdir=/dev/shm/portage,workdir=/dev/shm/portage-work ${tempmount}

#
# rsync that shit
#

rsync --stats --times --omit-dir-times --compress --whole-file -avr --delete-during --exclude="*ChangeLog" rsync://rsync.pl.gentoo.org/gentoo-portage/ ${tempmount}/
#rsync --stats --times --omit-dir-times --compress --whole-file -avr --delete-during --exclude="*ChangeLog" rsync://rsync.pl.gentoo.org/gentoo-portage/ ${tempmount}/

#
# pack that shit
#

date=$(date +%F-%H-%M-%S)
#mksquashfs ${tempmount} /mnt/storage/portage-${date}.sqfs -comp lz4 -Xhc -progress
mksquashfs ${tempmount} /mnt/storage/portage-${date}.sqfs -comp xz -progress

#
# clean up that shit
#

umount ${tempmount} && rm -rf ${sqfs} && rm -rf ${sqfs}-work
umount ${tempdir} && rm -rf ${tempdir} || exit 1
umount /usr/portage || exit 1

rm -rf ${tempmount}


#
# most recent file linked as portage-latest.sqfs
#

cd /mnt/storage
mv portage-latest.sqfs portage-prev.sqfs
ln -sf portage-${date}.sqfs portage-latest.sqfs
mount /usr/portage

#
# clean up old sqfs files from 7+ days
#


find /mnt/storage -maxdepth 1 -name "portage*sqfs" -mtime +7 -delete

