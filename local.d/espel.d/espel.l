.TH ESPEL 1 "ENHANCEMENT TO SPELL"
.SH NAME
espel \- enhanced spell
.SH SYNOPSIS
.B espel
[ option ] ...
[ file ] ...
.SH DESCRIPTION
.I Espel
works like spell with two differences.  It
uses your local dictionary and operates interactively.
.PP
The local dictionary consists of two files, .spellht and .spellwd,
kept in your home directory.  '.spellht' is the hashtable
and '.spellwd' is the list of additions. 
There are two ways to add words to your local dictionary.
The first is to run espel,
where the 'a' command allows you to add to the local dictionary.
The second is to edit the list of additions,
remove your hashtable and run espel.  When espel
finds you have no hashtable it will use the list (if it exists ) and
the spell hashtable ( /usr/dict/hlista ) to make your new hashtable.
.PP
Espel defaults to the interactive mode.
To run with out it use the -n option.
Interactively the user is given the opportunity to add words
to the local dictionary, look up correct spelling, correct misspelled
words in the documents and other interesting things.
.PP
Command line options:
.TP
.B \-n
Not interactive.
Simply prints the list of misspelled words on standard out.
.TP
.B \-s
Silent option. Spell normally puts all your misspelled words
into a big list so everybody can laugh at them.  This option
allows you to appear as though you never misspell any words.
.SH COMMANDS
These commands are used in the interactive mode ( default ).
Commands consist of a single letter
and some are followed by an argument.
There are two kinds of commands: looping and nonlooping.
Looping commands keep the current word,
while nonlooping commands advance to the next.
.PP
Looping commands:
.TP
.B h
Help.  Prints out a brief summary of the interactive commands.
.TP
.B s pattern
Look-up.  Spells out all words beginning with pattern found in 
the dictionary.
.TP
.B c (word)
Context of word is printed out. Assumes current word if none other given
Each line containing the
the current word is shown on the screen. (Useful for situations where the
word is so badly misspelled you do not even know what you were
trying to spell.)
.TP
.B l (number)
List number of remaining misspelled words. If no number given all the
words are printed.
.TP
.B ! command
Shell escape. The command is executed and then espel returns.
.PP
Non-looping commands:
.TP
.B a (word)
Adds current word to local dictionary if no argument is 
given, adds 'word' otherwise.
.TP
.B r word
Replaces all occurrences of the current word
with 'word' in all the files given on the command line.
.TP
.B rtn
Carriage return.  Skips the current word.
.TP
.B - (number)
Backup. Backs up the list by the number specified. (defaults to one
if no number specified)
.TP
.B q
Quits the interactive session executing any additions or changes
you may have specified.
.TP
.B x
Exits without making any changes.
.PP
.SH FILES
/usr/dict: directory of word lists and hashtables
.br
/usr/lib/spell: binary for espel filter
.br
\.spellht: local hashtable
.br
\.spellwd: local word list
.br
deroff(1), sort(1), tee(1), sed(1), spell(1)
.SH AUTHOR
Eric Grimm
