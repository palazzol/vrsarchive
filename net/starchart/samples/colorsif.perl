$[ = 1;			# set array base to 1
$, = $FS = ';';
$\ = "\n";		# set output record separator

while (<>) {
    chop;	# strip record separator
    @Fld = split(/[;\n]/, $_, 999);
    if (($Fld[4] =~ /^C./)) {
	$Fld[5] = 'y9';
    } elsif (($Fld[4] =~ /^N./)) {
	$Fld[5] = 'g9';
    } elsif (($Fld[4] =~ /^G./)) {
	$Fld[5] = 'r9';
    }
    print @Fld;
}
