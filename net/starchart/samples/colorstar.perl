$[ = 1;			# set array base to 1
$\ = "\n";		# set output record separator

while (<>) {
    chop;	# strip record separator
    if ((substr($_, 15, 1) eq 'C')) {
	$t = 'y9';
    } elsif ((substr($_, 15, 1) eq 'N')) {
	$t = 'g9';
    } elsif ((substr($_, 15, 1) eq 'G')) {
	$t = 'r9';
    } else {
        $t = '  ';
    }
    printf "%s%s%s\n", substr($_, 1, 16), $t, substr($_, 19, 999999);
}
