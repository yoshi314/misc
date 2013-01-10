#!/usr/local/bin/bash

# eliminate inter-package dependencies and minimize dependency list

for dep in "$@" ; do 
    depfiles="$depfiles /etc/sysconfig/tcedir/optional/$dep.dep" 
done

for dep in "$@" ; do 
    grep -q "$dep" ${depfiles} 2>/dev/null || echo "$dep" 
done