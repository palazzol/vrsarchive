%token <dval> NUMBER
%token <sval> NAME

%token ABORT
%token ALL
%token CONTROL
%token CORB
%token COURSE
%token DAMAGE
%token DEAD
%token DELAY
%token DEMAND
%token DESTR
%token DET
%token DISPLAY
%token ELUDE
%token ENG
%token FIRE
%token HELP
%token JETT
%token LAUNCH
%token LOAD
%token LOCK
%token PARAM
%token PHASER
%token POWER
%token PROBE
%token PROXIMITY
%token PURSUE
%token ROTATE
%token SAVE
%token SCAN
%token SPREAD
%token STATUS
%token SURREND
%token SURV
%token TACTIC
%token TOWARD
%token TUBE
%token UNLOAD
%token VERSION
%token WARP

%type <dval> number1
%type <dval> number2
%type <dval> number3
%type <dval> number4
%type <sval> name

%{
#ident "@(#) TREK73 $Header: /home/Vince/cvs/games.d/trek73.d/grammar.y,v 1.1 1987-12-25 20:56:22 vrs Exp $"
/*
 * $Source: /home/Vince/cvs/games.d/trek73.d/grammar.y,v $
 *
 * $Header: /home/Vince/cvs/games.d/trek73.d/grammar.y,v 1.1 1987-12-25 20:56:22 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  87/10/09  12:24:34  12:24:34  okamoto (Jeff Okamoto)
 * Added many extra stuff
 * 
 * Revision 1.1  87/10/09  12:22:55  12:22:55  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
#include "union.h"
	extern char parsed[];
	char string[80];
	double num1;
	double num2;
	double num3;
	double num4;
%}

%%

orders	:
	|	'\n'
	|	error
			{ yyclearin; }
	|	phfire
	|	tufire
	|	phlock
	|	tulock
	|	phrot
	|	turot
	|	phstat
	|	tustat
	|	tuload
	|	probe
	|	control
	|	tactical
	|	display
	|	pursue
	|	elude
	|	course
	|	damage
	|	scan
	|	power
	|	jettison
	|	detonate
	|	dead
	|	param
	|	corb
	|	surr
	|	esurr
	|	selfd
	|	abort
	|	surv
	|	version
	|	save
	|	help
	;

number1	:	NUMBER
			{ num1 = yylval.dval; }
	;

number2	:	NUMBER
			{ num2 = yylval.dval; }
	;
number3	:	NUMBER
			{ num3 = yylval.dval; }
	;
number4	:	NUMBER
			{ num4 = yylval.dval; }
	;

name	:	NAME
			{ strcpy(string, yylval.sval); }
	;

phfire	:	FIRE	PHASER	number1	SPREAD	number2
			{ sprintf(parsed, "1 %.0f %.0f", num1, num2);}
	|	FIRE	PHASER	number1
			{ sprintf(parsed, "1 %.0f", num1); }
	|	FIRE	ALL	PHASER	SPREAD	number1
			{ sprintf(parsed, "1 all %.0f", num1); }
	|	FIRE	ALL	PHASER
			{ strcpy(parsed, "1 all"); }
	|	FIRE	PHASER
			{ strcpy(parsed, "1"); }
	;

tufire	:	FIRE	TUBE	number1
			{ sprintf(parsed, "2 %.0f", num1); }
	|	FIRE	ALL	TUBE
			{ sprintf(parsed, "2 all"); }
	|	FIRE	TUBE
			{ strcpy(parsed, "2"); }
	;

phlock	:	LOCK	PHASER	number1	name
			{ sprintf(parsed, "3 %.0f %s", num1, string);}
	|	LOCK	PHASER	number1
			{ sprintf(parsed, "3 %.0f", num1); }
	|	LOCK	ALL	PHASER	name
			{ (void)sprintf(parsed, "3 all %s", string); }
	|	LOCK	ALL	PHASER
			{ strcpy(parsed, "3 all"); }
	|	LOCK	PHASER
			{ strcpy(parsed, "3"); }
	;

tulock	:	LOCK	TUBE	number1	name
			{ sprintf(parsed, "4 %.0f %s", num1, string);}
	|	LOCK	TUBE	number1
			{ sprintf(parsed, "4 %.0f", num1); }
	|	LOCK	ALL	TUBE	name
			{ (void)sprintf(parsed, "4 all %s", string); }
	|	LOCK	ALL	TUBE
			{ strcpy(parsed, "4 all"); }
	|	LOCK	TUBE
			{ strcpy(parsed, "4"); }
	;

phrot	:	ROTATE	ALL	PHASER	number1
			{ sprintf(parsed, "5 all %.0f", num1); }
	|	ROTATE	ALL	PHASER
			{ strcpy(parsed, "5 all"); }
	|	ROTATE	PHASER	number1	number2
			{ sprintf(parsed,"5 %.0f %.0f", num1, num2); }
	|	ROTATE	PHASER	number1
			{ sprintf(parsed,"5 %.0f", num1); }
	;

turot	:	ROTATE	ALL	TUBE	number1
			{ sprintf(parsed, "6 all %.0f", num1); }
	|	ROTATE	ALL	TUBE
			{ strcpy(parsed, "6 all"); }
	|	ROTATE	TUBE	number1	number2
			{ sprintf(parsed,"6 %.0f %.0f", num1, num2); }
	|	ROTATE	TUBE	number1
			{ sprintf(parsed,"6 %.0f", num1); }
	;

phstat	:	PHASER	STATUS
			{ strcpy(parsed, "7"); }
	;
			  
tustat	:	TUBE	STATUS
			{ strcpy(parsed, "8"); }
	;

tuload	:	LOAD	ALL	TUBE
			{ strcpy(parsed, "9 l all"); }
	|	UNLOAD	ALL	TUBE
			{ strcpy(parsed, "9 u all"); }
	|	LOAD	TUBE	number1
			{ sprintf(parsed, "9 l %.0f", num1); }
	|	UNLOAD	TUBE	number1
			{ sprintf(parsed, "9 u %.0f", num1); }
	|	LOAD	TUBE
			{ strcpy(parsed, "9 l"); }
	|	UNLOAD	TUBE
			{ strcpy(parsed, "9 u"); }
	;
			  
probe	:	LAUNCH	PROBE
			{ strcpy(parsed, "10"); }
	|	LAUNCH	PROBE	number1
			{ sprintf(parsed, "10 %.0f", num1); }
	|	LAUNCH	PROBE	number1	DELAY	number2
			{sprintf(parsed, "10 %.0f %.0f", num1, num2); }
	|	LAUNCH	PROBE	number1	DELAY	number2	PROXIMITY	number3
			{ sprintf(parsed, "10 %.0f %.0f %.0f", num1, num2, num3); }
	|	LAUNCH	PROBE	number1	DELAY	number2	PROXIMITY	number3	TOWARD	name
			{ sprintf(parsed, "10 %.0f %.0f %.0f %s", num1, num2, num3, string); }
	|	LAUNCH	PROBE	number1	DELAY	number2	PROXIMITY	number3	COURSE	number4
			{ sprintf(parsed, "10 %.0f %.0f %.0f\n\r %.0f", num1, num2, num3, num4); }
	;

control	:	CONTROL	PROBE	number1
			{ sprintf(parsed, "11 %.0f", num1); }
	|	CONTROL	PROBE
			{ strcpy(parsed, "11"); }
	|	PROBE	CONTROL	number1
			{ sprintf(parsed, "11 %.0f", num1); }
	|	PROBE	CONTROL
			{ strcpy(parsed, "11"); }
	;

tactical:	TACTIC
			{ strcpy(parsed, "12"); }
	;

display	:	DISPLAY	number1
			{ sprintf(parsed, "13 %.0f", num1); }
	|	DISPLAY
			{ strcpy(parsed, "13"); }
	;

pursue	:	PURSUE	name	WARP	number1
			{ sprintf(parsed, "14 %s %f", string, num1);}
	|	PURSUE	name	number1
			{ sprintf(parsed, "14 %s %f", string, num1);}
	|	PURSUE	name
			{ sprintf(parsed, "14 %s", string); }
	|	PURSUE
			{ strcpy(parsed, "14"); }
	;

elude	:	ELUDE	name	WARP	number1
			{ sprintf(parsed, "15 %s %f", string, num1);}
	|	ELUDE	name	number1
			{ sprintf(parsed, "15 %s %f", string, num1);}
	|	ELUDE	name
			{ sprintf(parsed, "15 %s", string); }
	|	ELUDE
			{ strcpy(parsed, "15"); }
	;

course	:	COURSE	number1	WARP	number2
			{ sprintf(parsed, "16 %.0f %0f", num1, num2);}
	|	COURSE	number1	number2
			{ sprintf(parsed, "16 %.0f %0f", num1, num2);}
	|	COURSE	number1
			{ sprintf(parsed, "16 %.0f", num1);}
	|	COURSE	COURSE	number1	WARP	number2
			{ sprintf(parsed, "16 %.0f %0f", num1, num2);}
	|	COURSE	COURSE	number1	number2
			{ sprintf(parsed, "16 %.0f %0f", num1, num2);}
	|	COURSE	COURSE	number1
			{ sprintf(parsed, "16 %.0f", num1);}
	;

damage	:	DAMAGE
			{ strcpy(parsed, "17"); }
	;

scan	:	SCAN	name
			{ sprintf(parsed, "18 %s", string); }
	|	SCAN	number1
			{ sprintf(parsed, "18 %.0f", num1); }
	|	SCAN
			{ strcpy(parsed, "18"); }
	;

power	:	POWER
			{ strcpy(parsed, "19"); }
	;

jettison:	JETT	ENG
			{ strcpy(parsed, "20"); }
	;

detonate:	DET	ENG	name
			{ sprintf(parsed, "21 %s", string); }
	|	DET	ENG
			{ strcpy(parsed, "21"); }
	;

param	:	PARAM
			{ strcpy(parsed, "22"); }
	;

dead	:	DEAD	name
			{ sprintf(parsed, "23 %s", string); }
	|	DEAD
			{ strcpy(parsed, "23"); }
	;
		
corb	:	CORB
			{ strcpy(parsed, "24"); }
	;

surr	:	SURREND
			{ strcpy(parsed, "25"); }
	;

esurr	:	DEMAND	SURREND
			{ strcpy(parsed, "26"); }
	;

selfd	:	DESTR
			{ strcpy(parsed, "27"); }
	;

abort	:	ABORT	DESTR
			{ strcpy(parsed, "28"); }
	;

surv	:	SURV
			{ strcpy(parsed, "29"); }
	;

version	:	VERSION
			{ strcpy(parsed, "30"); }
	;

save	:	SAVE
			{ strcpy(parsed, "31"); }
	;

help	:	HELP
			{ strcpy(parsed, "32"); }
	;
			
%%

#include "lex.yy.c"

yywrap() { return(1); }

yyerror(s) char *s; { fprintf(stderr, "%s", s); }
