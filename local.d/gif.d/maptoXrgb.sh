LIBDIR=/usr/local/lib
COLORMAP=$LIBDIR/Xrgb.gif
for i in $*; do
	gray2clr -u <$i |
	fbext -w800 -h600 |
	fbsharp 2 |
	fbquant -G -m$COLORMAP >/tmp/`basename $i .gif`.16
done
