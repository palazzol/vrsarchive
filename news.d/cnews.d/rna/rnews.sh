#
# rnews  (M. J. Liebelt, University of Adelaide, May 1984)
#
# netgets files sent to "news" as "newsitem"s and pipes them into
# postnews, then reads any mail sent to "news" and pipes that into
# $LIBDIR/uurec, which passes the news portions onto postnews.
#
# It should be run as "/etc/su news rnews " (i.e. with uid = news)
#
# News should be sent to news at this host from remote hosts thus:
#      ... | /usr/lib/news/uusend news:thishost   (to send by mail, or)
#      ... | net -hthishost -nnews -f -Nnewsitem -M
#
#    where "thishost" is the NETID of this host.
#
# LIBDIR - same as LIBDIR in Makefile
# BINDIR - same as BINDIR in Makefile
# NETDIR - where netget lives
LIBDIR=/usr/lib/news
BINDIR=/bin
NETDIR=/bin

cd %news

while $NETDIR/netget newsitem > /dev/null 2>&1
do
        $BINDIR/postnews -p < newsitem > rnews.errors 2>&1
        if [ -s rnews.errors ]
        then
               cat $LIBDIR/rnews.mail1 rnews.errors $LIBDIR/rnews.mail2 newsitem | mail root
        fi
done

if [ -r .mail ]
then {
        mv .mail newsmail
        $LIBDIR/uurec < newsmail > rnews.errors 2>&1
        if [ -s rnews.errors ]
        then
                cat $LIBDIR/rnews.mail1 rnews.errors $LIBDIR/rnews.mail2 newsmail | mail root
        fi
        }
fi

rm -f newsitem newsmail rnews.errors
