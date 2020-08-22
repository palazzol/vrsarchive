#
# First extract the data from regions.tbl file using emacs
# to split the data beginning at 14401 into 9537 lines
# of 48 characters each.  an easier method is:
#    dd if=regions.tbl of=reg.txt cbs=48 conv=unblock ibs=2880 skip=5
# FOLLOWED by head -9537 reg.txt > reg.txt.final
#
# The resulting file was processed using this
# perl script to produce the index.indx file for the GSC CD-ROM.
# You will need to set the parent directory of the CD-ROM below.
# If you have two CD-ROM readers for the two CD-ROMs, see
# "TWO READERS" below.
#
# perl is a replacement for shell awk and sed scripts, developed by
# Larry Wall at jpl-devvax.jpl.nasa.gov, where the latest version is
# avaliable via anonymous ftp.


$, = ":";
$\ = "\n";
while (<>) {
	$reg_no = substr($_,0,5);
	$ra_h_low = substr($_,7,2);
	$ra_m_low = substr($_,10,2);
	$ra_s_low = substr($_,13,5);
	$ra_h_hi = substr($_,19,2);
	$ra_m_hi = substr($_,22,2);
	$ra_s_hi = substr($_,25,5);
	$decsi_lo = substr($_,31,1);
	$dec_d_lo = substr($_,32,2);
	$dec_m_lo = substr($_,35,4);
	$decsi_hi = substr($_,40,1);
	$dec_d_hi = substr($_,41,2);
	$dec_m_hi = substr($_,44,4);


	$dec_hi = $dec_d_hi + $dec_m_hi/60.0;
	if ($decsi_hi eq "-") {$dec_hi *= -1;}

	$dec_lo = $dec_d_lo + $dec_m_lo/60.0;
	if ($decsi_lo eq "-") {$dec_lo *= -1;}

	$ra_hi = $ra_h_hi + $ra_m_hi/60.0 + $ra_s_hi / 3600.0;
	$ra_lo = $ra_h_low + $ra_m_low/60.0 + $ra_s_low / 3600.0;

	if ($ra_hi == 0.0) {$ra_hi = 24.0;}

	$dec_abs = $dec_lo;
	if ($dec_abs < 0.0) {$dec_abs = -$dec_abs;}
# Note: name of cdrom here should not be changed, change it below.
	$subdir = "/cdrom/gsc/";
	if ($decsi_hi eq "-") {
		$subdir .= "s";
	} else {
		$subdir .= "n";
	}
	
	if ($dec_abs < 7.5) {
		$subdir .= "0000";
	} elsif ($dec_abs < 15.00) {
		$subdir .= "0730";
	} elsif ($dec_abs < 22.50) {
		$subdir .= "1500";
	} elsif ($dec_abs < 30.00) {
		$subdir .= "2230";
	} elsif ($dec_abs < 37.50) {
		$subdir .= "3000";
	} elsif ($dec_abs < 45.00) {
		$subdir .= "3730";
	} elsif ($dec_abs < 52.50) {
		$subdir .= "4500";
	} elsif ($dec_abs < 60.00) {
		$subdir .= "5230";
	} elsif ($dec_abs < 67.50) {
		$subdir .= "6000";
	} elsif ($dec_abs < 75.00) {
		$subdir .= "6730";
	} elsif ($dec_abs < 82.50) {
		$subdir .= "7500";
	} else {
		$subdir .= "8230";
	}

	$subdir .= "/";

## TWO READERS
## At this point subdir should be modified if a second cdrom reader is
## to be used for the southern disk
#	if (($subdir =~ m"/s[0-9][0-9][0-9][0-9]/")
#		&& ($subdir !~ m"/s0000/")) {
##			On second disk
#			$subdir =~ s"cdrom"cdrom2";
#	}

# here, you can rename /cdrom/ to be the value appropriate for your
# site.
# e.g.:
#	$subdir =~ s"cdrom/"my/owncdrom/";
#	$subdir =~ s"cdrom2/"my/owncdrom2/";

	if ($dec_hi < $dec_lo) {
		$dec_up = $dec_lo;
		$dec_dn = $dec_hi;
	} else {
		$dec_up = $dec_hi;
		$dec_dn = $dec_lo;
	}

	printf "%9.6f %7.3f %9.6f %7.3f %s%04d.gsc gsc\n",
		$ra_hi, $dec_up,
		$ra_lo, $dec_dn,
		$subdir,
		$reg_no;
}

