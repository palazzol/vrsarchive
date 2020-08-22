#!/bin/csh -f
set log = /mip/usr/games/lib/conquest/runlog
setenv SHELL /bin/csh
setenv USER mwp
setenv HOME /f/hons/mwp
/bin/date >>&! $log
/mip/usr/games/conquest -x >>&! $log

set day = `/bin/date | /bin/awk '{ print $1 }'`
set hour = `/bin/date | /bin/awk '{ print $4 }' | /bin/awk -F: '{ print $1 }'`
switch ($day)
	case Sat:
		set nextday = mon
		breaksw
	default:
		set nextday =
		breaksw
endsw
switch ($hour)
	case 00:
		set nexthour = 1500
		breaksw
	default:
		set nexthour = 0000
		breaksw
endsw
/usr/bin/at $nexthour $nextday /mip/usr/games/lib/conquest/run >>&! $log
