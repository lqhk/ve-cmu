#!/bin/bash

hostname
echo "Converting PPMs to JPGs..."

while [ 1 ];
do
	for i in $(seq -f %02g 1 24)
	do
		ppmtojpeg /live/${i}.ppm > /live/${i}.tmp.jpg
		mv /live/${i}.tmp.jpg /live/${i}.jpg
	done
	ppmtojpeg /live/thumb.ppm > /live/thumb.tmp.jpg
	mv /live/thumb.tmp.jpg /live/thumb.jpg
	#convert -geometry 160x120 /live/thumb.ppm /live/thumb_160x120.jpg
done

