/*
 * Parse .rc file and interactive input
 *
 * Copyright (c) 1990 by Craig Counterman. All rights reserved.
 *
 * This software may be redistributed freely, not sold.
 * This copyright notice and disclaimer of warranty must remain
 *    unchanged. 
 *
 * No representation is made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty, to the extent permitted by applicable law.
 *
 */



static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/parse_input.c,v 1.1 1990-03-30 16:38:55 vrs Exp $";

#include <ctype.h>
#include "star3.h"
#define PARSE_INPUT
#include "parse_input.h"
  
#define LINELEN 81

static char var_name[LINELEN];

/* Parse input line, return data from keyword map in pointers,
   c_buf is always set to the string after the = if any,
   otherwise c_buf = v_name
   return value is TRUE if valid input, FALSE if not.
   validity of input is determined by the variable_type field in keyword_map
   if the string can be parsed as that type, return TRUE */
int parse_line(sbuf, v_name, r_code, var_type, c_buf, f_buf, i_buf, l_buf)
     char *sbuf;
     char **v_name;
     int *r_code;
     int *var_type;
     char **c_buf;		/* parsed as character string */
     double *f_buf;		/* parsed as double */
     int *i_buf;		/* parsed as integer */
     int *l_buf;		/* parsed as logical (TRUE or FALSE) */
{
  char *cp;
  char tbuf1[LINELEN];
  int i;
  int i_flag, f_flag, l_flag;	/* parsing successful */

  /* remove trailing \n */
  i = strlen(sbuf);
  if (sbuf[i-1] == '\n') sbuf[i-1] = '\0';

  /* eliminate comments */
  for (i=0; ((i <= LINELEN) && (sbuf[i])); i++)
    if (sbuf[i] == '#') sbuf[i] = '\0';
  
  /* Convert "var_name=value" to "var_name value" */
  for (i=0; ((i <= LINELEN) && (sbuf[i])); i++)
      if (sbuf[i] == '=') sbuf[i] = ' ';
  sbuf[i] = '\0';	
  
  /* scan variable name, value */
  /* everything to first space is var_name */
  for (i=0; ((i<=LINELEN) && (sbuf[i] != ' ') && (sbuf[i] != '\t')
	     && (sbuf[i])); i++)
    var_name[i] = sbuf[i];
  var_name[i++] = '\0';

  /* everything after is value, put in cp */
  if (var_name[0]) cp = &sbuf[i];
  else cp = var_name;

  /* Remove leading and trailing spaces, newlines and tabs from cp */
  while ((cp[0] == ' ') || (cp[0] == '\t')) cp++;
  for (i = strlen(cp) - 1;  ((i >= 0) && ((cp[i] == ' ') || (cp[i] == '\t')));
       i--);
  if (i != 0) cp[i+1] = '\0';


  /* lowercase var_name */
  for (i = 0; var_name[i]; i++)
    if (isupper(var_name[i]))
      var_name[i] = tolower(var_name[i]);
  
  
  
  /* variable value may be a string, double, integer, or logical value */
  f_flag = (sscanf(sbuf,"%*s%lf", f_buf) == 1);
  i_flag = (sscanf(sbuf,"%*s%d", i_buf) == 1);
  if (i_flag) {
    l_flag = TRUE;
    *l_buf = ((*i_buf == 0) ? FALSE : TRUE);
  } else {
    l_flag = FALSE;
  };

  i = 0;
  while (tbuf1[i] = cp[i]) i++;
  /* lowercase tbuf1 */
  for (i = 0; tbuf1[i]; i++)
    if (isupper(tbuf1[i]))
      tbuf1[i] = tolower(tbuf1[i]);

  if ((!strcmp(tbuf1,"true"))  || (!strcmp(tbuf1,"on"))) {
    l_flag = TRUE;
    *l_buf = TRUE;
  } else if ((!strcmp(tbuf1,"false")) || (!strcmp(tbuf1,"off"))) {
    l_flag = TRUE;
    *l_buf = FALSE;
  }
  *v_name = var_name;
  *c_buf = cp;

  /* Check validity, may be blank line */
  if ((var_name[0] == 0) && (cp[0] == 0)) {
    *r_code = R_NONE;
    *var_type = V_NONE;
    return TRUE;
  }
  
  i = 0;
  while (keyword_map[i].word[0]) {
    if (!strcmp(var_name, keyword_map[i].word)) {
      *r_code = keyword_map[i].return_code;
      switch (keyword_map[i].variable_type) {
      case V_NONE:
	*c_buf = var_name;
	return TRUE;
	break;
      case V_CHAR:
	return TRUE;
	break;
      case V_FLOAT:
	return f_flag;
	break;
      case V_INT:
	return i_flag;
	break;
      case V_BOOL:
	return l_flag;
	break;
      default:
	return FALSE;
      };
    }
    i++;
  }

  return FALSE;
}


static char *tr_vtype(v_ty)
int v_ty;
{
  switch (v_ty) {
  case V_NONE:
    return " (keyword)";
    break;
  case V_CHAR:
    return " = string";
  case V_FLOAT:
    return " = floating point (e.g. 3.14 or 6e23)";
  case V_INT:
    return " = integer";
  case V_BOOL:
    return " = boolean (e.g. 1 or false or off)";
  default:
    return "";
  }
}

/* send to stdout the keyword_map table */
help_vars(word)
char *word;
{
  int i;


  printf("Word = Expected value type\n");

  if (word[0]) {
    i = 0;
    while (keyword_map[i].word[0]) {
      if (!strcmp(word, keyword_map[i].word)) {
	if (keyword_map[i].synonym[0])
	  printf("%-20s: synonym for %s\n", word, keyword_map[i].synonym);
	else
	  if (keyword_map[i].doc[0])
	    printf("%-20s%s\n", word, keyword_map[i].doc);
	  else
	    printf("%-20s%s\n", word, tr_vtype(keyword_map[i].variable_type));
	return;
      }
      i++;
    };
    printf("%-20s is not a valid keyword\n", word);
  } else {
    i = 0;
    while (keyword_map[i].word[0]) {
	if (keyword_map[i].synonym[0])
	  printf("%-20s: synonym for %s\n", keyword_map[i].word,
		 keyword_map[i].synonym);
	else
	  if (keyword_map[i].doc[0])
	    printf("%-20s%s\n", keyword_map[i].word, keyword_map[i].doc);
	  else
	    printf("%-20s%s\n", keyword_map[i].word,
		   tr_vtype(keyword_map[i].variable_type));
      i++;
    }
  }
}

