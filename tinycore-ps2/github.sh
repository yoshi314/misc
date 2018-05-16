#!/bin/sh

rm -rf ~/Open-PS2-Loader

echo "doing initial checkout of git repository"

git clone https://github.com/ifcaro/Open-PS2-Loader 
rm ~/opl
ln -s Open-PS2-Loader opl
