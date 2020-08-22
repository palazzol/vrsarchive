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
#define THRESHOLD 4096
#define NULL 0

static char *FreeSpace = NULL, *sbrk ();
static int Left = 0;

char *malloc (size) unsigned size;
 {
 	char *res;
 	
 	if (FreeSpace == NULL)
 		FreeSpace = sbrk (0);
 	
 	size = (size + 3) & ~3;
 	res = FreeSpace;
 	if (size > Left)
 	 {
 	 	int chunk;

 	 	if (sbrk (chunk = size > THRESHOLD ? size : THRESHOLD) == (char *) -1)
 	 		return NULL;
 	 	Left += chunk;
 	 }
 	Left -= size; 		
 	FreeSpace += size;
 	
 	return res;
 }

int free (block) char *block;
 {
 }

char *realloc (b, size) char *b; unsigned size;
 {
 	char *block = malloc (size);
 	
 	while (--size > 0)
 		*block++ = *b++;
 	return block;
 }

char *calloc (size1, size2) unsigned size1, size2;
 {
 	unsigned total = size1 * size2;
 	char *res = malloc (total), *p = res;
 	
 	while (--total > 0)
 		*p++ = 0;
 	return res;
 }
