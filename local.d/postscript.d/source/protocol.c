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
#include <stdio.h>

#include "main.h"
#include "graphics.h"
#include "protocol.h"

#define SCALE 16384

static FILE *devfpi, *devfpo;
char *getenv (), *strcpy (), *strcat ();
unsigned char getb ();

void master_protocol ()
 {
 	char name [BUFSIZ], *e = getenv ("POSTSCRIPTDEVICE");
 	
 	if (e == NULL || *e == '\0')
 		VOID strcpy (name, "|viewer");
 	else
 		VOID strcpy (name, e);
 	if (*name == '|')
 	 {
 	 	int ip[2], op[2];
 	 	
 	 	pipe (ip);
 	 	pipe (op);
 	 	devfpi = fdopen (ip[0], "r");
 	 	devfpo = fdopen (op[1], "w");
 	 	
 	 	if (fork () == 0)
 	 	 {
 	 	 	dup2 (op[0], 0);
 	 	 	dup2 (ip[1], 1);
 	 	 	close (op[1]);
 	 	 	close (ip[0]);
 	 	 	
 	 	 	execl ("/bin/sh", "PSDEV", "-c", name + 1, NULL);
 	 	 	fprintf (stderr, "NO SHELL!\n");
 	 	 	exit (1);
 	 	 }
 	 	else
 	 	 {
 	 	 	close (op[0]);
 	 	 	close (ip[1]);
 	 	 }
 	 }
 	else if (*name == '%')
 	 {
 	 	devfpi = fopen (name + 1, "r");
 	 	devfpo = fopen (name + 1, "w");
 	 }
 	else
 	 {
 	 	devfpi = NULL;
 	 	devfpo = fopen (name, "w");
 	 }
 }

void slave_protocol ()
 {
	devfpi = stdin;
	devfpo = stdout;
 }

void send_colour (colour) Colour colour;
 {
 	send_small (colour.hue);
 	send_small (colour.saturation);
 	send_small (colour.brightness);
 }

Colour recv_colour ()
 {
 	Colour res;
 	
 	res.hue = recv_small ();
 	res.saturation = recv_small ();
 	res.brightness = recv_small ();
 	
 	return res;
 }

void send_small (f) float f;
 {
 	send_short ((int) (f * SCALE));
 }

float recv_small ()
 {
	return (float) recv_short () / SCALE;
 }

void send_point (p) DevicePoint p;
 {
	send_short (p.dx);
	send_short (p.dy);
 }

DevicePoint recv_point ()
 {
	short r = recv_short ();
	
	return NewDevicePoint (r, recv_short ());
 }

void send_byte (b) unsigned char b;
 {
	putc (b, devfpo);
 }

unsigned char recv_byte ()
 {
	return getb (devfpi);
 }

void send_short (i) short i;
 {
 	send_byte (i & 0xff);
 	send_byte (i >> 8);
 }

short recv_short ()
 {
	short i;
	
	i = recv_byte () & 0xff;
	i |= recv_byte () << 8;
	
	return i;
 }

void send_float (f) float f;
 {
	fprintf (devfpo, "%g\n", f);
 }

float recv_float ()
 {
 	float f;
 	char buf [BUFSIZ], *p = buf;
 	
 	while ((*p++ = getb (devfpi)) != '\n')
 		;
 	*p++ = '\0';
 	sscanf (buf, "%f\n", &f);
 	
 	return f;
 }

void send_string (s, len) char *s; int len;
 {
 	PanicIf (len != fwrite (s, 1, len, devfpo), "could not send bitmap from driver");
 }

void recv_string (s, len) char *s; int len;
 {
 	while (len--)
 		*s++ = getb (devfpi);
 }

void flush_protocol ()
 {
 	fflush (devfpo);
 }

int can_recv ()
 {
 	return devfpi != NULL;
 }
 
char buffer [BUFSIZ], *p;
int remaining = 0;

unsigned char getb (fp) FILE *fp;
 {
 	if (remaining > 0)
 	 {
 	 	--remaining;
 	 	return *p++;
 	 }
 	if ((remaining = read (fp->_file, buffer, BUFSIZ)) <= 0)
 	 	exit (1);
 	p = buffer;
 	return getb (fp);
 }
