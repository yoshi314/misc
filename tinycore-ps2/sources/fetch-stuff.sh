#!/bin/bash

thisdir=${PWD}

for repo in ps2sdk gsKit ps2-packer ps2sdk-ports ps2eth ps2toolchain ; do
    if [ ! -d "$repo" ] ; then
	echo "* cloning $repo"
	git clone https://github.com/ps2dev/${repo}.git
    else
        echo "* updating $repo"
	cd ${repo}
	git pull
	cd ${thisdir}
    fi
done

