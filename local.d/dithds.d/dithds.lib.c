/*
		Written by Jeff Slaney November 1985 
				helped by Phil Gross

*/

#define ESC 27

/* Tektronix 4110 Series Command Reference, p. 7-226, 12/11/85, jrs */
/* Only valid for Tektronix 4115 terminals */
coor_mode(mode,size)
int mode;
{
	printf("%cUX",ESC);
	trans_int(mode);
	trans_int(size);
}

/* Tektronix 4110 Series Command Reference, p. 7-191, 2/29/84, phg */
select_code(mode)
int mode;
{
	if (mode == 0)
		printf("%c%%\!0",ESC);
	else if (mode == 1)
		printf("%c%%\!1",ESC);
	else
		printf("%c%%\!2",ESC);
}

/* Tektronix 4110 Series Command Reference, p. 7-251, 2/29/84, phg */
fixup_level(level)
int level;
{
	printf("%cRF",ESC);
	trans_int(level);
}

/* Tektronix 4110 Series Command Reference, p. 7-40, 2/28/84, phg */
clear_dialog()
{
	printf("%cLZ",ESC);
}

/* Tektronix 4110 Series Command Reference, p. 7-237, 2/29/84, phg */
dialog_visib(status)
int status;
{
	printf("%cLV",ESC);
	trans_int(status);
}


/* Tektronix 4110 Series Command Reference, p. 7-356, 2/29/84, phg */
surf_visib(elems, arr)
int elems, arr[];
{
	printf("%cRI",ESC);
	int_array(elems, arr);
}

/* Tektronix 4110 Series Command Reference, p. 7-360, 2/29/84, phg */
text_index(text)
int text;
{
	printf("%cMT",ESC);
	trans_int(text);
}


/* Tektronix 4110 Series Command Reference, p. 7-276, 2/29/84, phg */
/*I001 graph*/tx_size(width, height, space)
int width, height, space;
{
	printf("%cMC",ESC);
	trans_int(width);
	trans_int(height);
	trans_int(space);
}


/* Tektronix 4110 Series Command Reference, p. 7-277, 2/29/84, phg */
/*I001 graph*/tx_slnt(angle)
float angle;
{
	printf("%cMA",ESC);
	trans_real(angle);
}


/* Tektronix 4110 Series Command Reference, p. 7-270, 2/29/84, phg */
/*I001 graph*/tx_font(font)
int font;
{
	printf("%cMF",ESC);
	trans_int(font);
}


/* Tektronix 4110 Series Command Reference, p. 7-274, 2/29/84, phg */
/*I001 graph*/tx_prec(precision)
int precision;
{
	printf("%cMQ",ESC);
	trans_int(precision);
}


/* Tektronix 4110 Series Command Reference, p. 7-275, 2/29/84, phg */
/*I001 graph*/tx_rot(angle)
float angle;
{
	printf("%cMR",ESC);
	trans_real(angle);
}


/* Tektronix 4110 Series Command Reference, p. 7-91, 2/29/84, phg */
end_segment()
{
	printf("%cSC",ESC);
}


/* Tektronix 4110 Series Command Reference, p. 7-55, 2/29/84, phg */
del_segment(segment)
int segment;
{
	printf("%cSK",ESC);
	trans_int(segment);
}


/* Tektronix 4110 Series Command Reference, p. 7-23, 2/28/84, phg */
begin_seg(segment_num)
int segment_num;
{
	printf("%cSO",ESC);
	trans_int(segment_num);
}

/* Tektronix 4110 Series Command Reference, p. 7-140, 2/29/84, phg */
page()
{
	printf("%c%c",ESC,12);  /* ESCAPE and a CONTROL L */
}


/* Tektronix 4110 Series Command Reference, p. 7-370, 2/29/84, phg */
set_window(llx, lly, urx, ury)
int llx, lly, urx, ury;
{
	printf("%cRW",ESC);
	trans_xy(llx, lly);
	trans_xy(urx, ury);
}


#include <math.h>
float torads();
arc(x, y, r, st, sp, point)
int x, y, r, point;
float st, sp;
/* x and y are the center of the circle
 * r is the radius of the arc
 * st is the starting degree angle of the arc
 * sp is the stopping degree angle of the arc
 * point is how many points along the arc are plotted
 */
{
	int t1, t2;
	float k, inc;
	t1 = x + (r * cos(torads(st)));
	t2 = y + (r * sin(torads(st)));
	move(t1, t2);
	inc = (sp - st)/ point;
	for ( k = inc; k < (sp - st); k += inc) {
		t1 = (x + r * cos(torads(st + k)));
		t2 = (y + r * sin(torads(st + k)));
		draw(t1, t2);
	}
	t1 = (x + r * cos(torads(sp)));
	t2 = (y + r * sin(torads(sp)));
	draw(t1, t2);
}

/* converts degrees to radians */
float torads(coord)
float coord;
{
	return(3.1415962/180.0 * coord);
}


/* Tektronix 4110 Series Command Reference, p. 7-137, 2/29/84, phg */
move(x, y)
int x, y;
{
	printf("%cLF",ESC);
	trans_xy(x, y);
}



/* Tektronix 4110 Series Command Reference, p. 7-72, 2/29/84, phg */
draw(x, y)
int x, y;
{
	printf("%cLG",ESC);
	trans_xy(x, y);
}


/* Tektronix 4110 Series Command Reference, p. 7-391, 2/29/84, phg */
/* translate coords to something the tek likes */
trans_xy(x, y)
int x, y;
{
	int t, arr[5];
	arr[0] = 0040 | getbits(y, 11, 5);
	arr[1] = 0140 | (getbits(y, 1, 2) << 2) | getbits(x, 1, 2);
	arr[2] = 0140 | getbits(y, 6, 5);
	arr[3] = 0040 | getbits(x, 11, 5);
	arr[4] = 0100 | getbits(x, 6, 5);
	for (t = 0; t <= 4; t++)
		printf("%c", arr[t]);
}


/* Tektronix 4110 Series Command Reference, p. 7-160, 3/19/84, phg */
trans_real(decimal)
float decimal;
{
	float epsilon = 0.00006104;
	float temp;
	int trunc, neg_flag, mantissa, exponent;
	temp = decimal;
	neg_flag = 0;
	exponent = 0;		/* I001 exponent may be used before set */
	if (temp < 0) {
		neg_flag = -1;
		temp = neg_flag * temp;
	}
	trunc = temp / 1;
	while ( temp > (trunc + epsilon) && trunc < 16383) {
		temp = temp * 2.0;
		exponent--;
		trunc = temp / 1;
	}
	while (temp > 32767.0) {
		temp = temp / 2.0;
		exponent++;
	}
	mantissa = (temp + 0.5) / 1;
	if (neg_flag == -1)
		mantissa = -mantissa;
	trans_int(mantissa);
	trans_int(exponent);
}



/* Tektronix 4110 Series Command Reference, p. 7-129, 3/19/84, phg */
/* prints a tek translated integer */
trans_int(x)
int x;
{
	int /*t,*/ arr[3];		/* I001 t set but not used */
	if ((/* t = */getbits(x, 31, 1)) == 1) { /* I001 t set but not used */
		x = ~x + 0001;
		arr[2] = 0040 | getbits(x, 3, 4);
	}
	else
		arr[2] = 0060 | getbits(x, 3, 4);
	arr[0] = 0100 | getbits(x, 15, 6);
	arr[1] = 0100 | getbits(x, 9, 6);
	if (arr[0] != '@') {
		putchar(arr[0]);
		putchar(arr[1]);
		putchar(arr[2]);
	}
	else if (arr[1] != '@') {
		putchar(arr[1]);
		putchar(arr[2]);
	}
	else if (arr[2] != '@')
		putchar(arr[2]);
}



/* This routine returns the value of n bits beginning with the p'th bit of x */
/*
 *	t = getbits(57, 5, 3)
 *	(x = 57
 *	x = 01110111
 *	p = 5, n = 3
 *	x = 00000110
 *	x = 6)
 *	t = 6
 */
getbits(x, p, n)
unsigned x, p, n;
{
	return((x >> (p+1-n)) & ~(~0 << n));
}


/* Tektronix 4110 Series Command Reference, p. 7-6, 3/19/84, phg */
/* prints a character string after printing a count */
#include <stdio.h>
char	*rmtb;

trans_char(string)
char string[];
{
	/* int char_len; I001 char_len unused in trans_char() */
	int i;

	rmtb = string;
	i = strlen(string) - 1;
	while (rmtb[i] == ' ')
		i--;
	rmtb[++i] = '\0';
	trans_int(i);
	printf("%s", rmtb);
}

/* Tektronix 4110 Series Command Reference, p. 7-6, 3/19/84, phg */
/* routine to transmit an encoded array */
int_array(elems, array)
int elems, array[];
{
	int i;
	trans_int(elems);
	for (i = 0; i < elems; i++) {
		trans_int(array[i]);
	}
}
