#
#	Recompute what online manual pages exist.
cd $MANDIR
for i in man?; do
  cd $i
  for p in *; do
    echo `soelim $p | \
          sed -n '/^.TH/{
                          s/[^ 	]*[ 	]*//
                          s/[^ 	]*[ 	]*//
                          s/[ 	].*//
                          s/.*/(&)/
                          p
                  }
                  /NAME/{
                          n
                          s/[^\n]*\n//
                          s/\\\\//
                          p
                          q
                  }'
         `
  done
  cd ..
done | \
awk '{ printf "%s %s\t", $2, $1;
       for (i=3; i <= NF; i++)
         printf "%s ", $i;
       printf "\n";
     }' | \
sort >$WHATIS
chmod 644 $WHATIS
