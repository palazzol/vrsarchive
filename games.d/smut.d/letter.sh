#!/bin/sh
GAMLIB=/usr/games/lib
PAGER=more
$GAMLIB/letter | tr '[A-Z][a-z]' '[N-Z][A-M][n-z][a-m]' | $PAGER
