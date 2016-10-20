# ~/.ashrc: Executed by SHells.
#

 echo " == attempting to update and rebuild OPL == "

 echo "in case of conflicts during checkout, you could try resetting the repo to original state"
 echo "simplest way to do this is to run the ./tarball.sh script, or you could use git"
 echo "cd ~/opl ; git clean -xdf ; git reset --hard"
 echo "then re-run this script"

 cd ~/opl
 git pull


 if git describe --tags ; then
     rev=$(git describe --tags)
    echo "building OPL rev $rev"
 else
     rev=""
     echo "building OPL"
 fi

 make rebuild

 if [ -f OPNPS2LD.ELF ] ; then
 	echo "OPL $rev built succesfully"
 	oplrev=$rev
 fi

 if [ -f opl.elf ] ; then
     echo "OPL $rev (unpacked) built successfully"
     oplrev=$rev
 fi
 
 echo "in order to copy files to USB device run ./upload.sh command"
 
