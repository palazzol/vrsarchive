GAMLIB=/usr/games/lib
$GAMLIB/letter | nroff | tr '[A-Z][a-z]' '[N-Z][A-M][n-z][a-m]' | more -s
