#include <stdio.h>

/*		-- Miscellaneous defines --				     */
#define FALSE 0
#define TRUE 1
#define MAXCOL 80
#define MAXLI 24

extern char *tgetstr();

int lastx, lasty;
struct _c {
	struct _c *c_next;
	int c_line, c_column;
	char c_mark;
} *clist;

/*		-- Global variables --					     */
char *tent;                                               /* Pointer to tbuf */
char *PC;                                                    /* Pad character */
char *UP, *BC;                                /* Upline, backsapce character */
short ospeed;                                       /* Terminal output speed */
int tglen;

char *cm,                                                   /* Cursor motion */
     *cl,                                                    /* Clear screen */
     *ti,						    /* Init terminal */
     *te;						   /* Reset terminal */
int  li,                                                  /* lines on screen */
     co;                                                    /* columns ditto */
char screen[MAXLI+1][MAXCOL];
char newscreen[MAXLI+1][MAXCOL];

main(ac, av)
int ac;
char **av;
{
	srand(getpid());
	tinit(getenv("TERM"));
	if(av[1])
		while(*++av)
			dropf(*av);
	else
		fdropf(stdin);
	tend();
}

at(x, y, c)
int x, y;
char c;
{
#ifdef DEBUG
	_at(x, y);
#else
	if(y==lasty) {
		if(x!=lastx) {
			if(x<lastx && lastx-x<tglen)
				while(x<lastx) {
					putchar('\b');
					lastx--;
				}
			else if(x>lastx && x-lastx<tglen)
				while(x>lastx) {
					putchar(newscreen[lasty][lastx]);
					lastx++;
				}
			else
				_at(x, y);
		}
	} else
		_at(x, y);
#endif
	c &= ~0200;
	putchar(c);
	if(c >= ' ' && c != '\177')
		lastx++;
	if(lastx>=co) {
		lastx -= co;
		lasty++;
	}
}

_at(x, y)
int x, y;
{
	outs(tgoto(cm, x, y));
	lastx = x;
	lasty = y;
}

tinit(name)
char *name;
{
	static char junkbuf[1024], *junkptr;
	char tbuf[1024];
	int  intr();

	junkptr = junkbuf;

	tgetent(tbuf, name);

	PC = tgetstr("pc", &junkptr);
	UP = tgetstr("up", &junkptr);
	BC = tgetstr("bc", &junkptr);
	cm = tgetstr("cm", &junkptr);
	cl = tgetstr("cl", &junkptr);
	ti = tgetstr("ti", &junkptr);
	te = tgetstr("te", &junkptr);
	li = min(tgetnum("li"), MAXLI);
	co = min(tgetnum("co"), MAXCOL);
	tglen = strlen(tgoto(co-1, li-1));
}

tend()
{
	outs(te);
	_at(0, li-1);
	putchar('\n');
	fflush(stdout);
}

readscreen(fp)
FILE *fp;
{
	int line, column, p;
	char tmp[256];

	for(line=0; line<li; line++)
		for(column=0; column<co; column++)
			newscreen[line][column] = screen[line][column] = ' ';
	for(column=0; column<co; column++)
		newscreen[li][column] = screen[li][column] = '*';
	line=0;
	while(line<li) {
		if(!fgets(tmp, 256, fp))
			return;

		for(column=0, p=0; tmp[p]; p++) {
			tmp[p] &= ~0200;
			if(tmp[p] < ' ' || tmp[p] == 127)
				switch(tmp[p]) {
					case '\t':
						while(++column % 8)
							continue;
						break;
					case '\n':
						column = 0;
						line++;
						break;
					default:
						newscreen[line][column] = '^';
						column++;
						if(column>=co) {
							column -= co;
							line++;
						}
						newscreen[line][column] =
							(tmp[p]+'@') & 127;
						column++;
						break;
				}
			else {
				newscreen[line][column] = tmp[p];
				column++;
			}
			if(column >= co) {
				column -= co;
				line++;
			}
			if(line >= li)
				break;
		}
	}
	for(column=0; column<co; column++)
		newscreen[line][column] = screen[li][column] = '*';
}

drawscreen()
{
	lastx = lasty = 0;
	outs(cl);
	update();
}

update() /* copy new screen back to old screen */
{
	int l, c;

	for(l=0; l<li; l++)
		for(c=0; c<co; c++)
			if(screen[l][c] != newscreen[l][c]) {
				if((screen[l][c] & ~0200) !=
				   (newscreen[l][c] & ~0200))
					at(c, l, newscreen[l][c]);
				screen[l][c] = newscreen[l][c];
			}
}

drop(line, column)
int line, column;
{
	struct _c *hold;

	if(line<0 || line>=li || column<0 || column>=co ||
	   (line>=li-2 && column >= co-1) || /* scroll potential */
	   screen[line][column]==' ' || /* empty */
	   screen[line][column] & 0200) /* already in list */
		return;
	if(screen[line+1][column]!=' ' &&
	   (column==co-1 ||screen[line+1][column+1]!=' ') &&
	   (column==0 ||screen[line+1][column-1]!=' '))
		return;

	hold = (struct _c *) malloc(sizeof(struct _c));
	hold -> c_next = clist;
	hold -> c_column = column;
	hold -> c_line = line;
	hold -> c_mark = 0;
	screen[line][column] |= 0200;
	clist = hold;
}

drops()
{
	int l, c;
	struct _c *hold;
	for(hold = clist; hold; hold=hold->c_next) {
		int line = hold->c_line, column=hold->c_column;
		if(line>= li-2 && column>=co-1) {
			newscreen[line][column] &= ~0200;
			screen[line][column] &= ~0200;
			hold->c_mark = 1;
			continue;
		}
		drop(line+1, column);
		drop(line, column+1);
		drop(line-1, column);
		drop(line, column-1);
		if(newscreen[line+1][column]==' ') {
			newscreen[line+1][column] = screen[line][column];
			newscreen[line][column] = ' ';
			line++;
		} else if(rand()&01000) {
			if(column>0 && newscreen[line][column-1] == ' ' &&
			    newscreen[line+1][column-1]==' ') {
				newscreen[line][column-1] =
					screen[line][column];
				newscreen[line][column] = ' ';
				column--;
			}
			else if(column<co-1 &&
				newscreen[line][column+1] == ' ' &&
				newscreen[line+1][column+1]==' ') {
					newscreen[line][column+1] =
						screen[line][column];
					newscreen[line][column] = ' ';
					column++;
			}
			else {
				screen[line][column] &= ~0200;
				newscreen[line][column] &= ~0200;
				hold -> c_mark = 1;
			}
		} else {
			if(column<co-1 && newscreen[line][column+1] == ' ' &&
			    newscreen[line+1][column+1]==' ') {
				newscreen[line][column+1] =
					screen[line][column];
				newscreen[line][column] = ' ';
				column++;
			}
			else if(column>0 && newscreen[line][column-1] == ' ' &&
			    newscreen[line+1][column-1]==' ') {
				newscreen[line][column-1] =
					screen[line][column];
				newscreen[line][column] = ' ';
				column--;
			}
			else {
				newscreen[line][column] &= ~0200;
				screen[line][column] &= ~0200;
				hold -> c_mark = 1;
			}
		}
		hold -> c_column = column;
		hold -> c_line = line;
		fflush(stdout);
	}

	while(clist && clist->c_mark) {
		struct _c *p = clist;
		clist = clist -> c_next;
		free(p);
	}
	hold = clist;
	while(hold && hold->c_next)
		if(hold->c_next->c_mark) {
			struct _c *p = hold->c_next;
			hold->c_next = p->c_next;
			free(p);
		} else
			hold=hold->c_next;
}

droplet(line, column)
int line, column;
{
	int ret;
	while(column>=0 && screen[line][column]!=' ')
		column--;
	column++;
	while(column<co && screen[line][column]!=' ')
		drop(line, column++);
	ret = clist != 0;
	while(clist) {
		drops();
		update();
	}
	return ret;
}

dropscreen()
{
	int column, line;
	int rubbish = 0, count = 0;

	do {
		int start, limit, incr;
		count++;
		rubbish = 0;
		if(count&1) { start=li-2; limit=0; incr = -1; }
		else { start=0; limit=li-2; incr=1; }
		for(line=start; line!=limit && !rubbish; line+=incr) {
			if(line&1)
				for(column=0; column<co && !rubbish; column++)
					rubbish += droplet(line, column);
			else
				for(column=co-1; column>=0 && !rubbish; column--)
					rubbish += droplet(line, column);
		}
	} while(rubbish);
}

dropf(file)
char *file;
{
	FILE *fp;

	if(!(fp = fopen(file, "r"))) {
		perror(file);
		return -1;
	}
	fdropf(fp);
}

fdropf(fp)
FILE *fp;
{
	int i;

	while(!feof(fp)) {
		readscreen(fp);
		drawscreen();
		for(i=0; i<20; i++)
			droplet((rand()>>4) % li, (rand()>>4) % co);
		dropscreen();
	}
}

outs(s)
char *s;
{
	fputs(s, stdout);
}

min(a, b)
int a, b;
{
	if(a<b) return a;
	return b;
}
