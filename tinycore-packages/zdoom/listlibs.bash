#!/usr/local/bin/bash

# display packages containing required libraries

if [ $# -lt 1 ] ; then

    echo "Usage : $0 /path/to/executable"
    exit 1
fi

app=$1

for lib in $(ldd ${app} | grep -v found | grep -v linux-gate | awk ' { print  }' | sed -e '/^$/d' -e 's/.*\///g'  ); do 
    find /tmp/tcloop -name "$lib"
done | sort #| awk -F/ ' { print $4 }' | uniq | sed 's/\(.*\)/\1.tcz/g'




