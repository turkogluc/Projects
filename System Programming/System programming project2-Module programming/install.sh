make
insmod ./scull.ko
mknod /dev/test c 247 0
chmod 777 /dev/test