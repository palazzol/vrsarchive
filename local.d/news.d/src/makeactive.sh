: "Create active file and newsgroup hierarchy for new machine"
: "Usage: sh makeactive.sh LIBDIR SPOOLDIR NEWSUSR NEWSGRP"
: '@(#)makeactive	1.9	9/4/84'
LIBDIR=$1
SPOOLDIR=$2
NEWSUSR=$3
NEWSGRP=$4
if test ! -s $LIBDIR/localgroups
then
	cat <<"E_O_F" >$LIBDIR/localgroups
general	Articles that should be read by everyone on your local system
E_O_F
fi
cat <<"E_O_F" > /tmp/$$groups
net.abortion		Abortion.
net.adm.site		Automatic maintenance of the USENET directory.
net.ai			Artificial intelligence.
net.analog		Analog design developments, ideas, and components.
net.announce		General announcements of interest to all.
net.announce.newusers	General announcements for new users.
net.arch		Computer architecture.
net.astro		Astronomy.
net.astro.expert	Astronomy for experts.
net.audio		High fidelity audio.
net.auto		Automobiles, automotive products and laws.
net.aviation		Aviation rules, means, and methods.
net.bicycle		Bicycles, related products and laws.
net.bio			Biology and related sciences.
net.books		Books of all genres, shapes, and sizes.
net.bugs		General bug reports and fixes.
net.bugs.2bsd		UNIX version 2BSD related bugs.
net.bugs.4bsd		UNIX version 4BSD related bugs.
net.bugs.usg		USG UNIX (System III, V, etc.) related bugs.
net.bugs.uucp		UUCP related bugs.
net.bugs.v7		UNIX V7 related bugs.
net.chess		Chess and computer chess.
net.cog-eng		Cognitive engineering.
net.college		College, college activities, campus life, etc.
net.columbia		The space shuttle and the STS program.
net.comics		The funnies, old and new.
net.consumers		Consumer interests, product reviews, etc.
net.cooks		Food, cooking, cookbooks, and recipes.
net.crypt		Different methods of data en/decryption.
net.cse			Computer science education.
net.cycle		Motorcycles and related products and laws.
net.dcom		Data communications hardware and software.
net.decus		DEC User's Society newsgroup.
net.emacs		EMACS editors of different flavors.
net.eunice		The Eunice system.
net.flame		Flaming on any topic.
net.followup		Followups to articles in net.general.
net.games		Games and computer games.
net.games.emp		The computer game Empire.
net.games.frp		Fantasy Role Playing games.
net.games.go		Go.
net.games.pbm		Play by Mail games.
net.games.rogue		Rogue.
net.games.trivia	Trivia.
net.games.video		Video games.
net.garden		Gardening, methods and results.
net.general		*Important* and timely announcements of interest to all.
net.graphics		Computer graphics, art, and animation.
net.ham-radio		Amateur Radio practices, contests, events, rules, etc.
net.info-terms		All sorts of terminals.
net.invest		Investments and the handling of money.
net.jobs		Job announcements, requests, etc.
net.jokes		Jokes and other humor.  Some may be offensive.
net.jokes.d		Discussions on the content of net.jokes.
net.kids		Children, their behavior and activities.
net.lan			Local area network hardware and software.
net.lang		Computer languages in general.
net.lang.ada		The computer language Ada.
net.lang.apl		The computer language APL.
net.lang.c		The computer language C.
net.lang.f77		The computer language FORTRAN.
net.lang.forth		The computer language Forth.
net.lang.lisp		The computer language LISP.
net.lang.mod2		The computer language Modula-2.
net.lang.pascal		The computer language Pascal.
net.lang.prolog		The computer language PROLOG.
net.lang.st80		The computer language Smalltalk 80.
net.legal		Legalities and the ethics of law.
net.lsi			Large scale integrated circuits.
net.mag			Magazine summaries, tables of contents, etc.
net.mail		Proposed new mail/network standards.
net.mail.headers	The ARPA header-people list.
net.mail.msggroup	The ARPA MsgGroup list.
net.math		Mathematical discussions and puzzles.
net.math.stat		Statistics.
net.med			Medicine and its related products and regulations.
net.micro		Micro computers of all kinds.
net.micro.16k		National 16000 & 32000 processors.
net.micro.432		Intel 432 processors.
net.micro.6809		Motorola 6809 processors.
net.micro.68k		Motorola 68000 processors.
net.micro.apple		Apple computers.
net.micro.atari		Atari computers.
net.micro.cbm		Commodore computers.
net.micro.cpm		The CP/M operating system.
net.micro.hp		Hewlett/Packard computers.
net.micro.pc		IBM personal computers.
net.micro.ti		Texas Instruments processors.
net.micro.trs-80	TRS-80 computers.
net.micro.zx		Sinclair zx computers.
net.misc		Miscellaneous discussions that don't belong elsewhere.
net.motss		Issues pertaining to homosexuality.
net.movies		Reviews and discussions of movies.
net.movies.sw		Subgroup for the Star Wars saga(s).
net.music		Music lovers.
net.music.classical	Classical music lovers.
net.net-people		Announcements, etc. concerning people on the net.
net.news		Discussions of USENET itself.
net.news.adm		For news administrators.
net.news.b		B news software.
net.news.config		Computer down times and network interruptions.
net.news.group		Discussions and lists of newsgroups.
net.news.map		Connectivity maps.
net.news.newsite	New site announcements.
net.news.sa		For system administrators.
net.nlang		Natural languages, cultures, heritages, etc.
net.nlang.celts		The Celtic culture.
net.nlang.greek		The Greek culture.
net.notes		Notesfile software from the University of Illinois.
net.origins		Evolution versus creationism.
net.periphs		Peripheral devices.
net.pets		Pets, pet care, and household animals in general.
net.philosophy		Philosophical discussions.
net.physics		Physical laws, properties, etc.
net.poems		For the posting of poems.
net.politics		Political discussions.
net.puzzle		Puzzles, problems, and quizes.
net.railroad		Real and model trains.
net.rec			Recreational/participant sports.
net.rec.birds		Bird watching.
net.rec.boat		Boating.
net.rec.bridge		Bridge.
net.rec.coins		Coin collecting.
net.rec.disc		Disc activities (Frisbee, etc).
net.rec.nude		Naturalist/nudist activities.
net.rec.photo		Photography.
net.rec.scuba		SCUBA diving.
net.rec.ski		Skiing.
net.rec.skydive		Skydiving.
net.rec.wood		Woodworking.
net.religion		Religious, ethical, and moral implications of actions.
net.religion.jewish	Judaism.
net.research		Research and computer research.
net.roots		Genealogical matters.
net.rumor		Rumors.
net.sci			General purpose scientific discussions.
net.sf-lovers		Science fiction lovers.
net.singles		Single people, their activities, etc.
net.social		Social activities.
net.sources		Submittion of Software packages.
net.space		Space, space programs, space related research, etc.
net.sport		Spectator sports.
net.sport.baseball	Baseball.
net.sport.football	Football.
net.sport.hockey	Hockey.
net.sport.hoops		Basketball.
net.startrek		Star Trek, the TV show and the movies.
net.std			All sorts of standards.
net.suicide		Suicide, laws, ethics, and its causes and effects.
net.taxes		Tax laws and advice.
net.test		Testing of network software.
net.text		Text processing.
net.travel		Traveling all over the world.
net.tv			The boob tube, its history, and past and current shows.
net.tv.drwho		The TV show Dr. Who.
net.tv.soaps		Soap operas.
net.unix		UNIX neophytes group.
net.unix-wizards	Discussions, bug reports, and fixes on and for UNIX.
net.usenix		USENIX Association events and announcements.
net.usoft		Universal (public domain) software packages.
net.veg			Vegetarians.
net.video		Video and video components.
net.vvs			The Vortex Video System for digitized video images.
net.wanted		Requests for things that are needed.
net.wines		Wines and spirits.
net.wobegon		"The Prairie Home Companion" radio show.
net.women		Women's rights, discrimination, etc.
net.women.only		Postings by women only (read by all).
net.works		Workstations in general.
net.works.apollo	Apollo workstations.
fa.arms-d		Arms discussion digest.
fa.arpa-bboard		ARPANET bulletin board.
fa.bitgraph		The BBN bitgraph terminal.
fa.digest-p		Digest-people digest.
fa.editor-p		Editor-people digest.
fa.energy		Energy programs, conservation, etc.
fa.human-nets		Computer aided communications digest.
fa.info-mac		The Apple Macintosh computer.
fa.info-terms		All sorts of terminals.
fa.info-vax		DEC's VAX line of computers.
fa.info-vlsi		Very large scale integrated circuits.
fa.laser-lovers		Laser printers, hardware and software.
fa.poli-sci		Politics and/versus science.
fa.railroad		Real and model trains.
fa.sf-lovers		Science fiction lovers.
fa.tcp-ip		TCP and IP network protocols.
fa.telecom		Telecommunications digest.
fa.teletext		Teletext digest.
mod.ber			Summaries of discussions from other groups.
E_O_F
: if active file is empty, run makeactive
if test ! -s $LIBDIR/active
then
	sed 's/[ 	].*/ 00000 00001/' /tmp/$$groups > $LIBDIR/active
	cat <<"E_O_F" >>$LIBDIR/active
general 00000 00001
control 00000 00001
junk 00000 00001
E_O_F
else
: make sure it is in the new format
	set - `sed 1q $LIBDIR/active`
	case $# in
	3|4)	;;
	2)	ed - $LIBDIR/active << 'EOF'
1,$s/$/ 00001/
w
q
EOF
		echo
		echo Active file updated to new format.
		echo You must run expire immediately after this install
		echo is done to properly update the tables.;;
	*) echo Active file is in unrecognized format. Not upgraded.;;
	esac
fi
if test $# -eq 3 -o $3 -eq -2
then
	(sed '/^!net/!d
s/^!//
s!^!/!
s!$! /s/$/ n/!
' $LIBDIR/ngfile
	echo '/^fa./s/$/ n/'
	echo '/ n$/!s/$/ y/') >/tmp/$$sed
	mv $LIBDIR/active $LIBDIR/oactive
	sed -f /tmp/$$sed $LIBDIR/oactive >$LIBDIR/active
	chown $NEWSUSR $LIBDIR/active
	chgrp $NEWSGRP $LIBDIR/active
	chmod 644 $LIBDIR/active
fi
sort /tmp/$$groups | $LIBDIR/checkgroups
rm -f /tmp/$$*
