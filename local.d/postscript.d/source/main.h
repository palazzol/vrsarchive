/*
 * Copyright (C) Crispin Goswell 1987, All Rights Reserved.
 */

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <setjmp.h>
#include <strings.h>

#define NONE		(-2)
#define EOS		'\0'
#define TRUE		1
#define FALSE		0

#define BUFSIZE	     1000

#define VOID	(void)

#define READABLE	 01
#define WRITEABLE	 02
#define EXECUTABLE	 04
#define PERMANENT	010
#define PIPED		020

typedef struct dict_struct *Type;

typedef struct object
 {
 	int flags;
	Type type;
	int Length;
	union
	 {
		int Integer, Boolean, Font;
	 	float Real;
	 	Type Dictionary;
	 	unsigned char *String;
	 	struct name_struct *Name;
	 	struct op_struct *Operator;
	 	struct file_struct *File;
	 	struct object *Array;
	 } u;
	
 } Object;
 
enum file_type { StringFile, StreamFile };

struct file_struct
 {
 	enum file_type file_type;
 	int available;
 	union
 	 {
 	 	unsigned char	*c_ptr;
 	 	FILE	*f_ptr;
 	 } f;
 };

struct dict_entry
 {
 	Object entry_key, entry_value;
 };

struct dict_struct
 {
 	int dict_flags, dict_size, dict_fill;
 	struct dict_entry *dict_body;
 };

typedef struct stack
 {
 	int stack_fill, stack_size;
 	Object overflow, underflow, *stack_body;
 } *Stack, StackOb;
 
Object SameFlags (), MakeObject (), Cvx (), Cvlit (), ReadOnly (), WriteOnly (), ExecOnly ();
int OpCheck (), min (), rCheck (), wCheck (), xCheck ();
Object MakeArray (), ParseArray (), getArray (), getIArray (), *BodyArray ();
Object MakeBoolean ();


Object MakeDict (), DictLoad (), Lookup (), DictFrom (), Load ();
Type MakeType (), TypeOf (), BodyDict ();

extern int EqTrue (), Equal ();

Object FileFrom (), FileString ();

int Getch ();
struct file_struct *BodyFile ();
Object MakeInteger (), IntReal ();

Object ParseNumber ();
float Deg (), Rad ();
Object ParseId (), NameFrom (), MakeName (), Cvn (), StringName ();
unsigned char *BodyName ();
Object MakeOp (), NameOperator ();
Object Parse ();


int PolyFirst (), PolySecond (), PolyThird (), PolyPair ();

Object MakeReal (), RealInteger ();

float BodyReal (), BodyFloat ();

char *Malloc ();
Object Pop (), Top (), Where (), DictLookup ();
int Push ();

extern Object MakeString (), StringFrom (), getIString (), ParseString (), ParseHexString ();
unsigned char *BodyString ();
int lengthString ();

extern Object PDictFull;
extern Object PDictOverflow,		PInvFont,		PSyntaxError;
extern Object PDictUnderflow,		PInvRestore,		PTypeCheck;
extern Object PExecOverflow,		PIOError,		PUndefined;
extern Object PExecUnderflow,		PLimitCheck,		PUnFilename;
extern Object PInterrupt,		PNoCurrentPoint,	PUnResult;
extern Object PInvAccess,		PRangeCheck,		PUnMatched;
extern Object PInvExit,			POpOverflow,		PUnregistered;
extern Object PInvFileAccess,		POpUnderflow,		PVMError;

extern Type Boolean, Mark, String, Real, Poly, Operator;
extern Type Name, File, Dictionary, Condition, Null, Integer;
extern Type Array, Mark, Condition, Null, Float, FontID;

extern Object SysDict, Absent, Nil;
extern Object True, False, Marker, Self;
extern Object OpInterp, Lbracket, Rbracket;
extern Object StatementEdit, Fstdin, Fstdout, Fstderr;

extern Stack OpStack, ExecStack, DictStack;
extern jmp_buf env;
extern int interactive, verbose;
extern char default_library[], *library;
extern unsigned char *Bcopy ();
extern FILE *vfp, *Fopen ();
extern void Fclose ();


#define TypeOf(a) 	((a).type)

#define Push(stack, object) (((stack)->stack_fill != (stack)->stack_size) ?	\
		((stack)->stack_body[(stack)->stack_fill] = (object), (stack)->stack_fill++, TRUE) : FALSE)

#define Pop(stack) 	((stack)->stack_body[--(stack)->stack_fill])

#define Top(stack)	((stack)->stack_body[(stack)->stack_fill - 1])

#define Height(stack)	((stack)->stack_fill)

#define MaxStack(stack)		((stack)->stack_size)

extern int getchbuf;

#define BodyFile(file) ((file).u.File)

#define StatusFile(file) (BodyFile(file)->available != 0)

#define Getch(file) ((StatusFile(file) && BodyFile(file)->file_type == StreamFile) ?\
	((getchbuf = getc (BodyFile(file)->f.f_ptr)), \
		((getchbuf != EOF) ? getchbuf : ((BodyFile(file)->available = 0), Close (file), EOF))) \
	: GeneralGetch (file))
