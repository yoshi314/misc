#!/bin/ash
echo "this script will copy your OPL build to the VirtualBox share"

echo "attempting to access your virtualbox share"


cd ~/opl
if git describe --tags > /dev/null ; then
    rev=$(git describe --tags)
else
    rev=""
fi

if [ $# -lt 1 ]; then
	echo "================================================================"
	echo "error : you need to add the shared directory name as a parameter"
	echo "        e.g. $0 share1234"
	echo "        assuming your shared directory name is \"share1234\""
	echo "        obviously directory cannot be shared as read-only ;)"
	echo "tip   : try naming the share in lower-case, and without spaces"
	echo "        to avoid potential problems"
	echo "================================================================"
	exit 1
fi

echo "loading virtualbox module vboxvfs"
sudo modprobe vboxvfs

echo "mounting shared directory $1 under /home/tc/pendrive ... "
sudo mount.vboxsf $1 /home/tc/pendrive


target=/home/tc/pendrive

if [ -n "$rev" ] ; then
    target="${target}/$rev"
    mkdir -p $target
fi


if mount | grep pendrive ; then
	echo "shared folder mounted successfully"
	
	echo "copying files ... "
    cp -v ~/opl/*.elf $target/
    cp -v ~/opl/*.ELF $target/
	echo " .. trying to unmount the shared dir"
	sudo umount pendrive && echo " .. umount successful"
	echo "check your shared dir for files"
else
	echo "failed to write files"
	echo "check your VM settings"
fi

