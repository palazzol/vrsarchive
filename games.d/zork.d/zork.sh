: /bin/sh
#
#	If dindx.dat and dtext.dat do not exit in the user's home directory,
#	we copy them from /usr/games/lib, and will unlink them later.
#	Assuming every user can write their home directory, they should also
#	be able to save games there.
#
cd $HOME
UNLINK=""
DNGLIB=/usr/games/lib
for i in dindx.dat dtext.dat; do
  if test -r $i; then
    : User has a copy of $i
  else
    cp $DNGLIB/$i .
    UNLINK="$UNLINK $i"
  fi
done
$DNGLIB/dng $*
rm $UNLINK
