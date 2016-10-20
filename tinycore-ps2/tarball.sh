#!/bin/sh

rm -rf open-ps2-loader

[ -e "master.zip" ] && rm master.zip 

echo "this will attempt to fetch most recent tarball off github"
echo "you should set http_proxy variable if using a proxy beforehand"

echo "e.g export http_proxy=10.1.1.5:8080"

echo "waiting for 3 seconds, you can press ctrl+c to abort if you need to"

sleep 3

wget https://github.com/ifcaro/Open-PS2-Loader/archive/master.zip

echo "unpacking"

unzip master.zip 


rm master.zip
ls -sf Open-PS2-Loader-master opl


echo "all ready now, hopefully"
