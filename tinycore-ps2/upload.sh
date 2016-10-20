#!/bin/ash


cd ~/opl
if git describe --tags > /dev/null ; then
    rev=$(git describe --tags)
else
    rev=""
fi

echo "this script will copy your OPL build to the pendrive"

echo "make sure your usb is properly formatted, plugged in and enabled via virtualbox"

echo "attempting to access pendrive"
sudo mount -t auto /dev/sda1 /home/tc/pendrive



if [ -n "$rev" ] ; then
    echo "files will be in $rev directory"
    target=/home/tc/pendrive/$rev
    sudo mkdir -p $target
else
    target=/home/tc/pendrive
fi

if mount | grep pendrive ; then
	echo "pendrive mounted successfully"
	echo "copying files ... "
	[ -f ~/opl/OPNPS2LD.ELF ] && sudo cp ~opl/OPNPS2LD.ELF $target
	[ -f ~/opl/opl.elf ] && sudo cp  ~/opl/opl.elf $target
	echo " .. trying to unmount the pendrive"
	sudo umount /dev/sda1 && echo " .. umount successful"
	[ -f ~/opl/OPNPS2LD.ELF ] && echo "OPL copied as OPNPS2LD.ELF"
	[ -f ~/opl/opl.elf ] && echo "OPL copied as opl.elf"
else
	echo "failed to access pendrive"
	echo "check your VM settings"
fi

