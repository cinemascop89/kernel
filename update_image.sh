 #!/bin/sh -e
 mkdir -p ./mnt
 sudo mount -o loop,offset=1048576 disk.img ./mnt
 sudo cp kernel.bin ./mnt/boot
 sudo cp ./grub/grub.cfg ./mnt/boot/grub
 sync
 sudo umount ./mnt
