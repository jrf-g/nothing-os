#!/bin/bash
IMG="build/nothingos.img"
DEV="/dev/sdb"

echo "installing NothingOS to storage or block device sdb."
sudo dd if=build/nothingos.img of=/dev/sdb bs=1M status=progress
echo "almost done... syncing changes, commiting data, flushing buffers"
sync
echo "you can now eject the media. to finish installation, copy over sdb to your boot drive. you may like to stay around while we verify integrity."
echo "loading verifiers (it is safe to unplug right now, make sure to press ctrl-c first though!)"
sleep 10
echo "verifing... (ctrl-c before unplug please)"
COUNT=64

echo "Comparing first $COUNT MB..."

IMG_HASH=$(dd if="$IMG" bs=1M count=$COUNT status=none | md5sum | awk '{print $1}')
DEV_HASH=$(sudo dd if="$DEV" bs=1M count=$COUNT status=none | md5sum | awk '{print $1}')

echo "Image partial MD5:  $IMG_HASH"
echo "Device partial MD5: $DEV_HASH"

if [ "$IMG_HASH" = "$DEV_HASH" ]; then
    echo "✔ Partial match — First $COUNT MB verified. retesting"
else
    echo "✘ Partial mismatch — Write may be corrupted."
fi
echo "Calculating MD5 of image..."
IMG_HASH=$(md5sum "$IMG" | awk '{print $1}')
echo "Image MD5: $IMG_HASH"

echo "Calculating MD5 of device..."
DEV_HASH=$(sudo dd if="$DEV" bs=4M status=none | md5sum | awk '{print $1}')
echo "Device MD5: $DEV_HASH"

if [ "$IMG_HASH" = "$DEV_HASH" ]; then
    echo "✔ MD5 MATCH — Write verified. retesting"
else
    echo "✘ MD5 MISMATCH — Write failed or incomplete."
fi
echo "Calculating SHA256 of image..."
IMG_HASH=$(sha256sum "$IMG" | awk '{print $1}')
echo "Image SHA256: $IMG_HASH"

echo "Calculating SHA256 of device..."
DEV_HASH=$(sudo dd if="$DEV" bs=4M status=none | sha256sum | awk '{print $1}')
echo "Device SHA256: $DEV_HASH"

if [ "$IMG_HASH" = "$DEV_HASH" ]; then
    echo "✔ SHA256 MATCH — Write verified. retesting"
else
    echo "✘ SHA256 MISMATCH — Write failed or incomplete."
fi
