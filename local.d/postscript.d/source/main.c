/*
 * Copyright (C) Rutherford Appleton Laboratory 1987
 * 
 * This source may be copied, distributed, altered or used, but not sold for profit
 * or incorporated into a product except under licence from the author.
 * It is not in the public domain.
 * This notice should remain in the source unaltered, and any changes to the source
 * made by persons other than the author should be marked as such.
 * 
 *	Crispin Goswell @ Rutherford Appleton Laboratory caag@uk.ac.rl.vd
 */
#include <signal.h>
#include "main.h"

#define ERRORDICTSIZE	 25

Object PDictFull;
Object PDictOverflow,		PInvFont,		PSyntaxError;
Object PDictUnderflow,		PInvRestore,		PTypeCheck;
Object PExecOverflow,		PIOError,		PUndefined;
Object PExecUnderflow,		PLimitCheck,		PUnFilename;
Object PInterrupt,		PNoCurrentPoint,	PUnResult;
Object PInvAccess,		PRangeCheck,		PUnMatched;
Object PInvExit,		POpOverflow,		PUnregistered;
Object PInvFileAccess,		POpUnderflow,		PVMError;

Object Lbracket, Rbracket, Nil, Marker;
Object OpInterp, ErrorDict;

static int ErrorFn ();
static Object InsErr ();
static void InitErrors (), InitArguments (), Interpreter ();
static Object OpError, error_name, execName;

static catchint ();
static int EnableInterrupts ();

char *library;

static int files_present = 0, interrupted = FALSE, protected = 0;
int interactive = FALSE, verbose = FALSE;
FILE *vfp = NULL;

main (argc, argv) int argc; char **argv;
 {
 	char profile[BUFSIZE], rcfile [BUFSIZE], *getenv ();
 	FILE *fp;
 	int here;
 	
 	library = getenv ("POSTSCRIPTLIB");
	if (library == NULL)
		library = default_library;
	
	VOID sprintf (rcfile, "%s/psrc", library);
 	
 	if (close (dup (9)) == 0)
 	 {
 		verbose = TRUE;
 		vfp = fdopen (9, "w");
 	 }
 	Message ("POSTSCRIPT");
 	Init ();
 	Message ("Init Errors");
 	InitErrors ();
	Message ("Init Arguments");
	
	VOID Push (ExecStack, Cvx (NameFrom ("executive")));
	
	here = Height (ExecStack);
	
	Install ("Library", StringFrom (library));
	
	InitArguments (argc, argv);
	interactive = interactive || isatty (0) && !files_present;
	ReverseStack (ExecStack, Height (ExecStack) - here);
	
 	VOID strcpy (profile, getenv ("HOME") ? getenv ("HOME") : "/etc");
 	VOID strcat (profile, "/.postscript");
 	
 	if (fp = fopen (profile, "r"))
		VOID Push (ExecStack, Cvx (FileFrom (fp)));
		
	if (!interactive)
		VOID Push (ExecStack,
			Cvx (StringFrom (files_present ?
					"/executive null def" :
					"/executive (%stdin) (r) file cvx def")));
	
	if ((fp = fopen (rcfile, "r")) == NULL)
		fprintf (stderr, "%s: cannot open %s\n", argv[0], rcfile),
		exit (1);
	else
		VOID Push (ExecStack, Cvx (FileFrom (fp)));
	InstallOp ("enableinterrupts",	EnableInterrupts, 0, 0, 0, 0);
/*	SysDict = ReadOnly (SysDict);	*/
	Message ("Entering Interpreter");
	Interpreter ();
 }

Message (s) char *s;
 {
	if (vfp != NULL)
		fprintf (vfp, "%s\n", s), fflush (vfp);
 }

Protect ()
 {
 	++protected;
 }

Unprotect ()
 {
 	--protected;
 }

static int EnableInterrupts ()
 {
	if (interactive)
		VOID signal (SIGINT, catchint);
	return TRUE;
 }

static void InitArguments (argc, argv) int argc; char **argv;
 {
  	int i, command = FALSE;
	
	for (i = 1; i < argc; i++)
	 {
	 	FILE *fp;
	 	
	 	if (command)
	 	 {
	 	 	command = FALSE;
	 	 	VOID Push (ExecStack, Cvx (StringFrom (argv[i])));
	 	 }
	 	else if (*argv[i] == '-')
	 		switch (argv[i][1])
	 		 {
	 		 	case 'i':
	 		 		interactive = TRUE;
	 		 		break;
	 		 	
	 		 	case 'c':
	 		 		++files_present; 
	 		 		command = TRUE;
	 		 		break;
	 		 	
	 		 	case 's': case '\0':
	 		 		++files_present;
			 		VOID Push (ExecStack, Cvx (Fstdin));
	 		 		break;
	 		 	
	 		 	default:
	 		 		fprintf (stderr, "%s: unknown option '%c'\n", argv[0], argv[i][1]);
	 		 		exit (1);
 	 		 }
 	 	else
 	 	 {
 	 	 	++files_present;
 	 	 	if (fp = Fopen (argv[i], "r"))
 				VOID Push (ExecStack, Cvx (FileFrom (fp)));
 			else
 				fprintf (stderr, "%s: cannot open %s\n", argv[0], argv[i]);
 		 }
 	 }
 }

static void Interpreter ()
 {
	Self = execName = NameFrom ("exec");
	while (Height (ExecStack) != 0)
	 {
 		int res;
 		Object item, exop;
 		
 		item = Pop (ExecStack);
  		if (!xCheck (item))
 			res = Push (OpStack, item);
  		else if (TypeOf (item) == Operator)
  			res = ExecOperator (item);
  		else if (TypeOf (item) == Array)
  			res = ExecArray (item);
  		else if (TypeOf (item) == File)
  			res = ExecFile (item);
  		else if (TypeOf (item) == Name)	/* names get special treatment for speed */
  		 {
  		 	Object newitem;
  		 	
  		 	newitem = Load (item);
  		 	if (TypeOf (newitem) != Condition)
  		 	 	item = newitem;
  		 	
  		 	if (!xCheck (item))
 				res = Push (OpStack, item);
  			else if (TypeOf (item) == Operator)
  				res = ExecOperator (item);
  			else if (TypeOf (item) == Array)
  				res = ExecArray (item);
  			else if (TypeOf (item) == File)
  				res = ExecFile (item);
			else
  			 {
 		 		res = Push (OpStack, item);
  		 		exop = Lookup (TypeOf (item), execName);
  		 		if (TypeOf (exop) != Condition)
  		 			VOID Push (ExecStack, exop);
  		 	 }
   		 }
  		else
  		 {
  		 	res = Push (OpStack, item);
  		 	exop = Lookup (TypeOf (item), execName);
  		 	if (TypeOf (exop) != Condition)
  		 		VOID Push (ExecStack, exop);
  		 }
  		
	 	if (interrupted && !protected)
	 	 {
 			interrupted = FALSE;
 			error_name = PInterrupt;
 			res = FALSE;
 		 }
 		
  		if (!res)
  		 {
  		 	Object error_op;
  		 	
  		 	VOID Push (OpStack, Self);
			error_op = DictLoad (ErrorDict, error_name);
			if (TypeOf (error_op) == Condition)
				VOID Push (ExecStack, OpError);
			else
				VOID Push (ExecStack, error_op);
	 		Cbreak (TRUE);
	 	 }
	 }
 }

static void InitErrors ()
 {
	ErrorDict = MakeDict (ERRORDICTSIZE);
		PDictFull	= InsErr (ErrorDict, "dictfull",			ErrorFn);
		PDictOverflow	= InsErr (ErrorDict, "dictstackoverflow",	ErrorFn);
		PDictUnderflow	= InsErr (ErrorDict, "dictstackunderflow",	ErrorFn);
		PExecOverflow	= InsErr (ErrorDict, "execstackoverflow",	ErrorFn);
		PExecUnderflow	= InsErr (ErrorDict, "execstackunderflow",	ErrorFn);
		PInterrupt	= InsErr (ErrorDict, "interrupt",		ErrorFn);
		PInvAccess	= InsErr (ErrorDict, "invalidaccess",		ErrorFn);
		PInvExit	= InsErr (ErrorDict, "invalidexit",		ErrorFn);
		PInvFileAccess	= InsErr (ErrorDict, "invalidfileaccess",	ErrorFn);
		PInvFont	= InsErr (ErrorDict, "invalidfont",		ErrorFn);
		PInvRestore	= InsErr (ErrorDict, "invalidrestore",		ErrorFn);
		PIOError	= InsErr (ErrorDict, "ioerror",			ErrorFn);
		PLimitCheck	= InsErr (ErrorDict, "limitcheck",		ErrorFn);
		PNoCurrentPoint	= InsErr (ErrorDict, "nocurrentpoint",		ErrorFn);
		PRangeCheck	= InsErr (ErrorDict, "rangecheck",		ErrorFn);
		POpOverflow	= InsErr (ErrorDict, "stackoverflow",		ErrorFn);
		POpUnderflow	= InsErr (ErrorDict, "stackunderflow",		ErrorFn);
		PSyntaxError	= InsErr (ErrorDict, "syntaxerror",		ErrorFn);
		PTypeCheck	= InsErr (ErrorDict, "typecheck",		ErrorFn);
		PUndefined	= InsErr (ErrorDict, "undefined",		ErrorFn);
		PUnFilename	= InsErr (ErrorDict, "undefinedfilename",	ErrorFn);
		PUnResult	= InsErr (ErrorDict, "undefinedresult",		ErrorFn);
		PUnMatched	= InsErr (ErrorDict, "unmatchedmark",		ErrorFn);
		PUnregistered	= InsErr (ErrorDict, "unregistered",		ErrorFn);
		PVMError	= InsErr (ErrorDict, "VMerror",			ErrorFn);
	Install ("errordict", ErrorDict);
	OpError = MakeOp ("(errorfn)", ErrorFn, 1, 0, 0, 1, Name);
 }

/*
 * This is the code to deal with user interrupts
 * In order to do this cleanly, we effectively poll for interrupts in the
 * main interpreter loop (above). Possibly any routine which could take an
 * inordinate amount of time out of this loop should also poll the `interrupted'
 * variable and generate an error if one is found.
 * Currently none do - "run"ning a file is done by the interpreter above.
 *
 */

static catchint ()
 {
 	VOID signal (SIGINT, catchint);
 	interrupted = TRUE;
 }

int Interrupted ()
 {
	return interrupted && !protected;
 }

/*
 *	An intrinsic operator generates an error condition by returning FALSE.
 *	The usual way to do this is to call Error with the PostScript name of
 *	the error condition.
 *	
 */

static Object InsErr (dict, name, fn) Object dict; char *name; int (*fn)();
 {
 	Object op;
 	
 	DictStore (dict, op = NameFrom (name), MakeOp (name, fn, 1, 0, 0, 1, Name));
 	return op;
 }

static ErrorFn (name) Object name;
 {
 	PrintName (Self);
 	printf (" in operator ");
 	PrintName (name);
 	putchar ('\n');
 	return Push (ExecStack, Cvx (NameFrom ("stop")));
 }

int Error (error) Object error;
 {
 	error_name = error;
 	return FALSE;
 }

Object MakeObject (type) Type type;
 {
 	Object res;
 	
 	res.type = type;
 	res.flags = READABLE | WRITEABLE;
 	res.Length = res.u.Integer = 0;
 	
 	return res;
 }

int OpCheck (args, res) int args, res;
 {
 	if (OpStack->stack_fill < args)
 		return Error (POpUnderflow);
 	else if (OpStack->stack_fill - args + res > OpStack->stack_size)
 		return Error (POpOverflow);
 	else
 		return TRUE;
 }

PanicIf (cond, s) int cond; char *s;
 {
 	if (cond)
 		Panic (s);
 }

Panic (s) char *s;
 {
	fprintf (stderr, "PostScript panic: %s\n", s);
 	fprintf (stderr, "Please report this fault to the support person for this program\n");
 	Cleanup ();
 	exit (1);
 }

int min (a, b) int a, b;
 {
 	return a < b ? a : b;
 }

int rCheck (object) Object object;
 {
 	if (object.type == Dictionary)
 		return object.u.Dictionary->dict_flags & READABLE;
 	else
 		return object.flags & READABLE;
 }

int wCheck (object) Object object;
 {
 	if (object.type == Dictionary)
 		return object.u.Dictionary->dict_flags & WRITEABLE;
 	else
 		return object.flags & WRITEABLE;
 }

int xCheck (object) Object object;
 {
 	return object.flags & EXECUTABLE;
 }

Object Cvx (o) Object o;
 {
	o.flags |= EXECUTABLE;
	return o;
 }

Object Cvlit (o) Object o;
 {
	o.flags &= ~EXECUTABLE;
	return o;
 }

Object ExecOnly (o) Object o;
 {
 	if (o.type == Dictionary)
 		o.u.Dictionary->dict_flags &= ~(READABLE | WRITEABLE);
 	else
 		o.flags &= ~(READABLE | WRITEABLE);
 	return ReadOnly (o);
 }

Object ReadOnly (o) Object o;
 {
 	if (o.type == Dictionary)
 		o.u.Dictionary->dict_flags &= ~WRITEABLE;
 	else
 		o.flags &= ~WRITEABLE;
 	return o;
 }

Object WriteOnly (o) Object o;
 {
 	if (o.type == Dictionary)
 		o.u.Dictionary->dict_flags &= ~READABLE;
 	else
 		o.flags &= ~READABLE;
 	return o;
 }

Object SameFlags (a, b) Object a, b;
 {
	b.flags = a.flags;
	return b;
 }

int Max (a, b) int a, b;
 {
	return a > b ? a : b;
 }

int Min (a, b) int a, b;
 {
	return a < b ? a : b;
 }
