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

#define SMALL_SIZE	4096

static char *small_buf;
static int left = 0;

char *Malloc (u) unsigned u;
 {
 	char *malloc (), *res;
 	
	if (0/*u < SMALL_SIZE*/)
 	 {
  	 	char *res;
	 	if (left < u)
	 	 {
	 		small_buf = Malloc (SMALL_SIZE);
	 		left = SMALL_SIZE;
	 	 }
 	 	res = small_buf;
 	 	small_buf += u;
 	 	left -= u;
 	 	
 	 	return res;
 	 }
 	
 	PanicIf ((res = malloc (u)) == NULL, "malloc failed");
 	return res;
 }

Free (s) char *s;
 {
	/* free (s);	*/
 }
