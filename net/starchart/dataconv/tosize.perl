
# perl is language/program by Larry Wall, posted to comp.sources.unix
# and available from several archives.

#
# 1) emulate tosize C program if only -d -s -m and/or numerical
#     arguments are given.
# 2) if one file is given, it's sif, and if the tosize field is
#     numeric, convert and replace with the encoded string.
# 3) if two files are given, the first is sif and the second contains
#     sizes.
#
# sizes are in seconds of arc by default, or minutes or degrees if m or d is
# appended.  e.g. '1.2m' is 1.2 minutes = 72 seconds, '3.4d' = 12240 seconds.
#

$alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
$, = ' ';
$\ = "\n";

$sepchar = ';';
$multiplier = 1;
for (@ARGV) {
	if (-r) {push(@filenames, $_);}
	elsif (/^-d$/) {$multiplier = 3600;}
	elsif (/^-m$/) {$multiplier = 60;}
	elsif (/^-s$/) {$multiplier = 1;}
	elsif (/^sepchar=/) {
		@schar = /^sepchar=(.)/;
		$sepchar = @schar[0];}
	else {print &tosze($_*$multiplier);}
}
#print $#filenames;



if ($#filenames > 1) {
	die "$0: [sif.file [sizefile]]\nExit ";
} elsif ($#filenames == 1) {
	open(SIF, @filenames[0])
		|| die "Error, couldn't open sif file @filenames[0]:$!";
	open(SIZEF, @filenames[1])
		|| die "Error, couldn't open size file @filenames[1]:$!";
	$, = $sepchar;
	while (<SIF>) {
		chop;
		@Flds = split(/$sepchar/);
		$sstr = <SIZEF>;
		$sze = &tosze($sstr);
		@Flds[5] = $sze;
		print @Flds;
	}
} elsif ($#filenames == 0) {
	open(SIF, @filenames[0])
		|| die "Error, couldn't open @filenames[0]:$!";
	$, = $sepchar;
	while (<SIF>) {
		chop;
		@Flds = split(/$sepchar/);
		@Flds[5] = &tosze(@Flds[5])
				unless (@Flds[5] =~ /^[A-Z][0-9]$/);
		print @Flds;
	}
} else {
	while (<STDIN>) {
		print &tosze($_);
	}
}


sub tosze {
local($szestr) = @_[0];
$_ = $szestr;
($sze, $magchar) = /([0-9.]*)([dms]*)$/;
if ($magchar eq 'd') { $sze *= 3600.0;}
elsif ($magchar eq 'm') { $sze *= 60.0;}

if ($sze < 99.5) {
	$sst = sprintf("%2d", $sze);
} elsif ($sze < 995.0) {
	$sst = sprintf("%2d", (($sze + 5.0)/10.0));
	substr($sst,0,1) = substr($alphabet, (($sze+5)/100.0)-1, 1);
} elsif ($sze < 9950.0) {
	$sst = sprintf("%2d", (($sze + 50.0)/100.0));
	substr($sst,0,1) = substr($alphabet, (($sze+50)/1000.0)+8, 1);
  } elsif ($sze < 89500.0) {
	$sst = sprintf("%2d", (($sze + 500.0)/1000.0));
	substr($sst,0,1) = substr($alphabet, (($sze+500)/10000.0)+17, 1);
  } else {$sst = 'Z9';}

  return $sst;
}
