echo "
                         Nov. 16, 1984


TTY	Baud		      User		Login
====================================================="
case $# in
  0) pattern="^" ;;
  *) pattern="^ $1 " ;;
esac
grep -y "$pattern" <<!
 0      9600		Printer
 1    300/1200		Dial-in Modem
 2      9600		Dial-Out Modem
 3      9600		Teltone
 4      9600		empty user port
 5      9600		empty user port
 6      9600		empty user port
 7      9600		empty user port
 8      9600		empty user port
 9      9600		empty user port
 a      9600		empty user port
 b      9600		empty user port
 c      9600		empty user port
 d      9600		Vincent R. Slyngstad	vrs
 e      9600		Bart Kessler		bart
 f      9600		Direct Connection	apssys
!
