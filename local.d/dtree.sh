echo $1
find $1 -type d -print | \
tr / \\1 | \
sort -f | \
tr \\1 / |\
sed -e s,\^$1,, -e /\^$/d -e "s,[^/]*/, \"	,g"
