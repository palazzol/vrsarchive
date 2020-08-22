ANSI Draft of Proposed  C Language Std.

Mail your replies to the author(s) below or to cbosgd!std-c.
Cbosgd is reachable via most of the USENET nodes, including ihnp4,
ucbvax, decvax, hou3c....  Administrivia should be mailed to 
cbosgd!std-c-request.

ARPA -> mail to cbosgd!std-c@BERKELEY.ARPA

**************** mod.std.c  Vol. 1 No. 4  12/15/84 ********************

Today's Topics:

	Comments on Sections A and B of 11/12/84 Draft (X3J11/84-161)
	Comments on Section C of 11/12/84 Draft (X3J11/84-161)
	Comments on Section D of 11/12/84 Draft (X3J11/84-161)

----------------------------------------------------------------------

Date: Tue, 11 Dec 84 20:51:22 EST
From: ihnp4!seismo!elsie!ado
Subject: Comments on Sections A and B

*	In section A.7 (page 3) appears:

		Strictly conforming programs are intended to be maximally
		portable.

	Given that the draft standard extends the C language (with a unary plus
	operator, a "case" statement allowing for a range of cases, and in other
	ways) it's possible to write strictly conforming programs that cannot
	be compiled by any existing C compiler that I know of.  I be reluctant
	to call such programs "maximally portable."  I suggest deleting this
	sentence from the draft standard.

*	In section B.3.2 (page 9) appears:

		\a  (alert) Produces an implementation-defined audible or
		    visible representation of an alert character.  Any
		    consequent alteration to the printing position is
		    implementation-defined.

	First, I favor changing the second sentence to read "The printing
	position remains as is."  Doing this would eliminate the need for
	programs using the alert character to do repositioning after writing
	the character; given the usual mapping of the alert character to a bell,
	this also accurately describes what happens.

	Second, I favor using

		\!

	or some other escape sequence involving a non alphanumeric character to
	represent the alert character.  This helps matters if a program is
	compiled on an "old" system--for example, on our 4.1bsd system, the
	program
		main(){printf("\aHello\n");}
	produces the message
		aHello
	while the program
		main(){printf("\!Hello\n");}
	produces the message
		!Hello
	...and I suggest that it's easier to diagnose what's going on in the
	second case than it is in the first.
	(If '\a' was selected because it is already used to represent the alert
	character in some systems, by all means stick with it.)

	And if the standard is going to add in new escape sequences, perhaps
	the committee will want to consider resurrecting the old (if I remember
	alright) '\s' sequence that stood for a space character--perhaps
	transmogrifying it to '\_' in the process.

*	In section B.3.4.1 (page 10) on the <limits.h> file appears:

		When these constraints may be expressed numerically, they
		are available via the standard header <limits.h>.

	I suggest changing this to

		Some of these constraints are available via the standard
		header <limits.h>.

	to avoid controversy over what may and may not be expressed numerically.

*	In section B.3.4.1 (page 10) appears:

		* maximum exponent power of ten that floating point can
		  represent	38

	On our 4.1bsd system, the program:
		main() { printf("%f\n", 10.0e37);printf("%f\n", 10.0e38); }
	produces this output:
		100000000000000000000000000000000000000.000000
		170141183460469230000000000000000000000.000000
	which means (to me) that 37 is the maximum "power of ten" that floating
	point can represent.  Either I or the description can stand being made
	clearer.

	I also wonder if 38 is the right value to have for a "strictly
	conforming" implementation.  Perhaps a lower value would allow more
	implementations to be conforming while at the same time not breaking
	any existing software.

*	In section B.3.4.1 (page 10) appears:

		* maximum number of bits for smallest datum (char or byte) [ 8 ]

	Later, in section C.1.2.5 (page 16) appears:

		An object declared as a character (char) is large enough to
		store any member of the execution character set that can be
		specified as a single-character character constant without
		using an octal or hexadecimal escape sequence.

	Since this second requirement might be satisfied (given the number of
	members of the execution character set) by as few as seven bits, I
	suggest changing the 8 to a seven in section B.3.4.1.

*	In section B.3.4.2 (page 11) appears:

		* External identifiers beginning with a leading underscore, and
		  all identifiers beginning with two underscores, are reserved
		  for use by the implementation and must not be used by a
		  program, except to specify implementation-defined values.

	This seems rather one-sided.  I suggest adding:

		* External identifiers that do not begin with an underscore are
		  reserved for use by programs and must not be used by the
		  implementation, except to specify implementation-defined
		  values.

	or some variant.

----------------------------------------------------------------------

Date: Tue, 11 Dec 84 20:51:22 EST
From: ihnp4!seismo!elsie!ado
Subject: Comments on Section C

*	In section C.1.2.1 (page 15) appears:

		If the declaration appears within the list of parameters in
		a function prototype, the identifier has function prototype
		scope, which extends from the completion of the declaration of
		the parameter to the end of the declaration of the function.

	I suggest replacing "to the end of the declaration of the function"
	with "to the end of the function prototype."

*	In section C.1.7 (page 22) appears:

		The contents of a comment are examined only to find the
		characters */ that terminate it.  Thus comments do not nest.

	Later, in section E.3 (page 130) appears:

		An implementation may generate warnings. . .The following are
		a few of the more common situations.

		* The characters /* are found in a comment (SC.1.7).

	The first statement that comments are examined ONLY to find */ seems
	at odds with the second that a warning may be issued if /* is found in
	a comment.  I suggest changing the first statement to read:

		Comments do not nest.

*	In section C.3.1.2 (page 28) appears:

		A function call is a primary expression followed by
		parentheses. . .The primary expression must have type "function
		returning type" or "pointer to function returning type". . .

	I guess that this statement is designed to avoid breaking programs
	like:
		subr()
		{
		}
		main()
		{
			int (*	ptr)();
			subr();
			(*subr)();
			ptr();
			(*ptr)();
		}
	where, because of laxness on the part of some existing compilers,
	source code that in some sense "isn't right" nonetheless compiles.
	However, I note that a program like:
		subr()
		{
		}
		main()
		{
			int (*	ptr)();
			subr();
			(*subr)();
			(**subr)();
			ptr();
			(*ptr)();
			(**ptr)();
		}
	also compiles.

	If the goal is to avoid breaking existing programs regardless of how
	silly they may be, I suggest changing the cited passage to:
		A function call is a primary expression followed by
		parentheses. . .The primary expression must have type "function
		returning type" or "pointer to function returning type" or
		"pointer to pointer to function returning type" (and so on). . .

	If the goal is to have a reasonable standard, I suggest changing the
	cited passage to:

		A function all is a primary expression followed by
		parentheses. . .The primary expression must have type "function
		returning type". . .

*	In Section C.3.1.3 (page 29) appears:

		One special guarantee is made. . .If a union contains several
		structures that share a common initial sequence, and if the
		union object currently contains one of these structures, it is
		permitted to inspect the common initial part of any of them.

	    Examples. . .

		The following is a valid fragment:

			union {
				struct {
					int	type;
				};
				struct {
					int	type;
					int	intnode;
				} ni;
				struct {
					int	type;
					float	floatnode;
				} nf;
			} u;
			...
			u.nf.type = 1;
			u.nf.floatnode = 3.14;
			...
			if (u.n.type == 1)
				... sin(u.nf.floatnode) ...

	Valid, yes. . .and granted, it is used for the purposes of an example.
	Still, why would the above code be used rather than:

			struct {
				int	type;
				union {
					int	intnode;
					float	floatnode;
				} u;
			} s;
			...
			s.type = 1;
			s.u.floatnode = 3.14;
			...
			if (s.type == 1)
				... sin(s.u.floatnode) ...

	Since the example in question is designed to legitimize the cumbersome
	"special guarantee" that precedes it, I like to see a more legitimate
	example.

*	Section C.3.3.3 describes a "unary + operator."  I suggest dropping
	this from the standard. . .it adds no functionality, and use of it by
	programs will ensure that they won't be portable to existing systems.

*	Section C.5.2 describes "type specifiers," with the type specifier
	"const" among them.  I recall the (perhaps apocryphal) story of Dennis
	Ritchie being asked what Dennis would do differently if "reinventing"
	UNIX (AT&T Bell Laboratories trademark).  Dennis's alleged response was
	that the only change would be to use "create" instead of "creat" as the
	name of the relevant system call.

	With that in mind, I suggest changing the keyword to "constant."
	(Either that or change "volatile" to "volat." :-))

*	In section C.5.3.3 (page 47) (on function declarators) appears:

		Any identifier declared in the list has function prototype
		scope, which extends to the end of the declaration of the
		function.

	I suggest replacing "to the end of the declaration of the function"
	with "to the end of the function prototype."

*	In Section C.7.1 (page 57) appears:

		Any formal parameter that is not declared or whose type is not
		given is taken to have type signed int.

	Oh the lint grief that this might cause!  I suggest replacing
	"signed int" with "int."

*	In Section C.8 (page 59) (on preprocessing directives) appears:
	
		. . .directives. . ."begin with" a # separator character;
		more precisely, by a # token that is the first character in the
		source file (after any number of spaces and horizontal-tab
		characters) or that follow a new-line character (after any
		number of space and horizontal-tab characters).

	The exclusion of form-feed characters and vertical-tab characters from
	the set of characters that may appear before the '#' makes skipping of
	"white space" in preprocessors more difficult.  I suggest replacing
	"any number of spaces and horizontal-tab characters" with "any amount
	of white space" in the above passage.

*	In Section C.8.3 (page 81) appears:

		# if	constant-expression	new-line

	I'm unclear on whether floating point constant expressions are allowed
	here.  I like the standard to be clear on that.

----------------------------------------------------------------------

Date: Tue, 11 Dec 84 20:51:22 EST
From: ihnp4!seismo!elsie!ado
Subject: Comments on Section D

*	Section D.9.5.3 (page 89) describes the "mode" arguments to "fopen,"
	among them:

		"rb+"	open binary file for update (reading and writing)
		"wb+"	create binary file for update or truncate
		"ab+"	append; open binary file or create for update,
			writing at end-of-file.

	I suggest changing these to "r+b", "w+b", and "a+b", respectively.
	Doing this allows programs using such modes to be ported without
	change to existing Berkeley systems.

*	Sections D.9.10.2 and D.9.10.3 describe the feof and ferror
	functions, which are specified with these prototypes on page 101:

		int feof(FILE *stream);
		int ferror(FILE *stream);

	It may be possible to change these prototypes to:

		int feof(const FILE *stream);
		int ferror(const FILE *stream);

	It may even be desirable.
--------------------------------------
End of Vol. 1, No. 4. Std-C  (Dec. 15, 1984  14:00:00)
-- 
Orlando Sotomayor-Diaz/AT&T Bell Laboratories/201-870-7249
		      /Crawfords Crnr. Rd., Holmdel WB 3D109, NJ, 07733
UUCP: {ihnp4, houxm, akgua, mhuxd, ...}!homxa!osd7  
