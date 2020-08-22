/*
		Written by Jeff Slaney November 1985 
				helped by Phil Gross

*/
#include <stdio.h>

#define ESC 27

/* Tektronix 4110 Series Command Reference, p. 7-226, 12/11/85, jrs */
/* Only valid for Tektronix 4115 terminals */
coor_mode(mode,size)
int mode;
{
}

/* Tektronix 4110 Series Command Reference, p. 7-191, 2/29/84, phg */
select_code(mode)
int mode;
{
}

/* Tektronix 4110 Series Command Reference, p. 7-251, 2/29/84, phg */
fixup_level(level)
int level;
{
}

/* Tektronix 4110 Series Command Reference, p. 7-40, 2/28/84, phg */
clear_dialog()
{
	printf("%c[H%c[J", ESC, ESC);
}

/* Tektronix 4110 Series Command Reference, p. 7-237, 2/29/84, phg */
dialog_visib(status)
int status;
{
	if (status)
		printf("\030");
	else
		printf("\035\037");
}


/* Tektronix 4110 Series Command Reference, p. 7-356, 2/29/84, phg */
surf_visib(elems, arr)
int elems, arr[];
{
}

/* Tektronix 4110 Series Command Reference, p. 7-360, 2/29/84, phg */
text_index(text)
int text;
{
}


/* Tektronix 4110 Series Command Reference, p. 7-276, 2/29/84, phg */
tx_size(width, height, space)
int width, height, space;
{
	static char size[] = {
		';', ':', '9', '8', '1', '2', '3'
	};
	printf("%c%c", ESC, size[width/15]);
}


/* Tektronix 4110 Series Command Reference, p. 7-277, 2/29/84, phg */
tx_slnt(angle)
float angle;
{
	static char rot[] = { '2', '4', '6', '0' };
	int i = angle/90;
	printf("%c%c",ESC,rot[i]);
}


/* Tektronix 4110 Series Command Reference, p. 7-270, 2/29/84, phg */
tx_font(font)
int font;
{
	if (font == 2)
		printf("%c<", ESC);
	else
		printf("%c=", ESC);
}


/* Tektronix 4110 Series Command Reference, p. 7-274, 2/29/84, phg */
tx_prec(precision)
int precision;
{
}


/* Tektronix 4110 Series Command Reference, p. 7-275, 2/29/84, phg */
tx_rot(angle)
float angle;
{
}


/* Tektronix 4110 Series Command Reference, p. 7-91, 2/29/84, phg */
end_segment()
{
}


/* Tektronix 4110 Series Command Reference, p. 7-55, 2/29/84, phg */
del_segment(segment)
int segment;
{
}


/* Tektronix 4110 Series Command Reference, p. 7-23, 2/28/84, phg */
begin_seg(segment_num)
int segment_num;
{
}

/* Tektronix 4110 Series Command Reference, p. 7-140, 2/29/84, phg */
page()
{
	printf("\031");
}


/* Tektronix 4110 Series Command Reference, p. 7-370, 2/29/84, phg */
set_window(llx, lly, urx, ury)
int llx, lly, urx, ury;
{
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
	printf("\035");
	trans_xy(x, y);
}



/* Tektronix 4110 Series Command Reference, p. 7-72, 2/29/84, phg */
draw(x, y)
int x, y;
{
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
		putchar(arr[t]);
}


/* Tektronix 4110 Series Command Reference, p. 7-129, 3/19/84, phg */
/* prints a tek translated integer */
trans_int(x)
int x;
{
	int arr[3];
	if ((getbits(x, 31, 1)) == 1) {
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
