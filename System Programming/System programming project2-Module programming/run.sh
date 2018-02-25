rmmod ./scull.ko
make
insmod ./scull.ko
echo "@root message-one-123-aaaaaaaaaaaaa" > /dev/test
cat /dev/test