#!/bin/ash


cd ~/opl
rev=$(git describe --tags)

echo "this script will copy your OPL build to the pendrive"

echo "make sure your usb is properly formatted, plugged in and enabled via virtualbox"

echo "attempting to access pendrive"
sudo mount -t auto /dev/sda1 /home/tc/pendrive


echo "files will be in $rev directory"
sudo mkdir /home/tc/pendrive/$rev

if mount | grep pendrive ; then
	echo "pendrive mounted successfully"
	echo "copying files ... "
	[ -f ~/opl/OPNPS2LD.ELF ] && sudo cp  opl/OPNPS2LD.ELF /home/tc/pendrive/$rev
	[ -f ~/opl/opl.elf ] && sudo cp  opl/opl.elf /home/tc/pendrive/$rev
	echo " .. trying to unmount the pendrive"
	sudo umount /dev/sda1 && echo " .. umount successful"
	[ -f ~/opl/OPNPS2LD.ELF ] && echo "OPL copied as OPNPS2LD.ELF"
	[ -f ~/opl/opl.elf ] && echo "OPL copied as opl.elf"
else
	echo "failed to access pendrive"
	echo "check your VM settings"
fi

