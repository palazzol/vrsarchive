echo -n "Name of utility or feature?  "
read feature
FEATURE=`echo $feature | tr '[a-z]' '[A-Z]'`
Feature=`expr $FEATURE : '\(.\)'``expr $feature : '.\(.*\)'`
echo -n "Short one line description?  "
read desc
echo -n "Section?  "
read section
echo -n "Usage?  "
read usage
date=`date +%D`
cat <<! >$feature.$section
.TH $FEATURE $section $date
.SH NAME
$feature \- $desc
.SH SYNOPSIS
.B $usage
.SH DESCRIPTION
.I $Feature
needs a summary here of how to use the utility or feature.
.SH FILES
.ta 1.5i
.nf
file	Use of file
.fi
.SH BUGS
Known limitations go here.
!
