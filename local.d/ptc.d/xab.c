#include "globals.h"

 void
checksymbol(ss)
	symset	ss;
{
	if (!(Member((unsigned)(currsym.st), ss.S)))
		error(ebadsymbol);
}

void nextsymbol();

 char
nextchar()
{
	register char	R86;
	char	c;

	if (Eof(input))
		c = null;
	else {
		colno = colno + 1;
		if (Eoln(input)) {
			lineno = lineno + 1;
			colno = 0;
		}
		c = Getchr(input);
		if (echo)
			if (colno == 0)
				Putchr('\n', output);
			else
				Putchr(c, output);
		if (c == tab1)
			colno = ((colno / tabwidth) + 1) * tabwidth;
	}
	if ((*G181_lastchr) > 0) {
		lasttok.A[(*G181_lastchr) - 1] = c;
		(*G181_lastchr) = (*G181_lastchr) + 1;
	}
	R86 = c;
	return R86;
}

 char
peekchar()
{
	register char	R87;

	if (Eof(input))
		R87 = null;
	else
		R87 = input.buf;
	return R87;
}

void nexttoken();

 boolean
idchar(c)
	char	c;
{
	register boolean	R88;

	R88 = (boolean)((c >= 'a') && (c <= 'z') || (c >= '0') && (c <= '9') || (c >= 'A') && (c <= 'Z') || (c == uscore));
	return R88;
}

 boolean
numchar(c)
	char	c;
{
	register boolean	R89;

	R89 = (boolean)((c >= '0') && (c <= '9'));
	return R89;
}

 integer
numval(c)
	char	c;
{
	register integer	R90;

	R90 = (unsigned)(c) - (unsigned)('0');
	return R90;
}

 symtyp
keywordcheck(w, l)
	toknbuf	*w;
	toknidx	l;
{
	register symtyp	R91;
	register unsigned char	n;
	unsigned char	i, j, k;
	keyword	wrd;
	symtyp	kwc;

	if ((l > 1) && (l < keywordlen)) {
		wrd = keytab.A[keytablen].wrd;
		{
			unsigned char	B44 = 1,
				B45 = l;

			if (B44 <= B45)
				for (n = B44; ; n++) {
					wrd.A[n - 1] = w->A[n - 1];
					if (n == B45) break;
				}
		}
		i = 0;
		j = keytablen;
		while (j > i) {
			k = (i + j) / 2;
			if (Cmpstr(keytab.A[k].wrd.A, wrd.A) >= 0)
				j = k;
			else
				i = k + 1;
		}
		if (Cmpstr(keytab.A[j].wrd.A, wrd.A) == 0)
			kwc = keytab.A[j].sym;
		else
			kwc = sid;
	} else
		kwc = sid;
	R91 = kwc;
	return R91;
}

 void
nexttoken(realok)
	boolean	realok;
{
	char	c;
	integer	n;
	boolean	ready;
	toknidx	wl;
	toknbuf	wb;

	(*G181_lastchr) = 0;
	do {
		c = nextchar();
		if (c == '{') {
			do {
				c = nextchar();
				if (diffcomm)
					ready = (boolean)(c == '}');
				else
					ready = (boolean)(((c == '*') && (peekchar() == ')')) || (c == '}'));
			} while (!(ready || Eof(input)));
			if (Eof(input) && !ready)
				error(eeofcmnt);
			if ((c == '*') && !Eof(input))
				c = nextchar();
			c = space;
		} else
			if ((c == '(') && (peekchar() == '*')) {
				c = nextchar();
				do {
					c = nextchar();
					if (diffcomm)
						ready = (boolean)((c == '*') && (peekchar() == ')'));
					else
						ready = (boolean)(((c == '*') && (peekchar() == ')')) || (c == '}'));
				} while (!(ready || Eof(input)));
				if (Eof(input) && !ready)
					error(eeofcmnt);
				if ((c == '*') && !Eof(input))
					c = nextchar();
				c = space;
			}
	} while (!((c != space) && (c != tab1)));
	lasttok.A[1 - 1] = c;
	(*G181_lastchr) = 2;
	lastcol = colno;
	lastline = lineno;
	if (c < okchr)
		c = badchr;
	{
		register struct S180 *W46 = &currsym;

		if (Eof(input)) {
			lasttok.A[1 - 1] = '*';
			lasttok.A[2 - 1] = 'E';
			lasttok.A[3 - 1] = 'O';
			lasttok.A[4 - 1] = 'F';
			lasttok.A[5 - 1] = '*';
			(*G181_lastchr) = 6;
			W46->st = seof;
		} else
			switch (c) {
			  case '|':  case '`':  case '~':  case '}':
			  case 92:  case 95:  case 63:
				error(ebadchar);
				break ;
			  case 'a':  case 'b':  case 'c':  case 'd':
			  case 'e':  case 'f':  case 'g':  case 'h':
			  case 'i':  case 'j':  case 'k':  case 'l':
			  case 'm':  case 'n':  case 'o':  case 'p':
			  case 'q':  case 'r':  case 's':  case 't':
			  case 'u':  case 'v':  case 'w':  case 'x':
			  case 'y':  case 'z':  case 'A':  case 'B':
			  case 'C':  case 'D':  case 'E':  case 'F':
			  case 'G':  case 'H':  case 'I':  case 'J':
			  case 'K':  case 'L':  case 'M':  case 'N':
			  case 'O':  case 'P':  case 'Q':  case 'R':
			  case 'S':  case 'T':  case 'U':  case 'V':
			  case 'W':  case 'X':  case 'Y':  case 'Z':
				wb.A[1 - 1] = lowercase(c);
				wl = 2;
				while ((wl < maxtoknlen) && idchar(peekchar())) {
					wb.A[wl - 1] = lowercase(nextchar());
					wl = wl + 1;
				}
				if (wl >= maxtoknlen) {
					lasttok.A[(*G181_lastchr) - 1] = null;
					error(elongtokn);
				}
				wb.A[wl - 1] = null;
				W46->st = keywordcheck(&wb, wl - 1);
				if (W46->st == sid)
					W46->U.V1.vid = saveid(&wb);
				break ;
			  case '0':  case '1':  case '2':  case '3':
			  case '4':  case '5':  case '6':  case '7':
			  case '8':  case '9':
				wb.A[1 - 1] = c;
				wl = 2;
				n = numval(c);
				while (numchar(peekchar())) {
					c = nextchar();
					n = n * 10 + numval(c);
					wb.A[wl - 1] = c;
					wl = wl + 1;
				}
				W46->st = sinteger;
				W46->U.V3.vint = n;
				if (realok) {
					if (peekchar() == '.') {
						W46->st = sreal;
						wb.A[wl - 1] = nextchar();
						wl = wl + 1;
						while (numchar(peekchar())) {
							wb.A[wl - 1] = nextchar();
							wl = wl + 1;
						}
					}
					c = peekchar();
					if ((c == 'e') || (c == 'E')) {
						W46->st = sreal;
						c = nextchar();
						wb.A[wl - 1] = xpnent;
						wl = wl + 1;
						c = peekchar();
						if ((c == '-') || (c == '+')) {
							wb.A[wl - 1] = nextchar();
							wl = wl + 1;
						}
						while (numchar(peekchar())) {
							wb.A[wl - 1] = nextchar();
							wl = wl + 1;
						}
					}
					if (W46->st == sreal) {
						wb.A[wl - 1] = null;
						W46->U.V4.vflt = savestr(&wb);
					}
				}
				break ;
			  case '(':
				if (peekchar() == '.') {
					c = nextchar();
					W46->st = slbrack;
				} else
					W46->st = slpar;
				break ;
			  case ')':
				W46->st = srpar;
				break ;
			  case '[':
				W46->st = slbrack;
				break ;
			  case ']':
				W46->st = srbrack;
				break ;
			  case '.':
				if (peekchar() == '.') {
					c = nextchar();
					W46->st = sdotdot;
				} else
					if (peekchar() == ')') {
						c = nextchar();
						W46->st = srbrack;
					} else
						W46->st = sdot;
				break ;
			  case ';':
				W46->st = ssemic;
				break ;
			  case ':':
				if (peekchar() == '=') {
					c = nextchar();
					W46->st = sassign;
				} else
					W46->st = scolon;
				break ;
			  case ',':
				W46->st = scomma;
				break ;
			  case '@':  case '^':
				W46->st = sarrow;
				break ;
			  case '=':
				W46->st = seq;
				break ;
			  case '<':
				if (peekchar() == '=') {
					c = nextchar();
					W46->st = sle;
				} else
					if (peekchar() == '>') {
						c = nextchar();
						W46->st = sne;
					} else
						W46->st = slt;
				break ;
			  case '>':
				if (peekchar() == '=') {
					c = nextchar();
					W46->st = sge;
				} else
					W46->st = sgt;
				break ;
			  case '+':
				W46->st = splus;
				break ;
			  case '-':
				W46->st = sminus;
				break ;
			  case '*':
				W46->st = smul;
				break ;
			  case '/':
				W46->st = squot;
				break ;
			  case 39:
				wl = 0;
				ready = false;
				do {
					if (Eoln(input)) {
						lasttok.A[(*G181_lastchr) - 1] = null;
						error(ebadstring);
					}
					c = nextchar();
					if (c == quote)
						if (peekchar() == quote)
							c = nextchar();
						else
							ready = true;
					if (c == null) {
						if (Eof(input))
							error(eeofstr);
						lasttok.A[(*G181_lastchr) - 1] = null;
						error(enulchr);
					}
					if (!ready) {
						wl = wl + 1;
						if (wl >= maxtoknlen) {
							lasttok.A[(*G181_lastchr) - 1] = null;
							error(elongstring);
						}
						wb.A[wl - 1] = c;
					}
				} while (!(ready));
				if (wl == 1) {
					W46->st = schar;
					W46->U.V2.vchr = wb.A[1 - 1];
				} else {
					wl = wl + 1;
					if (wl >= maxtoknlen) {
						lasttok.A[(*G181_lastchr) - 1] = null;
						error(elongstring);
					}
					wb.A[wl - 1] = null;
					W46->st = sstring;
					W46->U.V5.vstr = savestr(&wb);
				}
				break ;
			  default:
				Caseerror(Line);
			}
	}
	if ((*G181_lastchr) == 0)
		(*G181_lastchr) = 1;
	lasttok.A[(*G181_lastchr) - 1] = null;
}

 void
nextsymbol(ss)
	symset	ss;
{
	unsigned char	lastchr;
	unsigned char	*F182;

	F182 = G181_lastchr;
	G181_lastchr = &lastchr;
	nexttoken((boolean)(Member((unsigned)(sreal), ss.S)));
	checksymbol(ss);
	G181_lastchr = F182;
}

 treeptr
typeof(tp)
	treeptr	tp;
{
	register treeptr	R92;
	treeptr	tf, tq;

	tq = tp;
	tf = tq->ttype;
	while (tf == (struct S61 *)NIL) {
		switch (tq->tt) {
		  case nchar:
			tf = typnods.A[(int)(tchar)];
			break ;
		  case ninteger:
			tf = typnods.A[(int)(tinteger)];
			break ;
		  case nreal:
			tf = typnods.A[(int)(treal)];
			break ;
		  case nstring:
			tf = typnods.A[(int)(tstring)];
			break ;
		  case nnil:
			tf = typnods.A[(int)(tnil)];
			break ;
		  case nid:
			tq = idup(tq);
			if (tq == (struct S61 *)NIL)
				fatal(etree);
			break ;
		  case ntype:  case nvar:  case nconst:  case nfield:
		  case nvalpar:  case nvarpar:
			tq = tq->U.V14.tbind;
			break ;
		  case npredef:  case nptr:  case nscalar:  case nrecord:
		  case nconfarr:  case narray:  case nfileof:  case nsetof:
			tf = tq;
			break ;
		  case nsubrange:
			if (tq->tup->tt == nconfarr)
				tf = tq->tup->U.V22.tindtyp;
			else
				tf = tq;
			break ;
		  case ncall:
			tf = typeof(tq->U.V30.tcall);
			if (tf == typnods.A[(int)(tpoly)])
				tf = typeof(tq->U.V30.taparm);
			break ;
		  case nfunc:
			tq = tq->U.V13.tfuntyp;
			break ;
		  case nparfunc:
			tq = tq->U.V15.tpartyp;
			break ;
		  case nproc:  case nparproc:
			tf = typnods.A[(int)(tnone)];
			break ;
		  case nvariant:  case nlabel:  case npgm:  case nempty:
		  case nbegin:  case nlabstmt:  case nassign:  case npush:
		  case npop:  case nif:  case nwhile:  case nrepeat:
		  case nfor:  case ncase:  case nchoise:  case ngoto:
		  case nwith:  case nwithvar:
			fatal(etree);
			break ;
		  case nformat:  case nrange:
			tq = tq->U.V41.texpl;
			break ;
		  case nplus:  case nminus:  case nmul:
			tf = typeof(tq->U.V41.texpl);
			if (tf == typnods.A[(int)(tinteger)])
				tf = typeof(tq->U.V41.texpr);
			else
				if (tf->tt == nsetof)
					tf = typnods.A[(int)(tset)];
			break ;
		  case numinus:  case nuplus:
			tq = tq->U.V42.texps;
			break ;
		  case nmod:  case ndiv:
			tf = typnods.A[(int)(tinteger)];
			break ;
		  case nquot:
			tf = typnods.A[(int)(treal)];
			break ;
		  case neq:  case nne:  case nlt:  case nle:
		  case ngt:  case nge:  case nin:  case nor:
		  case nand:  case nnot:
			tf = typnods.A[(int)(tboolean)];
			break ;
		  case nset:
			tf = typnods.A[(int)(tset)];
			break ;
		  case nselect:
			tq = tq->U.V40.tfield;
			break ;
		  case nderef:
			tq = typeof(tq->U.V42.texps);
			switch (tq->tt) {
			  case nptr:
				tq = tq->U.V16.tptrid;
				break ;
			  case nfileof:
				tq = tq->U.V18.tof;
				break ;
			  case npredef:
				tf = typnods.A[(int)(tchar)];
				break ;
			  default:
				Caseerror(Line);
			}
			break ;
		  case nindex:
			tq = typeof(tq->U.V39.tvariable);
			if (tq->tt == nconfarr)
				tq = tq->U.V22.tcelem;
			else
				if (tq == typnods.A[(int)(tstring)])
					tf = typnods.A[(int)(tchar)];
				else
					tq = tq->U.V23.taelem;
			break ;
		  default:
			Caseerror(Line);
		}
	}
	if (tp->ttype == (struct S61 *)NIL)
		tp->ttype = tf;
	R92 = tf;
	return R92;
}

 void
linkup(up, tp)
	treeptr	up, tp;
{
	while (tp != (struct S61 *)NIL) {
		if (tp->tup == (struct S61 *)NIL) {
			tp->tup = up;
			switch (tp->tt) {
			  case npgm:  case nfunc:  case nproc:
				linkup(tp, tp->U.V13.tsubid);
				linkup(tp, tp->U.V13.tsubpar);
				linkup(tp, tp->U.V13.tfuntyp);
				linkup(tp, tp->U.V13.tsublab);
				linkup(tp, tp->U.V13.tsubconst);
				linkup(tp, tp->U.V13.tsubtype);
				linkup(tp, tp->U.V13.tsubvar);
				linkup(tp, tp->U.V13.tsubsub);
				linkup(tp, tp->U.V13.tsubstmt);
				break ;
			  case nvalpar:  case nvarpar:  case nconst:  case ntype:
			  case nfield:  case nvar:
				linkup(tp, tp->U.V14.tidl);
				linkup(tp, tp->U.V14.tbind);
				break ;
			  case nparproc:  case nparfunc:
				linkup(tp, tp->U.V15.tparid);
				linkup(tp, tp->U.V15.tparparm);
				linkup(tp, tp->U.V15.tpartyp);
				break ;
			  case nptr:
				linkup(tp, tp->U.V16.tptrid);
				break ;
			  case nscalar:
				linkup(tp, tp->U.V17.tscalid);
				break ;
			  case nsubrange:
				linkup(tp, tp->U.V19.tlo);
				linkup(tp, tp->U.V19.thi);
				break ;
			  case nvariant:
				linkup(tp, tp->U.V20.tselct);
				linkup(tp, tp->U.V20.tvrnt);
				break ;
			  case nrecord:
				linkup(tp, tp->U.V21.tflist);
				linkup(tp, tp->U.V21.tvlist);
				break ;
			  case nconfarr:
				linkup(tp, tp->U.V22.tcindx);
				linkup(tp, tp->U.V22.tcelem);
				linkup(tp, tp->U.V22.tindtyp);
				break ;
			  case narray:
				linkup(tp, tp->U.V23.taindx);
				linkup(tp, tp->U.V23.taelem);
				break ;
			  case nfileof:  case nsetof:
				linkup(tp, tp->U.V18.tof);
				break ;
			  case nbegin:
				linkup(tp, tp->U.V24.tbegin);
				break ;
			  case nlabstmt:
				linkup(tp, tp->U.V25.tlabno);
				linkup(tp, tp->U.V25.tstmt);
				break ;
			  case nassign:
				linkup(tp, tp->U.V27.tlhs);
				linkup(tp, tp->U.V27.trhs);
				break ;
			  case npush:  case npop:
				linkup(tp, tp->U.V28.tglob);
				linkup(tp, tp->U.V28.tloc);
				linkup(tp, tp->U.V28.ttmp);
				break ;
			  case ncall:
				linkup(tp, tp->U.V30.tcall);
				linkup(tp, tp->U.V30.taparm);
				break ;
			  case nif:
				linkup(tp, tp->U.V31.tifxp);
				linkup(tp, tp->U.V31.tthen);
				linkup(tp, tp->U.V31.telse);
				break ;
			  case nwhile:
				linkup(tp, tp->U.V32.twhixp);
				linkup(tp, tp->U.V32.twhistmt);
				break ;
			  case nrepeat:
				linkup(tp, tp->U.V33.treptstmt);
				linkup(tp, tp->U.V33.treptxp);
				break ;
			  case nfor:
				linkup(tp, tp->U.V34.tforid);
				linkup(tp, tp->U.V34.tfrom);
				linkup(tp, tp->U.V34.tto);
				linkup(tp, tp->U.V34.tforstmt);
				break ;
			  case ncase:
				linkup(tp, tp->U.V35.tcasxp);
				linkup(tp, tp->U.V35.tcaslst);
				linkup(tp, tp->U.V35.tcasother);
				break ;
			  case nchoise:
				linkup(tp, tp->U.V36.tchocon);
				linkup(tp, tp->U.V36.tchostmt);
				break ;
			  case nwith:
				linkup(tp, tp->U.V37.twithvar);
				linkup(tp, tp->U.V37.twithstmt);
				break ;
			  case nwithvar:
				linkup(tp, tp->U.V38.texpw);
				break ;
			  case nindex:
				linkup(tp, tp->U.V39.tvariable);
				linkup(tp, tp->U.V39.toffset);
				break ;
			  case nselect:
				linkup(tp, tp->U.V40.trecord);
				linkup(tp, tp->U.V40.tfield);
				break ;
			  case ngoto:
				linkup(tp, tp->U.V26.tlabel);
				break ;
			  case nrange:  case nformat:  case nin:  case neq:
			  case nne:  case nlt:  case nle:  case ngt:
			  case nge:  case nor:  case nplus:  case nminus:
			  case nand:  case nmul:  case ndiv:  case nmod:
			  case nquot:
				linkup(tp, tp->U.V41.texpl);
				linkup(tp, tp->U.V41.texpr);
				break ;
			  case nderef:  case nnot:  case nset:  case numinus:
			  case nuplus:
				linkup(tp, tp->U.V42.texps);
				break ;
			  case nid:  case nnil:  case ninteger:  case nreal:
			  case nchar:  case nstring:  case npredef:  case nlabel:
			  case nempty:
				break ;
			  default:
				Caseerror(Line);
			}
		}
		tp = tp->tnext;
	}
}

 symptr
mksym(vt)
	ltypes	vt;
{
	register symptr	R93;
	symptr	mp;

	mp = (struct S62 *)malloc((unsigned)(sizeof(*mp)));
	if (mp == (struct S62 *)NIL)
		error(enew);
	mp->lt = vt;
	mp->lnext = (struct S62 *)NIL;
	mp->lsymdecl = (struct S61 *)NIL;
	mp->ldecl = (struct S60 *)NIL;
	R93 = mp;
	return R93;
}

 void
declsym(sp)
	symptr	sp;
{
	hashtyp	h;

	if (Member((unsigned)(sp->lt), Conset[1]))
		h = sp->U.V6.lid->ihash;
	else
		h = hashmax;
	sp->lnext = symtab->ddecl.A[h];
	symtab->ddecl.A[h] = sp;
	sp->ldecl = symtab;
}

 treeptr
mknode(nt)
	treetyp	nt;
{
	register treeptr	R94;
	treeptr	tp;

	tp = (struct S61 *)NIL;
	switch (nt) {
	  case npredef:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V12.tdef) + sizeof(tp->U.V12)));
		break ;
	  case npgm:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V13.tsubid) + sizeof(tp->U.V13)));
		break ;
	  case nfunc:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V13.tsubid) + sizeof(tp->U.V13)));
		break ;
	  case nproc:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V13.tsubid) + sizeof(tp->U.V13)));
		break ;
	  case nlabel:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V43.tsym) + sizeof(tp->U.V43)));
		break ;
	  case nconst:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V14.tidl) + sizeof(tp->U.V14)));
		break ;
	  case ntype:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V14.tidl) + sizeof(tp->U.V14)));
		break ;
	  case nvar:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V14.tidl) + sizeof(tp->U.V14)));
		break ;
	  case nvalpar:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V14.tidl) + sizeof(tp->U.V14)));
		break ;
	  case nvarpar:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V14.tidl) + sizeof(tp->U.V14)));
		break ;
	  case nparproc:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V15.tparid) + sizeof(tp->U.V15)));
		break ;
	  case nparfunc:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V15.tparid) + sizeof(tp->U.V15)));
		break ;
	  case nsubrange:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V19.tlo) + sizeof(tp->U.V19)));
		break ;
	  case nvariant:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V20.tselct) + sizeof(tp->U.V20)));
		break ;
	  case nfield:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V14.tidl) + sizeof(tp->U.V14)));
		break ;
	  case nrecord:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V21.tflist) + sizeof(tp->U.V21)));
		break ;
	  case nconfarr:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V22.tcindx) + sizeof(tp->U.V22)));
		break ;
	  case narray:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V23.taindx) + sizeof(tp->U.V23)));
		break ;
	  case nfileof:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V18.tof) + sizeof(tp->U.V18)));
		break ;
	  case nsetof:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V18.tof) + sizeof(tp->U.V18)));
		break ;
	  case nbegin:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V24.tbegin) + sizeof(tp->U.V24)));
		break ;
	  case nptr:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V16.tptrid) + sizeof(tp->U.V16)));
		break ;
	  case nscalar:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V17.tscalid) + sizeof(tp->U.V17)));
		break ;
	  case nif:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V31.tifxp) + sizeof(tp->U.V31)));
		break ;
	  case nwhile:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V32.twhixp) + sizeof(tp->U.V32)));
		break ;
	  case nrepeat:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V33.treptstmt) + sizeof(tp->U.V33)));
		break ;
	  case nfor:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V34.tforid) + sizeof(tp->U.V34)));
		break ;
	  case ncase:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V35.tcasxp) + sizeof(tp->U.V35)));
		break ;
	  case nchoise:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V36.tchocon) + sizeof(tp->U.V36)));
		break ;
	  case ngoto:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V26.tlabel) + sizeof(tp->U.V26)));
		break ;
	  case nwith:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V37.twithvar) + sizeof(tp->U.V37)));
		break ;
	  case nwithvar:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V38.texpw) + sizeof(tp->U.V38)));
		break ;
	  case nempty:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V12.tdef)));
		break ;
	  case nlabstmt:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V25.tlabno) + sizeof(tp->U.V25)));
		break ;
	  case nassign:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V27.tlhs) + sizeof(tp->U.V27)));
		break ;
	  case nformat:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nin:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case neq:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nne:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nlt:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nle:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case ngt:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nge:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nor:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nplus:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nminus:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nand:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nmul:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case ndiv:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nmod:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nquot:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nnot:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V42.texps) + sizeof(tp->U.V42)));
		break ;
	  case numinus:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V42.texps) + sizeof(tp->U.V42)));
		break ;
	  case nuplus:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V42.texps) + sizeof(tp->U.V42)));
		break ;
	  case nset:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V42.texps) + sizeof(tp->U.V42)));
		break ;
	  case nrange:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V41.texpl) + sizeof(tp->U.V41)));
		break ;
	  case nindex:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V39.tvariable) + sizeof(tp->U.V39)));
		break ;
	  case nselect:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V40.trecord) + sizeof(tp->U.V40)));
		break ;
	  case nderef:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V42.texps) + sizeof(tp->U.V42)));
		break ;
	  case ncall:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V30.tcall) + sizeof(tp->U.V30)));
		break ;
	  case nid:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V43.tsym) + sizeof(tp->U.V43)));
		break ;
	  case nchar:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V43.tsym) + sizeof(tp->U.V43)));
		break ;
	  case ninteger:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V43.tsym) + sizeof(tp->U.V43)));
		break ;
	  case nreal:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V43.tsym) + sizeof(tp->U.V43)));
		break ;
	  case nstring:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V43.tsym) + sizeof(tp->U.V43)));
		break ;
	  case nnil:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V12.tdef)));
		break ;
	  case npush:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V28.tglob) + sizeof(tp->U.V28)));
		break ;
	  case npop:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V28.tglob) + sizeof(tp->U.V28)));
		break ;
	  case nbreak:
		tp = (struct S61 *)malloc((unsigned)(Unionoffs(tp, U.V29.tbrkid) + sizeof(tp->U.V29)));
		break ;
	  default:
		Caseerror(Line);
	}
	if (tp == (struct S61 *)NIL)
		error(enew);
	tp->tt = nt;
	tp->tnext = (struct S61 *)NIL;
	tp->tup = (struct S61 *)NIL;
	tp->ttype = (struct S61 *)NIL;
	R94 = tp;
	return R94;
}

 treeptr
mklit()
{
	register treeptr	R95;
	symptr	sp;
	treeptr	tp;

	switch (currsym.st) {
	  case sinteger:
		sp = mksym(linteger);
		sp->U.V10.linum = currsym.U.V3.vint;
		tp = mknode(ninteger);
		break ;
	  case sreal:
		sp = mksym(lreal);
		sp->U.V8.lfloat = currsym.U.V4.vflt;
		tp = mknode(nreal);
		break ;
	  case schar:
		sp = mksym(lcharacter);
		sp->U.V11.lchar = currsym.U.V2.vchr;
		tp = mknode(nchar);
		break ;
	  case sstring:
		sp = mksym(lstring);
		sp->U.V7.lstr = currsym.U.V5.vstr;
		tp = mknode(nstring);
		break ;
	  default:
		Caseerror(Line);
	}
	tp->U.V43.tsym = sp;
	sp->lsymdecl = tp;
	R95 = tp;
	return R95;
}

 symptr
lookupid(ip, fieldok)
	idptr	ip;
	boolean	fieldok;
{
	register symptr	R96;
	symptr	sp;
	declptr	dp;
	struct { setword	S[2]; }	vs;

	R96 = (struct S62 *)NIL;
	if (fieldok)
		Setncpy(vs.S, Conset[2], sizeof(vs.S));
	else
		Setncpy(vs.S, Conset[3], sizeof(vs.S));
	sp = (struct S62 *)NIL;
	dp = symtab;
	while (dp != (struct S60 *)NIL) {
		sp = dp->ddecl.A[ip->ihash];
		while (sp != (struct S62 *)NIL) {
			if ((Member((unsigned)(sp->lt), vs.S)) && (sp->U.V6.lid == ip))
				goto L999;
			sp = sp->lnext;
		}
		dp = dp->dprev;
	}
L999:
	R96 = sp;
	return R96;
}

 symptr
lookuplabel(i)
	integer	i;
{
	register symptr	R97;
	symptr	sp;
	declptr	dp;

	sp = (struct S62 *)NIL;
	dp = symtab;
	while (dp != (struct S60 *)NIL) {
		sp = dp->ddecl.A[hashmax];
		while (sp != (struct S62 *)NIL) {
			if ((Member((unsigned)(sp->lt), Conset[4])) && (sp->U.V9.lno == i))
				goto L999;
			sp = sp->lnext;
		}
		dp = dp->dprev;
	}
L999:
	R97 = sp;
	return R97;
}

 void
enterscope(dp)
	declptr	dp;
{
	register hashtyp	h;

	if (dp == (struct S60 *)NIL) {
		dp = (struct S60 *)malloc((unsigned)(sizeof(*dp)));
		{
			hashtyp	B47 = 0,
				B48 = hashmax;

			if (B47 <= B48)
				for (h = B47; ; h++) {
					dp->ddecl.A[h] = (struct S62 *)NIL;
					if (h == B48) break;
				}
		}
	}
	dp->dprev = symtab;
	symtab = dp;
}

 declptr
currscope()
{
	register declptr	R98;

	R98 = symtab;
	return R98;
}

 void
leavescope()
{
	symtab = symtab->dprev;
}

 symptr
mkid(ip)
	idptr	ip;
{
	register symptr	R99;
	symptr	sp;

	sp = mksym(lidentifier);
	sp->U.V6.lid = ip;
	sp->U.V6.lused = false;
	declsym(sp);
	ip->inref = ip->inref + 1;
	R99 = sp;
	return R99;
}

 treeptr
newid(ip)
	idptr	ip;
{
	register treeptr	R100;
	symptr	sp;
	treeptr	tp;

	sp = lookupid(ip, false);
	if (sp != (struct S62 *)NIL)
		if (sp->ldecl != symtab)
			sp = (struct S62 *)NIL;
	if (sp == (struct S62 *)NIL) {
		tp = mknode(nid);
		sp = mkid(ip);
		sp->lsymdecl = tp;
		tp->U.V43.tsym = sp;
	} else
		if (sp->lt == lpointer) {
			tp = mknode(nid);
			tp->U.V43.tsym = sp;
			sp->lt = lidentifier;
			sp->lsymdecl = tp;
		} else
			if (sp->lt == lforward) {
				sp->lt = lidentifier;
				tp = sp->lsymdecl;
			} else
				error(emultdeclid);
	R100 = tp;
	return R100;
}

 treeptr
oldid(ip, l)
	idptr	ip;
	ltypes	l;
{
	register treeptr	R101;
	symptr	sp;
	treeptr	tp;

	sp = lookupid(ip, true);
	if (sp == (struct S62 *)NIL) {
		if (Member((unsigned)(l), Conset[5])) {
			tp = newid(ip);
			tp->U.V43.tsym->lt = l;
		} else
			error(enotdeclid);
	} else {
		sp->U.V6.lused = true;
		tp = mknode(nid);
		tp->U.V43.tsym = sp;
		if ((sp->lt == lpointer) && (l == lidentifier)) {
			sp->lt = lidentifier;
			sp->lsymdecl = tp;
		}
	}
	R101 = tp;
	return R101;
}

 treeptr
oldfield(tp, ip)
	treeptr	tp;
	idptr	ip;
{
	register treeptr	R102;
	treeptr	tq, ti, fp;

	fp = (struct S61 *)NIL;
	tq = tp->U.V21.tflist;
	while (tq != (struct S61 *)NIL) {
		ti = tq->U.V14.tidl;
		while (ti != (struct S61 *)NIL) {
			if (ti->U.V43.tsym->U.V6.lid == ip) {
				fp = mknode(nid);
				fp->U.V43.tsym = ti->U.V43.tsym;
				goto L999;
			}
			ti = ti->tnext;
		}
		tq = tq->tnext;
	}
	tq = tp->U.V21.tvlist;
	while (tq != (struct S61 *)NIL) {
		fp = oldfield(tq->U.V20.tvrnt, ip);
		if (fp != (struct S61 *)NIL)
			tq = (struct S61 *)NIL;
		else
			tq = tq->tnext;
	}
L999:
	R102 = fp;
	return R102;
}

void parse();

treeptr plabel();

treeptr pidlist();


treeptr pconst();

treeptr pconstant();

treeptr precord();

treeptr ptypedef();

treeptr ptype();

treeptr pvar();

treeptr psubs();

treeptr psubpar();

treeptr plabstmt();

treeptr pstmt();

treeptr psimple();

treeptr pvariable();

treeptr pexpr();

treeptr pcase();

treeptr pif();

treeptr pwhile();

treeptr prepeat();

treeptr pfor();

treeptr pwith();

treeptr pgoto();

treeptr pbegin();

void scopeup();

 void
addfields(rp)
	treeptr	rp;
{
	treeptr	fp, ip, vp;
	symptr	sp;

	fp = rp->U.V21.tflist;
	while (fp != (struct S61 *)NIL) {
		ip = fp->U.V14.tidl;
		while (ip != (struct S61 *)NIL) {
			sp = mksym(lfield);
			sp->U.V6.lid = ip->U.V43.tsym->U.V6.lid;
			sp->U.V6.lused = false;
			sp->lsymdecl = ip;
			declsym(sp);
			ip = ip->tnext;
		}
		fp = fp->tnext;
	}
	vp = rp->U.V21.tvlist;
	while (vp != (struct S61 *)NIL) {
		addfields(vp->U.V20.tvrnt);
		vp = vp->tnext;
	}
}

 void
scopeup(tp)
	treeptr	tp;
{
	addfields(typeof(tp));
}

 treeptr
newlbl()
{
	register treeptr	R126;
	symptr	sp;
	treeptr	tp;

	tp = mknode(nlabel);
	sp = lookuplabel(currsym.U.V3.vint);
	if (sp != (struct S62 *)NIL)
		if (sp->ldecl != symtab)
			sp = (struct S62 *)NIL;
	if (sp == (struct S62 *)NIL) {
		sp = mksym(lforwlab);
		sp->U.V9.lno = currsym.U.V3.vint;
		sp->U.V9.lgo = false;
		sp->lsymdecl = tp;
		declsym(sp);
	} else
		error(emultdecllab);
	tp->U.V43.tsym = sp;
	R126 = tp;
	return R126;
}

 treeptr
oldlbl(defpt)
	boolean	defpt;
{
	register treeptr	R127;
	symptr	sp;
	treeptr	tp;

	sp = lookuplabel(currsym.U.V3.vint);
	if (sp == (struct S62 *)NIL) {
		prtmsg(enotdecllab);
		tp = newlbl();
		sp = tp->U.V43.tsym;
	} else {
		tp = mknode(nlabel);
		tp->U.V43.tsym = sp;
	}
	if (defpt) {
		if (sp->lt == lforwlab)
			sp->lt = llabel;
		else
			error(emuldeflab);
	}
	R127 = tp;
	return R127;
}

 void
pbody(tp)
	treeptr	tp;
{
	treeptr	tq;

	statlvl = statlvl + 1;
	if (currsym.st == slabel) {
		tp->U.V13.tsublab = plabel();
		linkup(tp, tp->U.V13.tsublab);
	} else
		tp->U.V13.tsublab = (struct S61 *)NIL;
	if (currsym.st == sconst) {
		tp->U.V13.tsubconst = pconst();
		linkup(tp, tp->U.V13.tsubconst);
	} else
		tp->U.V13.tsubconst = (struct S61 *)NIL;
	if (currsym.st == stype) {
		tp->U.V13.tsubtype = ptype();
		linkup(tp, tp->U.V13.tsubtype);
	} else
		tp->U.V13.tsubtype = (struct S61 *)NIL;
	if (currsym.st == svar) {
		tp->U.V13.tsubvar = pvar();
		linkup(tp, tp->U.V13.tsubvar);
	} else
		tp->U.V13.tsubvar = (struct S61 *)NIL;
	tp->U.V13.tsubsub = (struct S61 *)NIL;
	tq = (struct S61 *)NIL;
	while ((currsym.st == sproc) || (currsym.st == sfunc)) {
		if (tq == (struct S61 *)NIL) {
			tq = psubs();
			tp->U.V13.tsubsub = tq;
		} else {
			tq->tnext = psubs();
			tq = tq->tnext;
		}
	}
	linkup(tp, tp->U.V13.tsubsub);
	checksymbol(*((symset *)Conset[6]));
	if (currsym.st == sbegin) {
		tp->U.V13.tsubstmt = pbegin(false);
		linkup(tp, tp->U.V13.tsubstmt);
	}
	statlvl = statlvl - 1;
}

treeptr pprogram();

 treeptr
pprmlist()
{
	register treeptr	R129;
	treeptr	tp, tq;
	idptr	din, dut;

	tp = (struct S61 *)NIL;
	din = deftab.A[(int)(dinput)]->U.V14.tidl->U.V43.tsym->U.V6.lid;
	dut = deftab.A[(int)(doutput)]->U.V14.tidl->U.V43.tsym->U.V6.lid;
	while ((currsym.U.V1.vid == din) || (currsym.U.V1.vid == dut)) {
		if (currsym.U.V1.vid == din)
			defnams.A[(int)(dinput)]->U.V6.lused = true;
		else
			defnams.A[(int)(doutput)]->U.V6.lused = true;
		nextsymbol(*((symset *)Conset[7]));
		if (currsym.st == srpar)
			goto L999;
		nextsymbol(*((symset *)Conset[8]));
	}
	tq = newid(currsym.U.V1.vid);
	tq->U.V43.tsym->lt = lpointer;
	tp = tq;
	nextsymbol(*((symset *)Conset[9]));
	while (currsym.st == scomma) {
		nextsymbol(*((symset *)Conset[10]));
		if (currsym.U.V1.vid == din)
			defnams.A[(int)(dinput)]->U.V6.lused = true;
		else
			if (currsym.U.V1.vid == dut)
				defnams.A[(int)(doutput)]->U.V6.lused = true;
			else {
				tq->tnext = newid(currsym.U.V1.vid);
				tq = tq->tnext;
				tq->U.V43.tsym->lt = lpointer;
			}
		nextsymbol(*((symset *)Conset[11]));
	}
L999:
	R129 = tp;
	return R129;
}

 treeptr
pprogram()
{
	register treeptr	R128;
	treeptr	tp;

	enterscope((declptr)NIL);
	tp = mknode(npgm);
	nextsymbol(*((symset *)Conset[12]));
	tp->U.V13.tstat = statlvl;
	tp->U.V13.tsubid = mknode(nid);
	tp->U.V13.tsubid->tup = tp;
	tp->U.V13.tsubid->U.V43.tsym = mksym(lidentifier);
	tp->U.V13.tsubid->U.V43.tsym->U.V6.lid = currsym.U.V1.vid;
	tp->U.V13.tsubid->U.V43.tsym->lsymdecl = tp->U.V13.tsubid;
	linkup(tp, tp->U.V13.tsubid);
	nextsymbol(*((symset *)Conset[13]));
	if (currsym.st == slpar) {
		nextsymbol(*((symset *)Conset[14]));
		tp->U.V13.tsubpar = pprmlist();
		linkup(tp, tp->U.V13.tsubpar);
		nextsymbol(*((symset *)Conset[15]));
	} else
		tp->U.V13.tsubpar = (struct S61 *)NIL;
	nextsymbol(*((symset *)Conset[16]));
	pbody(tp);
	checksymbol(*((symset *)Conset[17]));
	tp->U.V13.tscope = currscope();
	leavescope();
	R128 = tp;
	return R128;
}

 treeptr
pmodule()
{
	register treeptr	R130;
	treeptr	tp;

	enterscope((declptr)NIL);
	tp = mknode(npgm);
	tp->U.V13.tstat = statlvl;
	tp->U.V13.tsubid = (struct S61 *)NIL;
	tp->U.V13.tsubpar = (struct S61 *)NIL;
	pbody(tp);
	checksymbol(*((symset *)Conset[18]));
	tp->U.V13.tscope = currscope();
	leavescope();
	R130 = tp;
	return R130;
}

 treeptr
plabel()
{
	register treeptr	R131;
	treeptr	tp, tq;

	tq = (struct S61 *)NIL;
	do {
		nextsymbol(*((symset *)Conset[19]));
		if (tq == (struct S61 *)NIL) {
			tq = newlbl();
			tp = tq;
		} else {
			tq->tnext = newlbl();
			tq = tq->tnext;
		}
		nextsymbol(*((symset *)Conset[20]));
	} while (!(currsym.st == ssemic));
	nextsymbol(*((symset *)Conset[21]));
	R131 = tp;
	return R131;
}

 treeptr
pidlist(l)
	ltypes	l;
{
	register treeptr	R132;
	treeptr	tp, tq;

	tq = newid(currsym.U.V1.vid);
	tq->U.V43.tsym->lt = l;
	tp = tq;
	nextsymbol(*((symset *)Conset[22]));
	while (currsym.st == scomma) {
		nextsymbol(*((symset *)Conset[23]));
		tq->tnext = newid(currsym.U.V1.vid);
		tq = tq->tnext;
		tq->U.V43.tsym->lt = l;
		nextsymbol(*((symset *)Conset[24]));
	}
	R132 = tp;
	return R132;
}

 treeptr
pconst()
{
	register treeptr	R133;
	treeptr	tp, tq;

	tq = (struct S61 *)NIL;
	nextsymbol(*((symset *)Conset[25]));
	do {
		if (tq == (struct S61 *)NIL) {
			tq = mknode(nconst);
			tq->U.V14.tattr = anone;
			tp = tq;
		} else {
			tq->tnext = mknode(nconst);
			tq = tq->tnext;
			tq->U.V14.tattr = anone;
		}
		tq->U.V14.tidl = pidlist(lidentifier);
		checksymbol(*((symset *)Conset[26]));
		nextsymbol(*((symset *)Conset[27]));
		tq->U.V14.tbind = pconstant(true);
		nextsymbol(*((symset *)Conset[28]));
		nextsymbol(*((symset *)Conset[29]));
	} while (!(currsym.st != sid));
	R133 = tp;
	return R133;
}

 treeptr
pconstant(realok)
	boolean	realok;
{
	register treeptr	R134;
	treeptr	tp, tq;
	boolean	neg;

	neg = (boolean)(currsym.st == sminus);
	if (Member((unsigned)(currsym.st), Conset[30]))
		if (realok)
			nextsymbol(*((symset *)Conset[31]));
		else
			nextsymbol(*((symset *)Conset[32]));
	if (currsym.st == sid)
		tp = oldid(currsym.U.V1.vid, lidentifier);
	else
		tp = mklit();
	if (neg) {
		tq = mknode(numinus);
		tq->U.V42.texps = tp;
		tp = tq;
	}
	R134 = tp;
	return R134;
}

 treeptr
precord(cs, dp)
	symtyp	cs;
	declptr	dp;
{
	register treeptr	R135;
	treeptr	tp, tq, tl, tv;
	lexsym	tsym;

	tp = mknode(nrecord);
	tp->U.V21.tflist = (struct S61 *)NIL;
	tp->U.V21.tvlist = (struct S61 *)NIL;
	tp->U.V21.tuid = (struct S59 *)NIL;
	tp->U.V21.trscope = (struct S60 *)NIL;
	if (cs == send) {
		enterscope(dp);
		dp = currscope();
	}
	nextsymbol(*((symset *)Union(Conset[33], Saveset((Tmpset = Newset(), (void)Insmem((unsigned)(cs), Tmpset), Tmpset)))));
	Claimset();
	tq = (struct S61 *)NIL;
	while (currsym.st == sid) {
		if (tq == (struct S61 *)NIL) {
			tq = mknode(nfield);
			tq->U.V14.tattr = anone;
			tp->U.V21.tflist = tq;
		} else {
			tq->tnext = mknode(nfield);
			tq = tq->tnext;
			tq->U.V14.tattr = anone;
		}
		tq->U.V14.tidl = pidlist(lfield);
		checksymbol(*((symset *)Conset[34]));
		leavescope();
		tq->U.V14.tbind = ptypedef();
		enterscope(dp);
		if (currsym.st == ssemic)
			nextsymbol(*((symset *)Union(Conset[35], Saveset((Tmpset = Newset(), (void)Insmem((unsigned)(cs), Tmpset), Tmpset)))));
		Claimset();
	}
	if (currsym.st == scase) {
		nextsymbol(*((symset *)Conset[36]));
		tsym = currsym;
		nextsymbol(*((symset *)Conset[37]));
		if (currsym.st == scolon) {
			tv = newid(tsym.U.V1.vid);
			if (tq == (struct S61 *)NIL) {
				tq = mknode(nfield);
				tp->U.V21.tflist = tq;
			} else {
				tq->tnext = mknode(nfield);
				tq = tq->tnext;
			}
			tq->U.V14.tidl = tv;
			tv->U.V43.tsym->lt = lfield;
			nextsymbol(*((symset *)Conset[38]));
			leavescope();
			tq->U.V14.tbind = oldid(currsym.U.V1.vid, lidentifier);
			enterscope(dp);
			nextsymbol(*((symset *)Conset[39]));
		}
		tq = (struct S61 *)NIL;
		do {
			tv = (struct S61 *)NIL;
			do {
				nextsymbol(*((symset *)Union(Conset[40], Saveset((Tmpset = Newset(), (void)Insmem((unsigned)(cs), Tmpset), Tmpset)))));
				Claimset();
				if (currsym.st == cs)
					goto L999;
				if (tv == (struct S61 *)NIL) {
					tv = pconstant(false);
					tl = tv;
				} else {
					tv->tnext = pconstant(false);
					tv = tv->tnext;
				}
				nextsymbol(*((symset *)Conset[41]));
			} while (!(currsym.st == scolon));
			nextsymbol(*((symset *)Conset[42]));
			if (tq == (struct S61 *)NIL) {
				tq = mknode(nvariant);
				tp->U.V21.tvlist = tq;
			} else {
				tq->tnext = mknode(nvariant);
				tq = tq->tnext;
			}
			tq->U.V20.tselct = tl;
			tq->U.V20.tvrnt = precord(srpar, dp);
		} while (!(currsym.st == cs));
	}
L999:
	if (cs == send) {
		tp->U.V21.trscope = dp;
		leavescope();
	}
	nextsymbol(*((symset *)Conset[43]));
	R135 = tp;
	return R135;
}

 treeptr
ptypedef()
{
	register treeptr	R136;
	treeptr	tp, tq;
	symtyp	st;
	symset	ss;

	nextsymbol(*((symset *)Conset[44]));
	if (currsym.st == spacked)
		nextsymbol(*((symset *)Conset[45]));
	Setncpy(ss.S, Conset[46], sizeof(ss.S));
	switch (currsym.st) {
	  case splus:  case sminus:  case schar:  case sinteger:
	  case sid:
		st = currsym.st;
		tp = pconstant(false);
		if (st == sid)
			nextsymbol(*((symset *)Union(Conset[47], ss.S)));
		else
			nextsymbol(*((symset *)Conset[48]));
		Claimset();
		if (currsym.st == sdotdot) {
			nextsymbol(*((symset *)Conset[49]));
			tq = mknode(nsubrange);
			tq->U.V19.tlo = tp;
			tq->U.V19.thi = pconstant(false);
			tp = tq;
			nextsymbol(ss);
		}
		break ;
	  case slpar:
		tp = mknode(nscalar);
		nextsymbol(*((symset *)Conset[50]));
		tp->U.V17.tscalid = pidlist(lidentifier);
		checksymbol(*((symset *)Conset[51]));
		nextsymbol(ss);
		break ;
	  case sarrow:
		tp = mknode(nptr);
		nextsymbol(*((symset *)Conset[52]));
		tp->U.V16.tptrid = oldid(currsym.U.V1.vid, lpointer);
		tp->U.V16.tptrflag = false;
		nextsymbol(*((symset *)Conset[53]));
		break ;
	  case sarray:
		nextsymbol(*((symset *)Conset[54]));
		tp = mknode(narray);
		tp->U.V23.taindx = ptypedef();
		tq = tp;
		while (currsym.st == scomma) {
			tq->U.V23.taelem = mknode(narray);
			tq = tq->U.V23.taelem;
			tq->U.V23.taindx = ptypedef();
		}
		checksymbol(*((symset *)Conset[55]));
		nextsymbol(*((symset *)Conset[56]));
		tq->U.V23.taelem = ptypedef();
		break ;
	  case srecord:
		tp = precord(send, (declptr)NIL);
		break ;
	  case sfile:  case sset:
		if (currsym.st == sfile)
			tp = mknode(nfileof);
		else {
			tp = mknode(nsetof);
			usesets = true;
		}
		nextsymbol(*((symset *)Conset[57]));
		tp->U.V18.tof = ptypedef();
		break ;
	  default:
		Caseerror(Line);
	}
	R136 = tp;
	return R136;
}

 treeptr
ptype()
{
	register treeptr	R137;
	treeptr	tp, tq;

	tq = (struct S61 *)NIL;
	nextsymbol(*((symset *)Conset[58]));
	do {
		if (tq == (struct S61 *)NIL) {
			tq = mknode(ntype);
			tq->U.V14.tattr = anone;
			tp = tq;
		} else {
			tq->tnext = mknode(ntype);
			tq = tq->tnext;
			tq->U.V14.tattr = anone;
		}
		tq->U.V14.tidl = pidlist(lidentifier);
		checksymbol(*((symset *)Conset[59]));
		tq->U.V14.tbind = ptypedef();
		nextsymbol(*((symset *)Conset[60]));
	} while (!(currsym.st != sid));
	R137 = tp;
	return R137;
}

 treeptr
pvar()
{
	register treeptr	R138;
	treeptr	ti, tp, tq;

	tq = (struct S61 *)NIL;
	nextsymbol(*((symset *)Conset[61]));
	do {
		if (tq == (struct S61 *)NIL) {
			tq = mknode(nvar);
			tq->U.V14.tattr = anone;
			tp = tq;
		} else {
			tq->tnext = mknode(nvar);
			tq = tq->tnext;
			tq->U.V14.tattr = anone;
		}
		ti = newid(currsym.U.V1.vid);
		tq->U.V14.tidl = ti;
		nextsymbol(*((symset *)Conset[62]));
		while (currsym.st == scomma) {
			nextsymbol(*((symset *)Conset[63]));
			ti->tnext = newid(currsym.U.V1.vid);
			ti = ti->tnext;
			nextsymbol(*((symset *)Conset[64]));
		}
		tq->U.V14.tbind = ptypedef();
		nextsymbol(*((symset *)Conset[65]));
	} while (!(currsym.st != sid));
	R138 = tp;
	return R138;
}

 treeptr
psubs()
{
	register treeptr	R139;
	treeptr	tp, tv, tq;
	boolean	func;
	symtyp	colsem;

	func = (boolean)(currsym.st == sfunc);
	if (func)
		colsem = scolon;
	else
		colsem = ssemic;
	nextsymbol(*((symset *)Conset[66]));
	tq = newid(currsym.U.V1.vid);
	if (tq->tup == (struct S61 *)NIL) {
		enterscope((declptr)NIL);
		if (func)
			tp = mknode(nfunc);
		else
			tp = mknode(nproc);
		tp->U.V13.tstat = statlvl;
		tp->U.V13.tsubid = tq;
		linkup(tp, tq);
		nextsymbol(*((symset *)(Tmpset = Newset(), (void)Insmem((unsigned)(slpar), Tmpset),
			(void)Insmem((unsigned)(colsem), Tmpset), Tmpset)));
		if (currsym.st == slpar) {
			tp->U.V13.tsubpar = psubpar();
			linkup(tp, tp->U.V13.tsubpar);
			nextsymbol(*((symset *)(Tmpset = Newset(), (void)Insmem((unsigned)(colsem), Tmpset), Tmpset)));
		} else
			tp->U.V13.tsubpar = (struct S61 *)NIL;
		if (func) {
			nextsymbol(*((symset *)Conset[67]));
			tp->U.V13.tfuntyp = oldid(currsym.U.V1.vid, lidentifier);
			nextsymbol(*((symset *)Conset[68]));
		} else
			tp->U.V13.tfuntyp = mknode(nempty);
		linkup(tp, tp->U.V13.tfuntyp);
		nextsymbol(*((symset *)Conset[69]));
	} else {
		enterscope(tq->tup->U.V13.tscope);
		if (func)
			tp = mknode(nfunc);
		else
			tp = mknode(nproc);
		tp->U.V13.tfuntyp = tq->tup->U.V13.tfuntyp;
		tv = tq->tup->U.V13.tsubpar;
		tp->U.V13.tsubpar = tv;
		while (tv != (struct S61 *)NIL) {
			tv->tup = tp;
			tv = tv->tnext;
		}
		tp->U.V13.tsubid = tq;
		tq->tup = tp;
		nextsymbol(*((symset *)Conset[70]));
		nextsymbol(*((symset *)Conset[71]));
	}
	if (Member((unsigned)(currsym.st), Conset[72])) {
		tp->U.V13.tsubid->U.V43.tsym->lt = lforward;
		nextsymbol(*((symset *)Conset[73]));
		tp->U.V13.tsublab = (struct S61 *)NIL;
		tp->U.V13.tsubconst = (struct S61 *)NIL;
		tp->U.V13.tsubtype = (struct S61 *)NIL;
		tp->U.V13.tsubvar = (struct S61 *)NIL;
		tp->U.V13.tsubsub = (struct S61 *)NIL;
		tp->U.V13.tsubstmt = (struct S61 *)NIL;
	} else
		pbody(tp);
	nextsymbol(*((symset *)Conset[74]));
	tp->U.V13.tscope = currscope();
	leavescope();
	R139 = tp;
	return R139;
}

 treeptr
pconfsub()
{
	register treeptr	R140;
	treeptr	tp;

	tp = mknode(nsubrange);
	nextsymbol(*((symset *)Conset[75]));
	tp->U.V19.tlo = newid(currsym.U.V1.vid);
	nextsymbol(*((symset *)Conset[76]));
	nextsymbol(*((symset *)Conset[77]));
	tp->U.V19.thi = newid(currsym.U.V1.vid);
	nextsymbol(*((symset *)Conset[78]));
	R140 = tp;
	return R140;
}

 treeptr
pconform()
{
	register treeptr	R141;
	treeptr	tp, tq;

	nextsymbol(*((symset *)Conset[79]));
	tp = mknode(nconfarr);
	tp->U.V22.tcuid = mkvariable('S');
	tp->U.V22.tcindx = pconfsub();
	nextsymbol(*((symset *)Conset[80]));
	tp->U.V22.tindtyp = oldid(currsym.U.V1.vid, lidentifier);
	nextsymbol(*((symset *)Conset[81]));
	tq = tp;
	while (currsym.st == ssemic) {
		error(econfconf);
		tq->U.V22.tcelem = mknode(nconfarr);
		tq = tq->U.V22.tcelem;
		tq->U.V22.tcindx = pconfsub();
		nextsymbol(*((symset *)Conset[82]));
		tq->U.V22.tindtyp = oldid(currsym.U.V1.vid, lidentifier);
		nextsymbol(*((symset *)Conset[83]));
	}
	nextsymbol(*((symset *)Conset[84]));
	nextsymbol(*((symset *)Conset[85]));
	switch (currsym.st) {
	  case sid:
		tq->U.V22.tcelem = oldid(currsym.U.V1.vid, lidentifier);
		break ;
	  case sarray:
		error(econfconf);
		tq->U.V22.tcelem = pconform();
		break ;
	  default:
		Caseerror(Line);
	}
	R141 = tp;
	return R141;
}

 treeptr
psubpar()
{
	register treeptr	R142;
	treeptr	tp, tq;
	treetyp	nt;

	tq = (struct S61 *)NIL;
	do {
		nextsymbol(*((symset *)Conset[86]));
		switch (currsym.st) {
		  case sid:
			nt = nvalpar;
			break ;
		  case svar:
			nt = nvarpar;
			break ;
		  case sfunc:
			nt = nparfunc;
			break ;
		  case sproc:
			nt = nparproc;
			break ;
		  default:
			Caseerror(Line);
		}
		if (nt != nvalpar)
			nextsymbol(*((symset *)Conset[87]));
		if (tq == (struct S61 *)NIL) {
			tq = mknode(nt);
			tp = tq;
		} else {
			tq->tnext = mknode(nt);
			tq = tq->tnext;
		}
		switch (nt) {
		  case nvarpar:  case nvalpar:
			tq->U.V14.tidl = pidlist(lidentifier);
			tq->U.V14.tattr = anone;
			checksymbol(*((symset *)Conset[88]));
			if (nt == nvalpar)
				nextsymbol(*((symset *)Conset[89]));
			else
				nextsymbol(*((symset *)Conset[90]));
			switch (currsym.st) {
			  case sid:
				tq->U.V14.tbind = oldid(currsym.U.V1.vid, lidentifier);
				break ;
			  case sarray:
				tq->U.V14.tbind = pconform();
				break ;
			  default:
				Caseerror(Line);
			}
			nextsymbol(*((symset *)Conset[91]));
			break ;
		  case nparproc:
			tq->U.V15.tparid = newid(currsym.U.V1.vid);
			nextsymbol(*((symset *)Conset[92]));
			if (currsym.st == slpar) {
				enterscope((declptr)NIL);
				tq->U.V15.tparparm = psubpar();
				nextsymbol(*((symset *)Conset[93]));
				leavescope();
			} else
				tq->U.V15.tparparm = (struct S61 *)NIL;
			tq->U.V15.tpartyp = (struct S61 *)NIL;
			break ;
		  case nparfunc:
			tq->U.V15.tparid = newid(currsym.U.V1.vid);
			nextsymbol(*((symset *)Conset[94]));
			if (currsym.st == slpar) {
				enterscope((declptr)NIL);
				tq->U.V15.tparparm = psubpar();
				nextsymbol(*((symset *)Conset[95]));
				leavescope();
			} else
				tq->U.V15.tparparm = (struct S61 *)NIL;
			nextsymbol(*((symset *)Conset[96]));
			tq->U.V15.tpartyp = oldid(currsym.U.V1.vid, lidentifier);
			nextsymbol(*((symset *)Conset[97]));
			break ;
		  default:
			Caseerror(Line);
		}
	} while (!(currsym.st == srpar));
	R142 = tp;
	return R142;
}

 treeptr
plabstmt()
{
	register treeptr	R143;
	treeptr	tp;

	nextsymbol(*((symset *)Conset[98]));
	if (currsym.st == sinteger) {
		tp = mknode(nlabstmt);
		tp->U.V25.tlabno = oldlbl(true);
		nextsymbol(*((symset *)Conset[99]));
		nextsymbol(*((symset *)Conset[100]));
		tp->U.V25.tstmt = pstmt();
	} else
		tp = pstmt();
	R143 = tp;
	return R143;
}

 treeptr
pstmt()
{
	register treeptr	R144;
	treeptr	tp;

	switch (currsym.st) {
	  case sid:
		tp = psimple();
		break ;
	  case sif:
		tp = pif();
		break ;
	  case swhile:
		tp = pwhile();
		break ;
	  case srepeat:
		tp = prepeat();
		break ;
	  case sfor:
		tp = pfor();
		break ;
	  case scase:
		tp = pcase();
		break ;
	  case swith:
		tp = pwith();
		break ;
	  case sbegin:
		tp = pbegin(true);
		break ;
	  case sgoto:
		tp = pgoto();
		break ;
	  case send:  case selse:  case suntil:  case ssemic:
		tp = mknode(nempty);
		break ;
	  default:
		Caseerror(Line);
	}
	R144 = tp;
	return R144;
}

 treeptr
psimple()
{
	register treeptr	R145;
	treeptr	tq, tp;

	tp = pvariable(oldid(currsym.U.V1.vid, lidentifier));
	if (currsym.st == sassign) {
		tq = mknode(nassign);
		tq->U.V27.tlhs = tp;
		tq->U.V27.trhs = pexpr((treeptr)NIL);
		tp = tq;
	}
	R145 = tp;
	return R145;
}

 treeptr
pvariable(varptr)
	treeptr	varptr;
{
	register treeptr	R146;
	treeptr	tp, tq;

	nextsymbol(*((symset *)Conset[101]));
	if (Member((unsigned)(currsym.st), Conset[102])) {
		switch (currsym.st) {
		  case slpar:
			tp = mknode(ncall);
			tp->U.V30.tcall = varptr;
			tq = (struct S61 *)NIL;
			do {
				if (tq == (struct S61 *)NIL) {
					tq = pexpr((treeptr)NIL);
					tp->U.V30.taparm = tq;
				} else {
					tq->tnext = pexpr((treeptr)NIL);
					tq = tq->tnext;
				}
			} while (!(currsym.st == srpar));
			break ;
		  case slbrack:
			tq = varptr;
			do {
				tp = mknode(nindex);
				tp->U.V39.tvariable = tq;
				tp->U.V39.toffset = pexpr((treeptr)NIL);
				tq = tp;
			} while (!(currsym.st == srbrack));
			break ;
		  case sdot:
			tp = mknode(nselect);
			tp->U.V40.trecord = varptr;
			nextsymbol(*((symset *)Conset[103]));
			tq = typeof(varptr);
			enterscope(tq->U.V21.trscope);
			tp->U.V40.tfield = oldid(currsym.U.V1.vid, lfield);
			leavescope();
			break ;
		  case sarrow:
			tp = mknode(nderef);
			tp->U.V42.texps = varptr;
			break ;
		  default:
			Caseerror(Line);
		}
		tp = pvariable(tp);
	} else {
		tp = varptr;
		if (tp->tt == nid) {
			tq = idup(tp);
			if (tq != (struct S61 *)NIL)
				if (Member((unsigned)(tq->tt), Conset[104])) {
					tp = mknode(ncall);
					tp->U.V30.tcall = varptr;
					tp->U.V30.taparm = (struct S61 *)NIL;
				}
		}
	}
	R146 = tp;
	return R146;
}

treeptr pexpr();

 treeptr
padjust(tu, tr)
	treeptr	tu, tr;
{
	register treeptr	R148;

	if (pprio.A[(int)(tu->tt) - (int)(nassign)] >= pprio.A[(int)(tr->tt) - (int)(nassign)]) {
		if (Member((unsigned)(tr->tt), Conset[105]))
			tr->U.V42.texps = padjust(tu, tr->U.V42.texps);
		else
			tr->U.V41.texpl = padjust(tu, tr->U.V41.texpl);
		R148 = tr;
	} else {
		if (Member((unsigned)(tu->tt), Conset[106]))
			tu->U.V42.texps = tr;
		else
			tu->U.V41.texpr = tr;
		R148 = tu;
	}
	return R148;
}

 treeptr
pexpr(tnp)
	treeptr	tnp;
{
	register treeptr	R147;
	treeptr	tp, tq;
	treetyp	nt;
	boolean	next;

	nextsymbol(*((symset *)Conset[107]));
	next = true;
	switch (currsym.st) {
	  case splus:
		tp = mknode(nuplus);
		tp->U.V42.texps = (struct S61 *)NIL;
		tp = pexpr(tp);
		next = false;
		break ;
	  case sminus:
		tp = mknode(numinus);
		tp->U.V42.texps = (struct S61 *)NIL;
		tp = pexpr(tp);
		next = false;
		break ;
	  case snot:
		tp = mknode(nnot);
		tp->U.V42.texps = (struct S61 *)NIL;
		tp = pexpr(tp);
		next = false;
		break ;
	  case schar:  case sinteger:  case sreal:  case sstring:
		tp = mklit();
		break ;
	  case snil:
		usenilp = true;
		tp = mknode(nnil);
		break ;
	  case sid:
		tp = pvariable(oldid(currsym.U.V1.vid, lidentifier));
		next = false;
		break ;
	  case slpar:
		tp = mknode(nuplus);
		tp->U.V42.texps = pexpr((treeptr)NIL);
		break ;
	  case slbrack:
		usesets = true;
		tp = mknode(nset);
		tp->U.V42.texps = (struct S61 *)NIL;
		tq = (struct S61 *)NIL;
		do {
			if (tq == (struct S61 *)NIL) {
				tq = pexpr((treeptr)NIL);
				tp->U.V42.texps = tq;
			} else {
				tq->tnext = pexpr((treeptr)NIL);
				tq = tq->tnext;
			}
		} while (!(currsym.st == srbrack));
		break ;
	  case srbrack:
		tp = mknode(nempty);
		next = false;
		break ;
	  default:
		Caseerror(Line);
	}
	if (next)
		nextsymbol(*((symset *)Conset[108]));
	switch (currsym.st) {
	  case sdotdot:
		nt = nrange;
		break ;
	  case splus:
		nt = nplus;
		break ;
	  case sminus:
		nt = nminus;
		break ;
	  case smul:
		nt = nmul;
		break ;
	  case sdiv:
		nt = ndiv;
		break ;
	  case smod:
		nt = nmod;
		break ;
	  case squot:
		defnams.A[(int)(dreal)]->U.V6.lused = true;
		nt = nquot;
		break ;
	  case sand:
		nt = nand;
		break ;
	  case sor:
		nt = nor;
		break ;
	  case sinn:
		nt = nin;
		usesets = true;
		break ;
	  case sle:
		nt = nle;
		break ;
	  case slt:
		nt = nlt;
		break ;
	  case seq:
		nt = neq;
		break ;
	  case sge:
		nt = nge;
		break ;
	  case sgt:
		nt = ngt;
		break ;
	  case sne:
		nt = nne;
		break ;
	  case scolon:
		nt = nformat;
		break ;
	  case sid:  case schar:  case sinteger:  case sreal:
	  case sstring:  case snil:  case ssemic:  case scomma:
	  case slpar:  case slbrack:  case srpar:  case srbrack:
	  case send:  case suntil:  case sthen:  case selse:
	  case sdo:  case sdownto:  case sto:  case sof:
		nt = nnil;
		break ;
	  default:
		Caseerror(Line);
	}
	if (Member((unsigned)(nt), Conset[109]))
		defnams.A[(int)(dboolean)]->U.V6.lused = true;
	if (nt != nnil) {
		tq = mknode(nt);
		tq->U.V41.texpl = tp;
		tq->U.V41.texpr = (struct S61 *)NIL;
		tp = pexpr(tq);
	}
	if (tnp != (struct S61 *)NIL)
		tp = padjust(tnp, tp);
	R147 = tp;
	return R147;
}

 treeptr
pcase()
{
	register treeptr	R149;
	treeptr	tp, tq, tv;

	tp = mknode(ncase);
	tp->U.V35.tcasxp = pexpr((treeptr)NIL);
	checksymbol(*((symset *)Conset[110]));
	tq = (struct S61 *)NIL;
	do {
		if (tq == (struct S61 *)NIL) {
			tq = mknode(nchoise);
			tp->U.V35.tcaslst = tq;
		} else {
			tq->tnext = mknode(nchoise);
			tq = tq->tnext;
		}
		tv = (struct S61 *)NIL;
		do {
			nextsymbol(*((symset *)Conset[111]));
			if (Member((unsigned)(currsym.st), Conset[112]))
				goto L999;
			if (tv == (struct S61 *)NIL) {
				tv = pconstant(false);
				tq->U.V36.tchocon = tv;
			} else {
				tv->tnext = pconstant(false);
				tv = tv->tnext;
			}
			nextsymbol(*((symset *)Conset[113]));
		} while (!(currsym.st == scolon));
		tq->U.V36.tchostmt = plabstmt();
	} while (!(currsym.st == send));
L999:
	if (currsym.st == sother) {
		nextsymbol(*((symset *)Conset[114]));
		if (currsym.st == scolon)
			nextsymbol(*((symset *)Conset[115]));
		tp->U.V35.tcasother = pstmt();
	} else {
		tp->U.V35.tcasother = (struct S61 *)NIL;
		usecase = true;
	}
	nextsymbol(*((symset *)Conset[116]));
	R149 = tp;
	return R149;
}

 treeptr
pif()
{
	register treeptr	R150;
	treeptr	tp;

	tp = mknode(nif);
	tp->U.V31.tifxp = pexpr((treeptr)NIL);
	checksymbol(*((symset *)Conset[117]));
	tp->U.V31.tthen = plabstmt();
	if (currsym.st == selse)
		tp->U.V31.telse = plabstmt();
	else
		tp->U.V31.telse = (struct S61 *)NIL;
	R150 = tp;
	return R150;
}

 treeptr
pwhile()
{
	register treeptr	R151;
	treeptr	tp;

	tp = mknode(nwhile);
	tp->U.V32.twhixp = pexpr((treeptr)NIL);
	checksymbol(*((symset *)Conset[118]));
	tp->U.V32.twhistmt = plabstmt();
	R151 = tp;
	return R151;
}

 treeptr
prepeat()
{
	register treeptr	R152;
	treeptr	tp, tq;

	tp = mknode(nrepeat);
	tq = (struct S61 *)NIL;
	do {
		if (tq == (struct S61 *)NIL) {
			tq = plabstmt();
			tp->U.V33.treptstmt = tq;
		} else {
			tq->tnext = plabstmt();
			tq = tq->tnext;
		}
		checksymbol(*((symset *)Conset[119]));
	} while (!(currsym.st == suntil));
	tp->U.V33.treptxp = pexpr((treeptr)NIL);
	R152 = tp;
	return R152;
}

 treeptr
pfor()
{
	register treeptr	R153;
	treeptr	tp;

	tp = mknode(nfor);
	nextsymbol(*((symset *)Conset[120]));
	tp->U.V34.tforid = oldid(currsym.U.V1.vid, lidentifier);
	nextsymbol(*((symset *)Conset[121]));
	tp->U.V34.tfrom = pexpr((treeptr)NIL);
	checksymbol(*((symset *)Conset[122]));
	tp->U.V34.tincr = (boolean)(currsym.st == sto);
	tp->U.V34.tto = pexpr((treeptr)NIL);
	checksymbol(*((symset *)Conset[123]));
	tp->U.V34.tforstmt = plabstmt();
	R153 = tp;
	return R153;
}

 treeptr
pwith()
{
	register treeptr	R154;
	treeptr	tp, tq;

	tp = mknode(nwith);
	tq = (struct S61 *)NIL;
	do {
		if (tq == (struct S61 *)NIL) {
			tq = mknode(nwithvar);
			tp->U.V37.twithvar = tq;
		} else {
			tq->tnext = mknode(nwithvar);
			tq = tq->tnext;
		}
		enterscope((declptr)NIL);
		tq->U.V38.tenv = currscope();
		tq->U.V38.texpw = pexpr((treeptr)NIL);
		scopeup(tq->U.V38.texpw);
		checksymbol(*((symset *)Conset[124]));
	} while (!(currsym.st == sdo));
	tp->U.V37.twithstmt = plabstmt();
	tq = tp->U.V37.twithvar;
	while (tq != (struct S61 *)NIL) {
		leavescope();
		tq = tq->tnext;
	}
	R154 = tp;
	return R154;
}

 treeptr
pgoto()
{
	register treeptr	R155;
	treeptr	tp;

	nextsymbol(*((symset *)Conset[125]));
	tp = mknode(ngoto);
	tp->U.V26.tlabel = oldlbl(false);
	nextsymbol(*((symset *)Conset[126]));
	R155 = tp;
	return R155;
}

 treeptr
pbegin(retain)
	boolean	retain;
{
	register treeptr	R156;
	treeptr	tp, tq;

	tq = (struct S61 *)NIL;
	do {
		if (tq == (struct S61 *)NIL) {
			tq = plabstmt();
			tp = tq;
		} else {
			tq->tnext = plabstmt();
			tq = tq->tnext;
		}
	} while (!(currsym.st == send));
	if (retain) {
		tq = mknode(nbegin);
		tq->U.V24.tbegin = tp;
		tp = tq;
	}
	nextsymbol(*((symset *)Conset[127]));
	R156 = tp;
	return R156;
}

 void
parse()
{
	nextsymbol(*((symset *)Conset[128]));
	if (currsym.st == spgm)
		top = pprogram();
	else
		top = pmodule();
	nextsymbol(*((symset *)Conset[129]));
}

 integer
cvalof(tp)
	treeptr	tp;
{
	register integer	R157;
	integer	v;
	treeptr	tq;

	switch (tp->tt) {
	  case nuplus:
		R157 = cvalof(tp->U.V42.texps);
		break ;
	  case numinus:
		R157 = -cvalof(tp->U.V42.texps);
		break ;
	  case nnot:
		R157 = 1 - cvalof(tp->U.V42.texps);
		break ;
	  case nid:
		tq = idup(tp);
		if (tq == (struct S61 *)NIL)
			fatal(etree);
		tp = tp->U.V43.tsym->lsymdecl;
		switch (tq->tt) {
		  case nscalar:
			v = 0;
			tq = tq->U.V17.tscalid;
			while (tq != (struct S61 *)NIL)
				if (tq == tp)
					tq = (struct S61 *)NIL;
				else {
					v = v + 1;
					tq = tq->tnext;
				}
			R157 = v;
			break ;
		  case nconst:
			R157 = cvalof(tq->U.V14.tbind);
			break ;
		  default:
			Caseerror(Line);
		}
		break ;
	  case ninteger:
		R157 = tp->U.V43.tsym->U.V10.linum;
		break ;
	  case nchar:
		R157 = (unsigned)(tp->U.V43.tsym->U.V11.lchar);
		break ;
	  default:
		Caseerror(Line);
	}
	return R157;
}

 integer
clower(tp)
	treeptr	tp;
{
	register integer	R158;
	treeptr	tq;

	tq = typeof(tp);
	if (tq->tt == nscalar)
		R158 = scalbase;
	else
		if (tq->tt == nsubrange)
			if (tq->tup->tt == nconfarr)
				R158 = 0;
			else
				R158 = cvalof(tq->U.V19.tlo);
		else
			if (tq == typnods.A[(int)(tchar)])
				R158 = 0;
			else
				if (tq == typnods.A[(int)(tinteger)])
					R158 = -maxint;
				else
					fatal(etree);
	return R158;
}

 integer
cupper(tp)
	treeptr	tp;
{
	register integer	R159;
	treeptr	tq;
	integer	i;

	tq = typeof(tp);
	if (tq->tt == nscalar) {
		tq = tq->U.V17.tscalid;
		i = scalbase;
		while (tq->tnext != (struct S61 *)NIL) {
			i = i + 1;
			tq = tq->tnext;
		}
		R159 = i;
	} else
		if (tq->tt == nsubrange)
			if (tq->tup->tt == nconfarr)
				fatal(euprconf);
			else
				R159 = cvalof(tq->U.V19.thi);
		else
			if (tq == typnods.A[(int)(tchar)])
				R159 = maxchar;
			else
				if (tq == typnods.A[(int)(tinteger)])
					R159 = maxint;
				else
					fatal(etree);
	return R159;
}

 integer
crange(tp)
	treeptr	tp;
{
	register integer	R160;

	R160 = cupper(tp) - clower(tp) + 1;
	return R160;
}

 integer
csetwords(i)
	integer	i;
{
	register integer	R161;

	i = (i + (C37_setbits)) / (C37_setbits + 1);
	if (i > maxsetrange)
		error(esetsize);
	R161 = i;
	return R161;
}

 integer
csetsize(tp)
	treeptr	tp;
{
	register integer	R162;
	treeptr	tq;
	integer	i;

	tq = typeof(tp->U.V18.tof);
	i = clower(tq);
	if ((i < 0) || (i >= 6 * (C37_setbits + 1)))
		error(esetbase);
	R162 = csetwords(crange(tq)) + 1;
	return R162;
}

 boolean
islocal(tp)
	treeptr	tp;
{
	register boolean	R163;
	treeptr	tq;

	tq = tp->U.V43.tsym->lsymdecl;
	while (!(Member((unsigned)(tq->tt), Conset[130])))
		tq = tq->tup;
	while (!(Member((unsigned)(tp->tt), Conset[131])))
		tp = tp->tup;
	R163 = (boolean)(tp == tq);
	return R163;
}

void transform();

void renamf();

 void
crtnvar(tp)
	treeptr	tp;
{
	while (tp != (struct S61 *)NIL) {
		switch (tp->tt) {
		  case npgm:
			crtnvar(tp->U.V13.tsubsub);
			break ;
		  case nfunc:  case nproc:
			crtnvar(tp->U.V13.tsubsub);
			crtnvar(tp->U.V13.tsubstmt);
			break ;
		  case nbegin:
			crtnvar(tp->U.V24.tbegin);
			break ;
		  case nif:
			crtnvar(tp->U.V31.tthen);
			crtnvar(tp->U.V31.telse);
			break ;
		  case nwhile:
			crtnvar(tp->U.V32.twhistmt);
			break ;
		  case nrepeat:
			crtnvar(tp->U.V33.treptstmt);
			break ;
		  case nfor:
			crtnvar(tp->U.V34.tforstmt);
			break ;
		  case ncase:
			crtnvar(tp->U.V35.tcaslst);
			crtnvar(tp->U.V35.tcasother);
			break ;
		  case nchoise:
			crtnvar(tp->U.V36.tchostmt);
			break ;
		  case nwith:
			crtnvar(tp->U.V37.twithstmt);
			break ;
		  case nlabstmt:
			crtnvar(tp->U.V25.tstmt);
			break ;
		  case nassign:
			if (tp->U.V27.tlhs->tt == ncall) {
				tp->U.V27.tlhs = tp->U.V27.tlhs->U.V30.tcall;
				tp->U.V27.tlhs->tup = tp;
			}
			(*G187_tv) = tp->U.V27.tlhs;
			if ((*G187_tv)->tt == nid)
				if ((*G187_tv)->U.V43.tsym == (*G183_ip))
					(*G187_tv)->U.V43.tsym = (*G185_iq);
			break ;
		  case nbreak:  case npush:  case npop:  case ngoto:
		  case nempty:  case ncall:
			break ;
		  default:
			Caseerror(Line);
		}
		tp = tp->tnext;
	}
}

 void
renamf(tp)
	treeptr	tp;
{
	symptr	ip, iq;
	treeptr	tq, tv;
	symptr	*F184;
	symptr	*F186;
	treeptr	*F188;

	F188 = G187_tv;
	G187_tv = &tv;
	F186 = G185_iq;
	G185_iq = &iq;
	F184 = G183_ip;
	G183_ip = &ip;
	while (tp != (struct S61 *)NIL) {
		switch (tp->tt) {
		  case npgm:  case nproc:
			renamf(tp->U.V13.tsubsub);
			break ;
		  case nfunc:
			tq = mknode(nvar);
			tq->U.V14.tattr = aregister;
			tq->tup = tp;
			tq->U.V14.tidl = newid(mkvariable('R'));
			tq->U.V14.tidl->tup = tq;
			tq->U.V14.tbind = tp->U.V13.tfuntyp;
			tq->tnext = tp->U.V13.tsubvar;
			tp->U.V13.tsubvar = tq;
			(*G185_iq) = tq->U.V14.tidl->U.V43.tsym;
			(*G183_ip) = tp->U.V13.tsubid->U.V43.tsym;
			crtnvar(tp->U.V13.tsubsub);
			crtnvar(tp->U.V13.tsubstmt);
			renamf(tp->U.V13.tsubsub);
			break ;
		  default:
			Caseerror(Line);
		}
		tp = tp->tnext;
	}
	G183_ip = F184;
	G185_iq = F186;
	G187_tv = F188;
}

void extract();

 treeptr
xtrit(tp, pp, last)
	treeptr	tp, pp;
	boolean	last;
{
	register treeptr	R164;
	treeptr	np, rp;
	idptr	ip;

	np = mknode(ntype);
	ip = mkvariable('T');
	np->U.V14.tidl = newid(ip);
	np->U.V14.tidl->tup = np;
	rp = oldid(ip, lidentifier);
	rp->tup = tp->tup;
	rp->tnext = tp->tnext;
	np->U.V14.tbind = tp;
	tp->tup = np;
	tp->tnext = (struct S61 *)NIL;
	np->tup = pp;
	if (last && (pp->U.V13.tsubtype != (struct S61 *)NIL)) {
		pp = pp->U.V13.tsubtype;
		while (pp->tnext != (struct S61 *)NIL)
			pp = pp->tnext;
		pp->tnext = np;
	} else {
		np->tnext = pp->U.V13.tsubtype;
		pp->U.V13.tsubtype = np;
	}
	R164 = rp;
	return R164;
}

treeptr xtrenum();

 void
nametype(tp)
	treeptr	tp;
{
	tp = typeof(tp);
	if (tp->tt == nrecord)
		if (tp->U.V21.tuid == (struct S59 *)NIL)
			tp->U.V21.tuid = mkvariable('S');
}

 treeptr
xtrenum(tp, pp)
	treeptr	tp, pp;
{
	register treeptr	R165;

	if (tp != (struct S61 *)NIL) {
		switch (tp->tt) {
		  case nfield:  case ntype:  case nvar:
			tp->U.V14.tbind = xtrenum(tp->U.V14.tbind, pp);
			break ;
		  case nscalar:
			if (tp->tup->tt != ntype)
				tp = xtrit(tp, pp, false);
			break ;
		  case narray:
			tp->U.V23.taindx = xtrenum(tp->U.V23.taindx, pp);
			tp->U.V23.taelem = xtrenum(tp->U.V23.taelem, pp);
			break ;
		  case nrecord:
			tp->U.V21.tflist = xtrenum(tp->U.V21.tflist, pp);
			tp->U.V21.tvlist = xtrenum(tp->U.V21.tvlist, pp);
			break ;
		  case nvariant:
			tp->U.V20.tvrnt = xtrenum(tp->U.V20.tvrnt, pp);
			break ;
		  case nfileof:
			tp->U.V18.tof = xtrenum(tp->U.V18.tof, pp);
			break ;
		  case nptr:
			nametype(tp->U.V16.tptrid);
			break ;
		  case nid:  case nsubrange:  case npredef:  case nempty:
		  case nsetof:
			break ;
		  default:
			Caseerror(Line);
		}
		tp->tnext = xtrenum(tp->tnext, pp);
	}
	R165 = tp;
	return R165;
}
