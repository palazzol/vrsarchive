: /bin/sh
sync;sleep 1
sync;sync
if test -x /etc/haltsys
  exec /etc/haltsys
else
  exec uadmin 2 0
fi
