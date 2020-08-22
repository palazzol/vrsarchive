# This awk program takes a "mapwin" file produced by starXaw or
# starX11 and changes the variables width, height, x_offset, and y_offset
# to match the scale of starpost.
# the numbers 4, 36, and 72 in this file as distributed are constants
# in starpost.  If starpost is changed, these numbers also must change
#
# This may be wrapped in a shell script, e.g.
# awk -f /usr/local/lib/star/postconv.awk
#
BEGIN {FS="="}
{if (($1 == "width") || ($1 == "height"))
		{printf "%s=%.0f\n", $1, $2*4}
	else if ($1 == "x_offset")
		{printf "%s=%.0f\n", $1, $2*4+36}
	else if ($1 == "y_offset")
		{printf "%s=%.0f\n", $1, $2*4+72}
	else {print $0}
}
