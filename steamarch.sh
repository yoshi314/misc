#!/bin/sh

# steam chroot directory
chroot_dir="/mnt/arch"

# mount chroot directories

(mount | grep "${chroot_dir}/dev" )|| mount -R /dev "${chroot_dir}/dev"
(mount | grep "${chroot_dir}/sys" ) || mount -R /sys "${chroot_dir}/sys"
(mount | grep "${chroot_dir}/proc" ) || mount -t proc proc "${chroot_dir}/proc"

# check for an existing dbus session
if [ -z "${DBUS_SESSION_BUS_ADDRESS}" ] ; then
  chroot "${chroot_dir}" dbus-launch
fi

# chroot, substitute user, and start steam
chroot "${chroot_dir}" su steam -c "dbus-launch steam"

