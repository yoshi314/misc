#!/bin/sh

#updater script

die() {
    echo "error : $1"
    exit 1
    rm -rf tmp
}

update() {
    file=$1
    hash=$2
    location=$3

    if [ ! -f "$location/$file" ] ; then
        old_md5="nofile"
    else
        old_md5=$(md5sum ${location}/${file} | cut -d' ' -f1)
    fi
    md5=$hash

    if [ "$old_md5" != "$md5" ]; then
	echo " * updating ${location}/${file}"
	wget --no-check-certificate -q ${base_url}/${file} -O tmp/${file}

	check=$(md5sum tmp/${file} | cut -d' ' -f1)
	if [ "$check" != "$hash" ]; then
		die "checksum for ${file} failed"
		rm tmp/${file}
		exit 1
	fi

	sudo mv /home/tc/tmp/$file $location
    fi
}

base_url="http://dl.dropbox.com/u/3873935/vm2_upd"

echo "this script will attempt to update your vm"
echo "checking script for updates..."

mkdir tmp > /dev/null 2>&1

#me=$(whoami)

wget --no-check-certificate ${base_url}/update.sh -O tmp/tmp.sh

self_md5=$(md5sum $0 | cut -d' ' -f1)
new_md5=$(md5sum tmp/tmp.sh | cut -d' ' -f1)

if [ "$self_md5" != "$new_md5" ];then
	cp $0 old_update.sh
	cp tmp/tmp.sh $0
	echo "script updated. please run it again"
	chmod +x $0
	exit 1
fi

wget --no-check-certificate ${base_url}/updates -O tmp/updates || die "failed to grab filelist"

#by default /mnt/hda1
if [ ! -d /mnt/hda1 ] ; then
    disk="\/mnt\/sda1"
else
    disk="\/mnt\/hda1"
fi

sed -i -e "s/@disk@/$disk/g" tmp/updates

exec < tmp/updates
while read line
do
    update $line
done

sudo chown tc *.sh
sudo chmod +x *.sh
sudo chmod +x /home/tc/.setbackground

chmod +x /home/tc/sources/*.sh

echo "update script finished"
echo

