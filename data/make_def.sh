#!/bin/sh

case "$1" in
	poweron)
		IMG_H=$1_image.h
		PART_H=$1_part.h
		;;
	poweroff)
		IMG_H=$1_image.h
		PART_H=$1_part.h
		;;
	*)
		echo "usage) $0 {poweron|poweroff}"
		echo "   ex) # $0 poweron"
		exit 1
		;;
esac

if [ -e $IMG_H ]; then
	mv $IMG_H $IMG_H.old
fi
touch $IMG_H

if [ -e $PART_H ]; then
	mv $PART_H $PART_H.old
fi
touch part.$$

LASTIMG=

cd image

for file in ./$1/*.png; do
	echo "image: \"$file\" LOSSY 85;" >> ../$IMG_H
	echo "tween: \"$file\";" >> ../part.$$
	LASTIMG=$file
done

cd ..

echo "normal: \"$LASTIMG\";" > $PART_H
cat part.$$ >> $PART_H
rm part.$$

