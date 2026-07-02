echo "installing NothingOS to storage or block device sdb."
sudo dd if=build/nothingos.img of=/dev/sdb bs=1M status=progress
echo "to finish installation, copy over sdb to your boot drive"
