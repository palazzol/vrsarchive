:
:	getto	- Find possible paths to a remote machine user
:
if test -f /etc/systemid; then
  HERE=`cat /etc/systemid`
else
  HERE=`awk '{ if ($2 == "sysname") print $3 }' /etc/systemid`
fi
case $# in
  1) set `echo $1 | awk -F@ '{ print $1, $2, $3; }'` ;;
esac
case $# in
  2) USER="$1"
     SITE="$2"
     for i in `getfrom $HERE $SITE`; do
       echo $i!$USER
     done ;;
  *) echo "Usage: $0 user@site"
     exit 1 ;;
esac
