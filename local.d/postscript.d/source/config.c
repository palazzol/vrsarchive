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
#include "main.h"

static int SizeArray = 53, SizeMark = 19, SizeBoolean = 53, SizeNull = 19;
static int SizeCondition = 19, SizeFile = 41, SizeString = 71, SizePoly = 19;
static int SizeInteger = 71, SizeReal = 71, SizeName = 19, SizeOperator = 19;
int SizeSysDict = 1001, SizeDictionary = 53, SizeFloat = 0, SizeFontID = 19;

Type Array, Mark, Boolean, Dictionary, Condition, Null, File, Integer;
Type Real, Name, Operator, String, Poly, Float, FontID;

extern Object Absent, Nil, SysDict;

char default_library[] = "/usr/ral/lib/postscript";

Init ()
 {	
	Nil 	= MakeObject ((Type) 0);
	Null	  	= Nil.type = MakeType (SizeNull);
	EmptyDict (Null);	/* needed because of this recursion */
	
	Dictionary	= MakeType (SizeDictionary);
	Condition	= MakeType (SizeCondition);
 	Name 		= MakeType (SizeName);
	Operator	= MakeType (SizeOperator);
 	Array 		= MakeType (SizeArray);
 	Mark 		= MakeType (SizeMark);
 	Boolean	 	= MakeType (SizeBoolean);
 	File 		= MakeType (SizeFile);
 	Integer 	= MakeType (SizeInteger);
 	Real 		= MakeType (SizeReal);
 	String 		= MakeType (SizeString);
 	Poly 		= MakeType (SizePoly);
 	Float		= MakeType (SizeFloat);
	FontID		= MakeType (SizeFontID);
	
	Message ("InitDictionary");	InitDictionary ();
	Message ("InitOperator");	InitOperator ();
	Message ("InitName");	InitName ();
	Message ("InitPoly");	InitPoly ();
	Message ("InitArray");	InitArray ();
	Message ("InitStack");	InitStack ();
	Message ("InitFile");	InitFile ();
	Message ("InitMisc");	InitMisc ();
	Message ("InitBoolean");	InitBoolean ();
	Message ("InitInteger");	InitInteger ();
	Message ("InitReal");	InitReal ();
	Message ("InitMath");	InitMath ();
	Message ("InitString");	InitString ();
	Message ("InitProperty");	InitProperty ();
	Message ("InitControl");	InitControl ();
	
	Message ("InitMatrix");	InitMatrix ();
	Message ("InitPath");	InitPath ();
 	Message ("InitFill");	InitFill ();
	Message ("InitStroke");	InitStroke ();
	Message ("InitGSave");	InitGSave ();
	Message ("InitDevices");	InitDevices ();
	Message ("InitCache");	InitCache ();
	Message ("InitImage");	InitImage ();
 	Message ("InitState");	InitState ();
	Message ("InitFont");	InitFont ();
	Message ("InitUnix");	InitUnix ();
	
	Install ("nulltype",		DictFrom (Null));
	Install ("dicttype",		DictFrom (Dictionary));
	Install ("conditiontype",	DictFrom (Condition));
 	Install ("nametype",		DictFrom (Name));
	Install ("operatortype",	DictFrom (Operator));
 	Install ("arraytype",		DictFrom (Array));
 	Install ("marktype",		DictFrom (Mark));
 	Install ("booleantype",		DictFrom (Boolean));
 	Install ("filetype",		DictFrom (File));
 	Install ("integertype",		DictFrom (Integer));
 	Install ("realtype",		DictFrom (Real));
 	Install ("stringtype",		DictFrom (String));
 	Install ("polytype",		DictFrom (Poly));
	Install ("fonttype",		DictFrom (FontID));
	
	Install ("version",	StringFrom ("Version 1.4"));
 }
