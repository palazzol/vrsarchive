$!
$! Define the location of the files on your system in the symbols below
$!	by Michael Campanella (campanella@cvg.enet.dec.com)
$!
$!   $Header: /home/Vince/cvs/net/starchart/starchart/starchart_init.com,v 1.1 1990-03-30 16:39:07 vrs Exp $
$!
$ sao_location := sys$games:[sao]	! change this for your system
$!
$ procedure = f$environment("procedure")
$ starchart_location = f$parse(procedure,,,"device") + f$parse(procedure,,,"directory")
$ observe_location = starchart_location - "]" + ".observe]"
$!
$ define/nolog starchart 'starchart_location'
$ define/nolog sao 'sao_location'
$!
$ stardsp :== $'starchart_location'stardsp
$ starpost :== $'starchart_location'starpost
$ startek :== $'starchart_location'startek
$ starx11 :== $'starchart_location'starx11
$ starxaw :== $'starchart_location'starxaw
$!
$ observe :== $'observe_location'observe
