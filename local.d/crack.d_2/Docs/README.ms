.de C
.ie n .B "\\$1" \\$2
.el .CW "\\$1" \\$2
..
.TL
"Crack Version 3.3c"
.br
A Sensible Password Checker for
.UX
.AU
Alec D.E. Muffett
.AI
Computer Unit, University College of Wales
Aberystwyth, Wales, SY23 3DB
.I aem@aber.ac.uk
+44 970 622492
.AB
.B Crack \**
.FS
.C Crack
is available for anonymous FTP from
.I "wuarchive.wustl.edu"
(128.252.135.4) in
.I ~/packages
.FE
is a freely available program designed to find standard
.UX
eight-character DES encrypted passwords by standard guessing techniques
outlined below.  It is written to be flexible, configurable and fast,
and to be able to make use of several networked hosts via the Berkeley
.C rsh
program (or similar), where possible.
.AE
.NH 1
Intent of this program
.LP
My intentions, whilst writing this program, were
.IP 1)
To provide the average system manager with a simple, flexible and fast
tool to find passwords which would be easily compromised by a dictionary
search.
.IP 2)
To weaken the complacency which has developed amongst many (in)?experienced
.UX
systems managers about password security. Just because the
.C crypt()
function cannot be reversed does not mean that your passwords are
secure.  If this program helps to raise the overall quotient of paranoia
just one little bit, then it will have served its purpose and is all to
the good.
.LP
I am quite certain that some childish morons out there will abuse the
work that I have put into creating this program.  It's up to them how
they use it, but if it's used for some illegal purpose it's not my
intention, nor my fault. I hope they get caught.
.LP
Crack was not designed to do anything
.I nasty ,
other than to attack passwords in sensible order.  This is why I feel I
can post it to the net without fear of recrimination.  This version of
Crack is supplied with a (hopefully) portable version of
.C fcrypt() ,
for speed reasons.  Fcrypt() is not yet widely available, but several
versions have now been posted to USENET, and with the growing ability to
access anonymous FTP databases like
.B Archie ,
I believe that people who really want access to such programs as
fcrypt() will be able to get them easily.  People who need them but
don't have the time to look, on the other hand, will not get them.  This
is not a good thing if you are a systems manager, because it gives the
crackers an advantage.  My philosophy is:
.I "give it to everyone, then at least the people who need it can get it" .
.NH 1
Fcrypt() Statistics
.LP
The version of fcrypt() that comes with Crack is several times faster
than the version that was originally supplied to me, and should
outperform most others which are publicly available.  I haven't tried
many speed-demon style tricks, but rather I have gone for portability,
elegance and simplicity, where applicable
.C 8-).
On a DECStation 5000/200, fcrypt() iterates about 580 times per second,
and overall, I measure this to be about 15 times faster than standard
crypt().  There are faster versions available, but at least I feel free
to redistribute this version without stepping on anyones toes.
.LP
A final note about my motives: If you think that I am a moron for
writing and then distributing this program, and you think that the
program is in itself dangerous, I suggest that you carefully consider
the fact that any moron could have written this program.  Flames to
.C /dev/null ,
please.
.NH 1
Implementation
.LP
I have tried to make Crack as portable as possible without compromising
speed.  Needless to say, it is imperfect in the this respect, but I have
tried.  If you have problems getting Crack up, please let me know what
these problems are, and what system you were trying to put Crack up on.
I believe that Crack will work on
.B Xenix
and various versions of
.B "System V" ,
but it may require a little effort if your
.UX
is not fairly modern.
.LP
To use the
.C "Crack -network"
option, you must have
.C rsh ,
or a similar program.
.C rsh
is a BSD-ism which has become fairly common on non-BSD systems.  If you
don't have it or something similar, let me know what you do have which
might, with a little ingenuity, do the job, and I shall see what I can
do.  Have a look in
.C Scripts/RCrack
and feel free to play around.
.QP
.B Note:
.B Xenix
users and some others have a
.C rcmd
program instead of
.C rsh .
Similarly, 
.B HP-UX 
has a 
.C remsh
command.  I'm not sure of the correct syntax for these programs, but it
should not be hard to integrate them, if they do not already work. 
There is a note about these commands in
.C Scripts/RCrack
.QP
On
.B "System V"
based systems, users may also be missing the BSD
function
.C gethostname() .
If this is so, but you
.B do
have the
.C uname()
system call, define the macro
.C CRACK_UNAME
in
.C Sources/conf.h
instead. This ought to fix the problem, but it
.I may
need a little user intervention first (it depends where your header file for
.C uname()
is).
.I "Caveat Emptor!"
.NH 1
Method of Password Cracking
.LP
Crack does not take the serial approach to password guessing that other
programs like the
.B COPS
password cracker does.  Rather, Crack makes two passes over the users'
password entries.
.LP
The first pass bases its guesses on the contents of the
.I gecos
field (containing the users' real name), and the username itself.  The
first pass is fast and usually very fruitful, and completes quite
quickly.  You would be surprised at the number of spods who still think
that their middle name, backwards and capitalised, is a good password. 
.LP
The second pass is made by encrypting several pre-processed dictionaries
on the fly, and comparing the results with users passwords.  Crack
optimises the number of calls to the encryption function by sorting the
user passwords by their
.I "encryption salt"
before loading, so that the dictionaries only have to be encrypted once
for each different salt.  This generally saves about 30% of the calls
you would have to make to
.C crypt() .
.LP
The preprocessed dictionaries are created by merging the source
dictionaries found in the directory
.C DictSrc
and then truncating, sorting and uniq'ing the output from the
preprocessor.  The default dictionaries named are
.C /usr/dict/words
which provides the bulk of the input data, and
.C DictSrc/bad_pws.dat
which is meant to provide all of those non-dictionary passwords, such as
.I 12345678
or
.I qwerty .\**
.FS
Extra dictionaries (those used in Dan Klein's paper, below) can be
obtained via anonymous FTP from
.I uunet.uu.net
(192.48.96.2) as
.I ~/pub/dictionaries.tar.Z
.FE
.LP
If you wish to add a dictionary of your own, just copy it into the
.C DictSrc
directory and then delete the contents of the
.C Dicts
directory (run
.C Scripts/spotless
from within
.C $CRACK_HOME ).
Your new dictionary will be merged in on the next run.  Do
not worry about replication of data, as the preprocessor's driver script
sorts and uniq's the data before putting it into the
.C Dicts
directory.
.LP
The formats of the output dictionaries are:
.IP a)
Unique words that have been forced lowercase, forwards.  These are the
most common passwords you will find, thus they are tried first.
.IP b)
Dictionary words which have been artificially pluralised, because not
many dictionaries contain plurals.
.IP c)
Dictionary words forced capitalised (eg:
.I Capitalised ).
.IP d)
Dictionary words, forced lowercase, with a trailing 0 or 1 (this may be
extended by hacking Sources/crack-pp.c).
.IP e)
Dictionary words forced lowercase and backwards.
.IP f)
Dictionary words, forced lowercase, with a leading 0 or 1 (this may be
extended by hacking Sources/crack-pp.c).
.IP g)
Dictionary words, forced uppercase, forwards.
.IP h)
Dictionary words, forced uppercase, backwards.
.IP i)
Dictionary words forced capitalised, reversed (eg:
.I desilatipaC ).
.sp 1v
.LP
This choice of attack is entirely empirical, my own, and made up on the
spot.  It is also horrendously successful, and because Crack uses each
of these dictionaries in turn, it tends to get passwords faster than a
program like the
.B COPS
password cracker which tries words every-which-way for each user.\**
.FS
For more information, see "Foiling the Cracker: A Survey of, and
Improvements to, Password Security" by Daniel Klein, available from
.I "ftp.sei.cmu.edu"
in
.I "~/pub/dvk/passwd.*" .
Also, read the
.C APPENDIX
file supplied with this distribution.
.FE
.QP
Optimisation Note: Crack has an
.B compile-time
option, called
.C CRACK_SHORT_PASSWDS ,
which, if
.B not
defined, makes the dictionary preprocessor
.I "throw away"
words which are less than 5 characters long.  The reasoning for this is
that many sites, with a semi-sensible
.C passwd
program, will not have passwords shorter than 5 characters long.
.QP
It is up to you whether you bother testing these short passwords, but I
would recommend that you do so at least once, to be safe.  (Setting the
option also leads to having smaller pre-processed dictionaries.  The
option, however, is defined by default).
.NH 1
Installation
.LP
Crack is one of those most unusual of beasties, a self-installing
program.  Once the necessary configuration options have been set, the
executables are created via
.C make
by running the Crack shellscript .
.LP
Some people have complained about this apparent weirdness, but it has
grown up with Crack ever since the earliest network version, when I
could not be bothered to log into several different machines with
several different architectures, just in order to build the binaries.
.LP
Down to the nitty-gritty.  Crack needs to know where it has been
installed.  You will have to edit the
.C CRACK_HOME
variable in the Crack shellscript to the correct value.  This variable
should be set to an absolute path name (relative to
.I ~user
should be OK, so long as you have some sort of
.C csh )
through which the directory containing Crack may be accessed on
.B all
the machines that Crack will be run on. There is a similar variable
.C CRACK_OUT
which specifies where Crack should put its output files - by default,
this is the same as
.C "$CRACK_HOME" .
.LP
The next thing you will have to do is to edit the file
.C Sources/conf.h
and work out which switches to enable.
Each
.C #define
has a small note explaining its purpose.  Where I have been in doubt about
the portability of certain library functions, usually I have re-written
it, so you should be OK.  Let me know of your problems, if you have any.
.LP
One of the most important switches in the conf.h file is the
.C CRACK_NETWORK
symbol.  This forces Crack to create all of its output files with an
embedded hostname (obtained by the
.C gethostname()
routine) so that you can keep track of output from all over the network.
If you have no
.C gethostname()
but have a
.C uname()
system call, you can use that by defining
.C CRACK_UNAME
in
.C Sources/conf.h .
.LP
If you will be using
.C CRACK_NETWORK ,
you will then have to generate a
.C Scripts/network.conf
file. This contains a list of hostnames to
.C rsh
to, what their
.I "binary type"
is (useful when running a network Crack on several different
architectures), a guesstimate of their
.I "relative power"
(take your slowest machine as unary, and measure all others relative to
it), and a list of per-host
.I flags
to
.B add
to those specified on the
.C Crack
command line, when calling that host.  There is an example of such a
file provided in the Scripts directory - take a look at it.
.LP
If ever you wish to specify a more precise figure as to the relative
power of your machines, or you are simply at a loss, play with the
command
.C "make tests"
in the source code directory.  This will provide you with the number of
fcrypt()s that your machine can do per second, which is a number that
you can plug into your
.C network.conf
as a measure of your machines' power (after rounding the value to an
integer).
.NH 1
Crack Usage
.LP
.DS B
.fi
.C Crack
[\c
.I options ]
[\c
.I bindir ]
.C /etc/passwd
[...other passwd files]
.sp 1v
.C "Crack -network"
[\c
.I options ]
.C /etc/passwd
[...other passwd files]
.DE
.LP
Where
.B bindir
is the optional name of the directory where you want the binaries
installed.  This is useful where you want to be able to run versions of
Crack on several different architectures. If
.B bindir
does not exist, a warning will be issued, and the directory created.
.QP
Note:
.B bindir
defaults to the name
.C generic
if not supplied.
.QP
.B "Notes for Yellow Pages (NIS) Users:"
I have had some queries about how to get Crack running from a YP
password file.  There are several methods, but by far the simplest is to
generate a passwd format file by running:-
.DS B
.C "ypcat passwd > passwd.yp"
.DE
and then running Crack on this file.
.NH 1
Options
.IP "\fB-f\fP"
Runs Crack in
.I foreground
mode, ie: the password cracker is not backgrounded, and messages appear
on stdout and stderr as you would expect.  This option is only really
useful for small password files, or when you want to put a wrapper
script around Crack, or after you have been running in feedback mode.
.IP
Foreground mode is disabled if you try running
.C "Crack -network -f" ,
on the command line, because of the insensibility of
.C rsh ing
to several machines in turn, waiting for each one to finish before
calling the next. However, please read the section about
.I "Network Cracking without NFS/RFS" ,
below.
.IP "\fB-F\fP"
Puts Crack into
.I feedback
mode, ie: when a run of the password cracker terminates normally, the
software writes a list of the DES encrypted passwords which were (or
were not) cracked, into a 
.I "feedback file" , 
named
.C Runtime/fbk.<pid> ,
or similar.  When Crack is next invoked with the
.C "-F"
option, Crack removes from its input all encrypted passwords which can
be found in one of the feedback files, and reports on them immediately. 
This prevents Crack from wasting time trying to crack passwords that it
has cracked (or not cracked) before. 
.IP
Obviously, use of the
.C "-F"
option is
.B not
recommended when amendments have been made to the source dictionaries,
because new words may have been added which would guess someones
password.  After such amendments have been made, it is recommended that
all the preprocessed dictionaries are removed and the feedback files
tidied by running
.C "Scripts/spotless" ,
and then a full run should be done.
.IP
Use of feedback mode is 
.B dependent 
on defining the symbol
.C "CRACK_PRINTOUT"
in
.C "Sources/conf.h" .
.IP
Please note that for feedback mode to work properly during network
cracks, the
.C "-F"
option must be specified on the
.C Crack
.B "command line"
and not in the crack-flags field of the 
.C Scripts/network.conf
file. If you do not specify
.C "-F"
on the command line, the program will fail to filter unguessable users
from the input password files, and will therefore waste much time. 
.IP "\fB-v\fP"
Sets verbose mode, whereby Crack will print every guess it is trying on
a per-user basis.  This is a very quick way of flooding your filestore.
If you undefine the
.C CRACK_VERBOSE
symbol in
.C Sources/conf.h ,
verbose mode will be permanently disabled.
.IP "\fB-nvalue\fP"
Sets the process to be
.C nice() ed
to
.I value ,
so that the switch
.C \&-n19
sets the Crack process to run at the lowest priority.
.IP "\fB-network\fP"
Throws Crack into network mode, in which it reads the
.C Scripts/network.conf
file, splits its input into chunks which are sized according to the
power of the target machine, and calls
.C rsh
to run Crack on that machine.  Options for Crack running on the target
machine may be supplied on the command line (eg: verbose or recover
mode), or in the network.conf file if they pertain to specific hosts
(eg:
.C nice()
values).
.IP "\fB-r<pointfile>\fP"
This is only for use when running in
.I recover
mode.  When a running Crack starts pass 2, it periodically saves its
state in a file in the directory
.C Runtime ,
named
.C "pnt.<pid>"
or
.C "pnt.<hostname>.<pid>"
depending on your naming convention (see 
.I "Installation" , 
above).  This file can be used to recover where you were should a host
crash.  Simply invoke Crack in
.B exactly
the same manner as the last time, with the addition of the
.C -rRuntime/pnt.file.name
switch.  Crack will startup and read the file, and jump to slightly
before where it left off.  If you are cracking a very large password
file, this can save oodles of time after a crash.
.IP
If you are running a
.I network
Crack, then the jobs will again be spawned onto all the machines of the
original Crack.  The program will then check that the host it is running
on is the same as is mentioned in the pointfile.  If it is not, it will
silently die.  Thus, assuming that you supply the same input data and do
not change your
.C network.conf
file, Crack should pick up
.B exactly
where it left off.  This is a bit inelegant, but it's better than
nothing at the moment.
.NH
Support Scripts
.LP
The
.C Scripts
directory contains a small number of support and utility scripts, some
of which are designed to help Crack users check their progress.
Briefly, the most useful ones are:-
.IP "\fBScripts/shadmrg\fP"
This is a small (but hopefully readable) script for merging
.C /etc/passwd
and
.C /etc/shadow
on System V style shadow password systems.  It produces the merged data
to stdout, and will need redirecting into a file before Crack can work
on it.  The script is meant to be fairly lucid, on the grounds that I
worry that there are many shadowing schemes out there, and perhaps not
all have the same data format.
.IP "\fBScripts/plaster\fP"
which is named after a dumb joke, but is a simple frontend to the
.C "Runtime/die.*"
script each copy of the password cracker generates. Invoking
.C Scripts/plaster
will kill off all copies of the password cracker you are running, over
the network or otherwise.
.IP "\fBScripts/status\fP"
This script
.C rsh es
to each machine mentioned in the
.C Scripts/network.conf
file, and provides some information about processes and uptime on that
machine.  This is useful when you want to find out just how well your
password crackers are getting on during a
.C "Crack -network" .
.IP "\fBScripts/{clean,spotless}\fP"
These are really just frontends to a makefile. Invoking
.C Scripts/clean
tidies up the Crack home directory, and removes probably unwanted files,
but leaves the pre-processed dictionaries intact.
.C Scripts/spotless
does the same as
.C Scripts/clean
but obliterates the preprocessed dictionaries and output files too, and
compresses the feedback files into one. 
.IP "\fBScripts/guess2fbk\fP"
Only useful if CRACK_PRINT is defined. This script takes your
.C out.*
files as arguments and reformats the 'Guessed' lines into a slightly
messy
.I feedback 
file, suitable for storing with the others.
.IP 
An occasion where this might be useful is when your cracker has guessed
many peoples passwords, and then died for some reason before writing out
the guesses to a feedback file. Running
.sp 1v
.C "Scripts/guess2fbk out.* > Runtime/fbk.new"
.sp 1v
will save the work that has been done.
.NH 1
Multiprocessing and parallelism
.LP
The method of error recovery outlined above causes headaches for users
who want to do multiprocessing on parallel architectures.  Crack is in
no way parallel, and because of the way it's structured, readind stdin
from shellscript frontends, it is a pain to divide the work amongst
several processes via
.C fork() ing.
.LP
The hack solution to get several copies of Crack running on one machine
with
.I n
processors at the moment is to run with the
.C CRACK_NETWORK
option enabled, and insert
.I n
copies of the entry for your parallel machine into the
.C Scripts/network.conf
file. If you use the
.C \&-r
option in these circumstances however, you will get
.I n
copies of the recovered process running, only one of them will have the
correct input data.  I'm working on this.  My current solution is to
save the current username in the point file, and test it on
startup, but doing this
.I may
break your recovery if you supply different input data (so that the data
is sorted even slightly differently).  Hohum.  If you want to use this
.I "verify username"
facility, use
.C \&-R
in place of
.C \&-r .
.LP
As for not using the
.C network.conf
file to provide multiprocessing, I'm
.I still
working on it.
.NH 1
Network Cracking without NFS/RFS
.LP
For those users who have some form of 
.C rsh
command, but do not have a a networked filestore running between hosts,
there is now a solution which will allow you to do networked cracking,
forwarded to me by Brian Tompsett at Hull.  Personally, I consider the
idea to be potty, but it fills in missing functionality in a wonderfully
tacky manner. 
.LP
From the documentation above, you will note that Crack will undo the 
.C "-f"
.I "(output in foreground)"
option, if it is invoked with the 
.C "-network" 
switch at the same time (see the 
.I Options
section above).  This is true, but it does not apply if you specify
.C "-f"
option in the 
.C network.conf
file.  
.LP
The practical upshot of doing this is that remote copies of Crack
can be made to read from
.I stdin
and write to 
.I stdout
over a network link, and thus remote processing is accomplished.  I have
tweaked Crack in such a way, therefore, that if the 
.C "-f"
option is specified amongst the crack-flags of a host in the
network.conf, rather than backgrounding itself on the remote host, the 
.C rsh
command on the 
.B server
is backgrounded, and output is written directly to the files on the
server's filestore.  
.LP
There are restrictions upon this method, mostly involving the number of
processes that a user may run on the server at any one time, and the
fact that if you run in
.I feedback
mode, you will have to collect the feedback output together manually
(dropping it into the
.C Runtime
directory on the server).  However, it works. Also, if you try to use 
.C rsh
as another user, you will suffer problems if
.C rsh 
insists on reading something from your terminal (eg: a password for the
remote account).  Also, checkpointing goes out the window unless you
specify the name of the pointfile on the remote machine. 
.NH 1
Notes on fast crypt() implementations
.LP
The stdlib version of the
.C crypt()
subroutine is incredibly slow.  It is a
.I massive
bottleneck to the execution of Crack and on typical platforms that you
get at universities, it is rare to find a machine which will achieve
more than 50 standard
.C crypt() s
per second.  On low-end diskless workstations, you may expect 2 or 3 per
second.  It was this slowness of the
.C crypt()
algorithm which originally supplied much of the security
.UX
needed.\**
.FS
See: "Password Security, A Case History" by Bob Morris & Ken Thomson, in
the
.UX
Programmer Docs.
.FE
.LP
However, there are now
.C many
implementations of faster versions of
.C crypt()
to be found on the network.  The one supplied with Crack v3.2 and
upwards is called
.C fcrypt() .
.LP
.C fcrypt()
was originally written in May 1986 by Robert Baldwin at MIT, and is a
good version of the
.C crypt()
subroutine.  I received a copy from Icarus Sparry at Bath University,
who had made a couple of portability enhancements to the code.
.LP
I rewrote most of the tables and the KeySchedule generating algorithm in
the original
.I fdes-init.c
to knock 40% off the execution overhead of
.C fcrypt()
in the form that it was shipped to me.  I inlined a bunch of stuff, put
it into a single file, got some advice from Matt Bishop and Bob Baldwin
[both of whom I am greatly indebted to] about what to do to the
.C xform()
routine and to the fcrypt function itself, and tidied up some algorithms.
I have also added more lookup tables and reduced several formula for
faster use.
.C fcrypt()
is now barely recognisable as being based on its former incarnation.
.LP
On a DecStation 5000/200, it is also about 15 times faster than the
standard crypt (your mileage may vary with other architectures and
compilers).  This speed puts
.C fcrypt()
into the "moderately fast" league of crypt implementations.  By using
.C fcrypt()
with Crack, I extracted 135 passwords from my standard 1087 user
password file in a little over 1 hour using 3 networked machines.  This
is from a moderately good password file.
.LP
Why am I saying this sort of thing ? Am I scaremongering ? In a word, yes.
.LP
If a fast version of
.C crypt()
is wired into a program like Crack it can break a poorly passworded site
open in minutes.  There are such programs available, eg: the "Killer
Cracker" written by the anonymous "Doctor Dissector", with anonymous
motives.  It comes with a modified version of Baldwin's fcrypt, as a
MS-DOS executable with a GNU copyleft licence.
.LP
The point that needs to be hammered home is that unless something is
done, and done soon, about the general quality of passwords on
.UX
systems, then in the near future our doors will be wide open to people
who have programs like Crack and questionable motives.
.NH 1
Solutions and Conclusions
.LP
What can be done about this form of attack ?
.LP
You must get a drop-in replacement for the
.C passwd
and
.C yppasswd
commands; one which will stop people from choosing bad passwords in the
first place.  There are several programs to do this; Matt Bishop's
.C "passwd+"
and Clyde Hoover's
.C "npasswd"
program are good examples which are freely available.  Consult an
.B Archie
database for more details on where you can get them from.
.LP
A little common-sense is all that is required to vet passwords: I
enclose a module in the Sources directory
.I goodpass.c
which I use in a modified version of the
.C yppasswd
in order to provide some security.  It is quite heavily customised for
use in the UK, but it should be easily portable.  The routine is invoked:
.sp 1v
.DS B
.C "char *retval = GoodPass(char *input);"
.DE
.LP
where
.C input
is the password under test, and
.C retval
will be set either to NULL (if the password is OK) or to a diagnostic
string which says what is wrong with the password.  It is far less
complex than a system such as
.I passwd+ ,
but still effective enough to make a password file (probably) withstand
.C Crack .
It would be nice if an organisation (such as
.B CERT ?)
could be persuaded to supply skeletons of
.I sensible
passwd commands for the public good, as well as an archive of security
related utilities\**
on top of the excellent
.C COPS .
.FS
.C COPS
is available for anonymous FTP from
.I "cert.sei.cmu.edu"
(128.237.253.5) in
.I ~/cops
.FE
However, for
.UX
security to improve on a global scale, we will also require pressure on
the vendors, so that programs are written correctly from the beginning.
