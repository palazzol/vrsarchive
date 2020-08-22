#include "globals.h"

 void
eexpr(tp)
	treeptr	tp;
{
	treeptr	tq;
	boolean	flag;

	(*G200_donearr) = false;
	if (Member((unsigned)(tp->tt), Conset[144])) {
		tq = typeof(tp->U.V41.texpl);
		if ((Member((unsigned)(tq->tt), Conset[145])) || (tq == typnods.A[(int)(tset)])) {
			switch (tp->tt) {
			  case nplus:
				(*G196_setused) = true;
				useunion = true;
				(void)fprintf(output.fp, "Union"), Putl(output, 0);
				break ;
			  case nminus:
				(*G196_setused) = true;
				usediff = true;
				(void)fprintf(output.fp, "Diff"), Putl(output, 0);
				break ;
			  case nmul:
				(*G196_setused) = true;
				useintr = true;
				(void)fprintf(output.fp, "Inter"), Putl(output, 0);
				break ;
			  case neq:
				useseq = true;
				(void)fprintf(output.fp, "Eq"), Putl(output, 0);
				break ;
			  case nne:
				usesne = true;
				(void)fprintf(output.fp, "Ne"), Putl(output, 0);
				break ;
			  case nge:
				usesge = true;
				(void)fprintf(output.fp, "Ge"), Putl(output, 0);
				break ;
			  case nle:
				usesle = true;
				(void)fprintf(output.fp, "Le"), Putl(output, 0);
				break ;
			  default:
				Caseerror(Line);
			}
			if (Member((unsigned)(tp->tt), Conset[146]))
				(*G198_dropset) = false;
			Putchr('(', output);
			eexpr(tp->U.V41.texpl);
			if (tq->tt == nsetof)
				(void)fprintf(output.fp, ".S"), Putl(output, 0);
			(void)fprintf(output.fp, ", "), Putl(output, 0);
			eexpr(tp->U.V41.texpr);
			tq = typeof(tp->U.V41.texpr);
			if (tq->tt == nsetof)
				(void)fprintf(output.fp, ".S"), Putl(output, 0);
			Putchr(')', output);
			goto L999;
		}
	}
	if (Member((unsigned)(tp->tt), Conset[147])) {
		tq = typeof(tp->U.V41.texpl);
		if (tq->tt == nconfarr)
			fatal(ecmpconf);
		if ((Member((unsigned)(tq->tt), Conset[148])) || (tq == typnods.A[(int)(tstring)])) {
			(void)fprintf(output.fp, "Cmpstr("), Putl(output, 0);
			eexpr(tp->U.V41.texpl);
			if (tq->tt == narray)
				(void)fprintf(output.fp, ".A"), Putl(output, 0);
			(void)fprintf(output.fp, ", "), Putl(output, 0);
			tq = typeof(tp->U.V41.texpr);
			if (tq->tt == nconfarr)
				fatal(ecmpconf);
			eexpr(tp->U.V41.texpr);
			if (tq->tt == narray)
				(void)fprintf(output.fp, ".A"), Putl(output, 0);
			Putchr(')', output);
			switch (tp->tt) {
			  case neq:
				(void)fprintf(output.fp, " == "), Putl(output, 0);
				break ;
			  case nne:
				(void)fprintf(output.fp, " != "), Putl(output, 0);
				break ;
			  case ngt:
				(void)fprintf(output.fp, " > "), Putl(output, 0);
				break ;
			  case nlt:
				(void)fprintf(output.fp, " < "), Putl(output, 0);
				break ;
			  case nge:
				(void)fprintf(output.fp, " >= "), Putl(output, 0);
				break ;
			  case nle:
				(void)fprintf(output.fp, " <= "), Putl(output, 0);
				break ;
			  default:
				Caseerror(Line);
			}
			Putchr('0', output);
			goto L999;
		}
	}
	switch (tp->tt) {
	  case neq:  case nne:  case nlt:  case nle:
	  case ngt:  case nge:  case nor:  case nand:
	  case nplus:  case nminus:  case nmul:  case ndiv:
	  case nmod:  case nquot:
		flag = (boolean)(cprio.A[(int)(tp->tt) - (int)(nassign)] > cprio.A[(int)(tp->U.V41.texpl->tt) - (int)(nassign)]);
		if ((Member((unsigned)(tp->tt), Conset[149])) && !arithexpr(tp->U.V41.texpl)) {
			(void)fprintf(output.fp, "(integer)"), Putl(output, 0);
			flag = true;
		}
		if (flag)
			Putchr('(', output);
		eexpr(tp->U.V41.texpl);
		if (flag)
			Putchr(')', output);
		switch (tp->tt) {
		  case neq:
			(void)fprintf(output.fp, " == "), Putl(output, 0);
			break ;
		  case nne:
			(void)fprintf(output.fp, " != "), Putl(output, 0);
			break ;
		  case nlt:
			(void)fprintf(output.fp, " < "), Putl(output, 0);
			break ;
		  case nle:
			(void)fprintf(output.fp, " <= "), Putl(output, 0);
			break ;
		  case ngt:
			(void)fprintf(output.fp, " > "), Putl(output, 0);
			break ;
		  case nge:
			(void)fprintf(output.fp, " >= "), Putl(output, 0);
			break ;
		  case nor:
			(void)fprintf(output.fp, " || "), Putl(output, 0);
			break ;
		  case nand:
			(void)fprintf(output.fp, " && "), Putl(output, 0);
			break ;
		  case nplus:
			(void)fprintf(output.fp, " + "), Putl(output, 0);
			break ;
		  case nminus:
			(void)fprintf(output.fp, " - "), Putl(output, 0);
			break ;
		  case nmul:
			(void)fprintf(output.fp, " * "), Putl(output, 0);
			break ;
		  case ndiv:
			(void)fprintf(output.fp, " / "), Putl(output, 0);
			break ;
		  case nmod:
			(void)fprintf(output.fp, " %% "), Putl(output, 0);
			break ;
		  case nquot:
			(void)fprintf(output.fp, " / (("), Putl(output, 0);
			printid(defnams.A[(int)(dreal)]->U.V6.lid);
			Putchr(')', output);
			break ;
		  default:
			Caseerror(Line);
		}
		flag = (boolean)(cprio.A[(int)(tp->tt) - (int)(nassign)] > cprio.A[(int)(tp->U.V41.texpr->tt) - (int)(nassign)]);
		if ((Member((unsigned)(tp->tt), Conset[150])) && !arithexpr(tp->U.V41.texpr)) {
			(void)fprintf(output.fp, "(integer)"), Putl(output, 0);
			flag = true;
		}
		if (flag)
			Putchr('(', output);
		eexpr(tp->U.V41.texpr);
		if (flag)
			Putchr(')', output);
		if (tp->tt == nquot)
			Putchr(')', output);
		break ;
	  case nuplus:  case numinus:  case nnot:
		switch (tp->tt) {
		  case numinus:
			Putchr('-', output);
			break ;
		  case nnot:
			Putchr('!', output);
			break ;
		  case nuplus:
			break ;
		  default:
			Caseerror(Line);
		}
		flag = (boolean)(cprio.A[(int)(tp->tt) - (int)(nassign)] >= cprio.A[(int)(tp->U.V42.texps->tt) - (int)(nassign)]);
		if (flag)
			Putchr('(', output);
		eexpr(tp->U.V42.texps);
		if (flag)
			Putchr(')', output);
		break ;
	  case nin:
		usememb = true;
		(void)fprintf(output.fp, "Member((unsigned)("), Putl(output, 0);
		eexpr(tp->U.V41.texpl);
		(void)fprintf(output.fp, "), "), Putl(output, 0);
		(*G198_dropset) = true;
		eexpr(tp->U.V41.texpr);
		(*G198_dropset) = false;
		tq = typeof(tp->U.V41.texpr);
		if (tq->tt == nsetof)
			(void)fprintf(output.fp, ".S"), Putl(output, 0);
		Putchr(')', output);
		break ;
	  case nassign:
		tq = typeof(tp->U.V27.trhs);
		if (tq == typnods.A[(int)(tstring)]) {
			(void)fprintf(output.fp, "%sstrncpy(", voidcast), Putl(output, 0);
			eexpr(tp->U.V27.tlhs);
			(void)fprintf(output.fp, ".A, "), Putl(output, 0);
			eexpr(tp->U.V27.trhs);
			(void)fprintf(output.fp, ", sizeof("), Putl(output, 0);
			eexpr(tp->U.V27.tlhs);
			(void)fprintf(output.fp, ".A))"), Putl(output, 0);
		} else
			if (tq == typnods.A[(int)(tboolean)]) {
				eexpr(tp->U.V27.tlhs);
				(void)fprintf(output.fp, " = "), Putl(output, 0);
				tq = tp->U.V27.trhs;
				while (tq->tt == nuplus)
					tq = tq->U.V42.texps;
				if (Member((unsigned)(tq->tt), Conset[151])) {
					Putchr('(', output);
					printid(defnams.A[(int)(dboolean)]->U.V6.lid);
					(void)fprintf(output.fp, ")("), Putl(output, 0);
					eexpr(tq);
					Putchr(')', output);
				} else
					eexpr(tq);
			} else
				if (tq == typnods.A[(int)(tnil)]) {
					eexpr(tp->U.V27.tlhs);
					(void)fprintf(output.fp, " = ("), Putl(output, 0);
					etypedef(typeof(tp->U.V27.tlhs));
					(void)fprintf(output.fp, ")NIL"), Putl(output, 0);
				} else {
					tq = typeof(tp->U.V27.tlhs);
					if (tq->tt == nsetof) {
						usescpy = true;
						(void)fprintf(output.fp, "Setncpy("), Putl(output, 0);
						eselect(tp->U.V27.tlhs);
						(void)fprintf(output.fp, "S, "), Putl(output, 0);
						(*G198_dropset) = true;
						tq = typeof(tp->U.V27.trhs);
						if (tq == typnods.A[(int)(tset)])
							eexpr(tp->U.V27.trhs);
						else {
							eselect(tp->U.V27.trhs);
							Putchr('S', output);
						}
						(*G198_dropset) = false;
						(void)fprintf(output.fp, ", sizeof("), Putl(output, 0);
						eselect(tp->U.V27.tlhs);
						(void)fprintf(output.fp, "S))"), Putl(output, 0);
					} else {
						eexpr(tp->U.V27.tlhs);
						(void)fprintf(output.fp, " = "), Putl(output, 0);
						eexpr(tp->U.V27.trhs);
					}
				}
		break ;
	  case ncall:
		tq = idup(tp->U.V30.tcall);
		if ((Member((unsigned)(tq->tt), Conset[152])) && (tq->U.V13.tsubstmt != (struct S61 *)NIL))
			if (tq->U.V13.tsubstmt->tt == npredef)
				epredef(tq, tp);
			else
				ecall(tp);
		else
			ecall(tp);
		break ;
	  case nselect:
		eselect(tp->U.V40.trecord);
		eexpr(tp->U.V40.tfield);
		break ;
	  case nindex:
		eselect(tp->U.V39.tvariable);
		(void)fprintf(output.fp, "A["), Putl(output, 0);
		tq = tp->U.V39.toffset;
		if (arithexpr(tq))
			eexpr(tq);
		else {
			(void)fprintf(output.fp, "(integer)("), Putl(output, 0);
			eexpr(tq);
			Putchr(')', output);
		}
		tq = typeof(tp->U.V39.tvariable);
		if (tq->tt == narray)
			if (clower(tq->U.V23.taindx) != 0) {
				(void)fprintf(output.fp, " - "), Putl(output, 0);
				tq = typeof(tq->U.V23.taindx);
				if (tq->tt == nsubrange)
					if (arithexpr(tq->U.V19.tlo))
						eexpr(tq->U.V19.tlo);
					else {
						(void)fprintf(output.fp, "(integer)("), Putl(output, 0);
						eexpr(tq->U.V19.tlo);
						Putchr(')', output);
					}
				else
					fatal(etree);
			}
		Putchr(']', output);
		break ;
	  case nderef:
		tq = typeof(tp->U.V42.texps);
		if ((tq->tt == nfileof) || ((tq->tt == npredef) && (tq->U.V12.tdef == dtext))) {
			eexpr(tp->U.V42.texps);
			(void)fprintf(output.fp, ".buf"), Putl(output, 0);
		} else
			if ((*G202_doarrow) == 0) {
				Putchr('*', output);
				eexpr(tp->U.V42.texps);
			} else {
				eexpr(tp->U.V42.texps);
				(void)fprintf(output.fp, "->"), Putl(output, 0);
				(*G200_donearr) = true;
			}
		break ;
	  case nid:
		tq = idup(tp);
		if (tq->tt == nvarpar) {
			if (((*G202_doarrow) == 0) || (tq->U.V14.tattr == areference)) {
				(void)fprintf(output.fp, "(*"), Putl(output, 0);
				printid(tp->U.V43.tsym->U.V6.lid);
				Putchr(')', output);
			} else {
				printid(tp->U.V43.tsym->U.V6.lid);
				(void)fprintf(output.fp, "->"), Putl(output, 0);
				(*G200_donearr) = true;
			}
		} else
			if ((tq->tt == nconst) && (*G194_conflag))
				(void)fprintf(output.fp, "%1ldL", cvalof(tp)), Putl(output, 0);
			else
				if (Member((unsigned)(tq->tt), Conset[153])) {
					(void)fprintf(output.fp, "(*"), Putl(output, 0);
					printid(tp->U.V43.tsym->U.V6.lid);
					Putchr(')', output);
				} else
					printid(tp->U.V43.tsym->U.V6.lid);
		break ;
	  case nchar:
		printchr(tp->U.V43.tsym->U.V11.lchar);
		break ;
	  case ninteger:
		(void)fprintf(output.fp, "%1ldL", tp->U.V43.tsym->U.V10.linum), Putl(output, 0);
		break ;
	  case nreal:
		printtok(tp->U.V43.tsym->U.V8.lfloat);
		break ;
	  case nstring:
		printstr(tp->U.V43.tsym->U.V7.lstr);
		break ;
	  case nset:
		if (constset(tp->U.V42.texps)) {
			(void)fprintf(output.fp, "Conset[%1d]", setcnt), Putl(output, 0);
			setcnt = setcnt + 1;
			tq = mknode(nset);
			tq->tnext = setlst;
			setlst = tq;
			tq->U.V42.texps = tp->U.V42.texps;
		} else {
			increment();
			flag = (*G198_dropset);
			if ((*G198_dropset))
				(*G198_dropset) = false;
			else
				(void)fprintf(output.fp, "Saveset("), Putl(output, 0);
			(void)fprintf(output.fp, "(Tmpset = Newset(), "), Putl(output, 0);
			tq = tp->U.V42.texps;
			while (tq != (struct S61 *)NIL) {
				switch (tq->tt) {
				  case nrange:
					usemksub = true;
					(void)fprintf(output.fp, "%sMksubr(", voidcast), Putl(output, 0);
					(void)fprintf(output.fp, "(unsigned)("), Putl(output, 0);
					eexpr(tq->U.V41.texpl);
					(void)fprintf(output.fp, "), "), Putl(output, 0);
					(void)fprintf(output.fp, "(unsigned)("), Putl(output, 0);
					eexpr(tq->U.V41.texpr);
					(void)fprintf(output.fp, "), Tmpset)"), Putl(output, 0);
					break ;
				  case nin:  case neq:  case nne:  case nlt:
				  case nle:  case ngt:  case nge:  case nor:
				  case nand:  case nmul:  case ndiv:  case nmod:
				  case nquot:  case nplus:  case nminus:  case nnot:
				  case numinus:  case nuplus:  case nindex:  case nselect:
				  case nderef:  case ncall:  case ninteger:  case nchar:
				  case nid:
					useins = true;
					(void)fprintf(output.fp, "%sInsmem(", voidcast), Putl(output, 0);
					(void)fprintf(output.fp, "(unsigned)("), Putl(output, 0);
					eexpr(tq);
					(void)fprintf(output.fp, "), Tmpset)"), Putl(output, 0);
					break ;
				  default:
					Caseerror(Line);
				}
				tq = tq->tnext;
				if (tq != (struct S61 *)NIL) {
					Putchr(',', output),Putchr('\n', output);
					indent();
				}
			}
			(void)fprintf(output.fp, ", Tmpset)"), Putl(output, 0);
			if (!flag) {
				Putchr(')', output);
				(*G196_setused) = true;
			}
			decrement();
		}
		break ;
	  case nnil:
		tq = tp;
		do {
			tq = tq->tup;
		} while (!(Member((unsigned)(tq->tt), Conset[154])));
		if (Member((unsigned)(tq->tt), Conset[155])) {
			if (typeof(tq->U.V41.texpl) == typnods.A[(int)(tnil)])
				tq = typeof(tq->U.V41.texpr);
			else
				tq = typeof(tq->U.V41.texpl);
			if (tq->tt == nptr) {
				Putchr('(', output);
				etypedef(tq);
				Putchr(')', output);
			}
		}
		(void)fprintf(output.fp, "NIL"), Putl(output, 0);
		break ;
	  default:
		Caseerror(Line);
	}
L999:
	;
}

 void
econst(tp)
	treeptr	tp;
{
	symptr	sp;

	while (tp != (struct S61 *)NIL) {
		sp = tp->U.V14.tidl->U.V43.tsym;
		if (sp->U.V6.lid->inref > 1)
			sp->U.V6.lid = mkrename('X', sp->U.V6.lid);
		if (tp->U.V14.tbind->tt == nstring) {
			indent();
			(void)fprintf(output.fp, "%s%s%c", C50_static, chartyp, tab1), Putl(output, 0);
			printid(sp->U.V6.lid);
			(void)fprintf(output.fp, "[]	= "), Putl(output, 0);
			eexpr(tp->U.V14.tbind);
			Putchr(';', output),Putchr('\n', output);
		} else {
			(void)fprintf(output.fp, "%s", C4_define), Putl(output, 0);
			printid(sp->U.V6.lid);
			Putchr(space, output);
			eexpr(tp->U.V14.tbind);
			Putchr('\n', output);
		}
		tp = tp->tnext;
	}
}

void etdef();

 void
etrange(tp)
	treeptr	tp;
{
	integer	lo, hi;
	register unsigned char	i;

	lo = clower(tp);
	hi = cupper(tp);
	{
		unsigned char	B51 = 1,
			B52 = nmachdefs;

		if (B51 <= B52)
			for (i = B51; ; i++) {
				{
					register struct S193 *W53 = &machdefs.A[i - 1];

					if ((lo >= W53->lolim) && (hi <= W53->hilim)) {
						printtok(W53->typstr);
						goto L999;
					}
				}
				if (i == B52) break;
			}
	}
	fatal(erange);
L999:
	;
}

 void
printsuf(ip)
	idptr	ip;
{
	toknbuf	w;
	toknidx	i, j;

	gettokn(ip->istr, &w);
	i = 1;
	j = i;
	while (w.A[i - 1] != null) {
		if (w.A[i - 1] == '.')
			j = i;
		i = i + 1;
	}
	if (w.A[j - 1] == '.')
		j = j + 1;
	while (w.A[j - 1] != null) {
		Putchr(w.A[j - 1], output);
		j = j + 1;
	}
}

 void
etdef(uid, tp)
	idptr	uid;
	treeptr	tp;
{
	integer	i;
	treeptr	tq;

	switch (tp->tt) {
	  case nid:
		printid(tp->U.V43.tsym->U.V6.lid);
		break ;
	  case nptr:
		tq = typeof(tp->U.V16.tptrid);
		if (tq->tt == nrecord) {
			(void)fprintf(output.fp, "struct "), Putl(output, 0);
			printid(tq->U.V21.tuid);
		} else
			printid(tp->U.V16.tptrid->U.V43.tsym->U.V6.lid);
		(void)fprintf(output.fp, " *"), Putl(output, 0);
		break ;
	  case nscalar:
		(void)fprintf(output.fp, "enum { "), Putl(output, 0);
		increment();
		tp = tp->U.V17.tscalid;
		if (tp->U.V43.tsym->U.V6.lid->inref > 1)
			tp->U.V43.tsym->U.V6.lid = mkrename('E', tp->U.V43.tsym->U.V6.lid);
		printid(tp->U.V43.tsym->U.V6.lid);
		i = 1;
		while (tp->tnext != (struct S61 *)NIL) {
			if (i >= 4) {
				Putchr(',', output),Putchr('\n', output);
				indent();
				i = 1;
			} else {
				(void)fprintf(output.fp, ", "), Putl(output, 0);
				i = i + 1;
			}
			tp = tp->tnext;
			if (tp->U.V43.tsym->U.V6.lid->inref > 1)
				tp->U.V43.tsym->U.V6.lid = mkrename('E', tp->U.V43.tsym->U.V6.lid);
			printid(tp->U.V43.tsym->U.V6.lid);
		}
		decrement();
		(void)fprintf(output.fp, " } "), Putl(output, 0);
		break ;
	  case nsubrange:
		tq = typeof(tp->U.V19.tlo);
		if (tq == typnods.A[(int)(tinteger)])
			etrange(tp);
		else {
			if (tq->tup->tt == ntype)
				tq = tq->tup->U.V14.tidl;
			etdef((idptr)NIL, tq);
		}
		break ;
	  case nfield:
		etdef((idptr)NIL, tp->U.V14.tbind);
		Putchr(tab1, output);
		tp = tp->U.V14.tidl;
		if (uid != (struct S59 *)NIL)
			tp->U.V43.tsym->U.V6.lid = mkconc('.', uid, tp->U.V43.tsym->U.V6.lid);
		printsuf(tp->U.V43.tsym->U.V6.lid);
		i = 1;
		while (tp->tnext != (struct S61 *)NIL) {
			if (i >= 4) {
				Putchr(',', output),Putchr('\n', output);
				indent();
				Putchr(tab1, output);
				i = 1;
			} else {
				(void)fprintf(output.fp, ", "), Putl(output, 0);
				i = i + 1;
			}
			tp = tp->tnext;
			if (uid != (struct S59 *)NIL)
				tp->U.V43.tsym->U.V6.lid = mkconc('.', uid, tp->U.V43.tsym->U.V6.lid);
			printsuf(tp->U.V43.tsym->U.V6.lid);
		}
		Putchr(';', output),Putchr('\n', output);
		break ;
	  case nrecord:
		(void)fprintf(output.fp, "struct "), Putl(output, 0);
		if (tp->U.V21.tuid == (struct S59 *)NIL)
			tp->U.V21.tuid = uid;
		else
			if (uid == (struct S59 *)NIL)
				printid(tp->U.V21.tuid);
		(void)fprintf(output.fp, " {\n"), Putl(output, 1);
		increment();
		if ((tp->U.V21.tflist == (struct S61 *)NIL) && (tp->U.V21.tvlist == (struct S61 *)NIL)) {
			indent();
			(void)fprintf(output.fp, "%s%cdummy;\n", inttyp, tab1), Putl(output, 1);
		}
		tq = tp->U.V21.tflist;
		while (tq != (struct S61 *)NIL) {
			indent();
			etdef(uid, tq);
			tq = tq->tnext;
		}
		if (tp->U.V21.tvlist != (struct S61 *)NIL) {
			indent();
			(void)fprintf(output.fp, "union {\n"), Putl(output, 1);
			increment();
			tq = tp->U.V21.tvlist;
			while (tq != (struct S61 *)NIL) {
				if ((tq->U.V20.tvrnt->U.V21.tflist != (struct S61 *)NIL) || (tq->U.V20.tvrnt->U.V21.tvlist != (struct S61 *)NIL)) {
					indent();
					if (uid == (struct S59 *)NIL)
						etdef(mkvrnt(), tq->U.V20.tvrnt);
					else
						etdef(mkconc('.', uid, mkvrnt()), tq->U.V20.tvrnt);
					Putchr(';', output),Putchr('\n', output);
				}
				tq = tq->tnext;
			}
			decrement();
			indent();
			(void)fprintf(output.fp, "} U;\n"), Putl(output, 1);
		}
		decrement();
		indent();
		if (tp->tup->tt == nvariant) {
			(void)fprintf(output.fp, "} "), Putl(output, 0);
			printsuf(tp->U.V21.tuid);
		} else
			Putchr('}', output);
		break ;
	  case nconfarr:
		(void)fprintf(output.fp, "struct "), Putl(output, 0);
		printid(tp->U.V22.tcuid);
		(void)fprintf(output.fp, " { "), Putl(output, 0);
		etdef((idptr)NIL, tp->U.V22.tcelem);
		(void)fprintf(output.fp, "%cA[]; }", tab1), Putl(output, 0);
		break ;
	  case narray:
		(void)fprintf(output.fp, "struct { "), Putl(output, 0);
		etdef((idptr)NIL, tp->U.V23.taelem);
		(void)fprintf(output.fp, "%cA[", tab1), Putl(output, 0);
		tq = typeof(tp->U.V23.taindx);
		if (tq->tt == nsubrange) {
			if (arithexpr(tq->U.V19.thi)) {
				eexpr(tq->U.V19.thi);
				if (cvalof(tq->U.V19.tlo) != 0) {
					(void)fprintf(output.fp, " - "), Putl(output, 0);
					eexpr(tq->U.V19.tlo);
				}
			} else {
				(void)fprintf(output.fp, "(integer)("), Putl(output, 0);
				eexpr(tq->U.V19.thi);
				if (cvalof(tq->U.V19.tlo) != 0) {
					(void)fprintf(output.fp, ") - (integer)("), Putl(output, 0);
					eexpr(tq->U.V19.tlo);
				}
				Putchr(')', output);
			}
			(void)fprintf(output.fp, " + 1"), Putl(output, 0);
		} else
			(void)fprintf(output.fp, "%1d", crange(tp->U.V23.taindx)), Putl(output, 0);
		(void)fprintf(output.fp, "]; }"), Putl(output, 0);
		break ;
	  case nfileof:
		(void)fprintf(output.fp, "struct {\n"), Putl(output, 1);
		indent();
		(void)fprintf(output.fp, "%cFILE%c*fp;\n", tab1, tab1), Putl(output, 1);
		indent();
		(void)fprintf(output.fp, "%c%s%ceoln:1,\n", tab1, filebits, tab1), Putl(output, 1);
		indent();
		(void)fprintf(output.fp, "%seof:1,\n", tab3), Putl(output, 1);
		indent();
		(void)fprintf(output.fp, "%sout:1,\n", tab3), Putl(output, 1);
		indent();
		(void)fprintf(output.fp, "%sinit:1,\n", tab3), Putl(output, 1);
		indent();
		(void)fprintf(output.fp, "%s:%1d;\n", tab3, filefill), Putl(output, 1);
		indent();
		Putchr(tab1, output);
		etdef((idptr)NIL, tp->U.V18.tof);
		(void)fprintf(output.fp, "%cbuf;\n", tab1), Putl(output, 1);
		indent();
		(void)fprintf(output.fp, "} "), Putl(output, 0);
		break ;
	  case nsetof:
		(void)fprintf(output.fp, "struct { %s%cS[%1d]; }", setwtyp, tab1, csetsize(tp)), Putl(output, 0);
		break ;
	  case npredef:
		switch (tp->U.V12.tobtyp) {
		  case tboolean:
			printid(defnams.A[(int)(dboolean)]->U.V6.lid);
			break ;
		  case tchar:
			(void)fprintf(output.fp, "%s", chartyp), Putl(output, 0);
			break ;
		  case tinteger:
			printid(defnams.A[(int)(dinteger)]->U.V6.lid);
			break ;
		  case treal:
			printid(defnams.A[(int)(dreal)]->U.V6.lid);
			break ;
		  case tstring:
			(void)fprintf(output.fp, "%s *", chartyp), Putl(output, 0);
			break ;
		  case ttext:
			(void)fprintf(output.fp, "text"), Putl(output, 0);
			break ;
		  case tnil:  case tset:  case terror:
			fatal(etree);
			break ;
		  case tnone:
			(void)fprintf(output.fp, "%s", voidtyp), Putl(output, 0);
			break ;
		  default:
			Caseerror(Line);
		}
		break ;
	  case nempty:
		(void)fprintf(output.fp, "%s", voidtyp), Putl(output, 0);
		break ;
	  default:
		Caseerror(Line);
	}
}

 void
etypedef(tp)
	treeptr	tp;
{
	etdef((idptr)NIL, tp);
}

 void
etype(tp)
	treeptr	tp;
{
	symptr	sp;

	while (tp != (struct S61 *)NIL) {
		sp = tp->U.V14.tidl->U.V43.tsym;
		if (sp->U.V6.lid->inref > 1)
			sp->U.V6.lid = mkrename('Y', sp->U.V6.lid);
		indent();
		(void)fprintf(output.fp, "%s", typdef), Putl(output, 0);
		etypedef(tp->U.V14.tbind);
		Putchr(tab1, output);
		printid(sp->U.V6.lid);
		Putchr(';', output),Putchr('\n', output);
		tp = tp->tnext;
	}
}

 void
evar(tp)
	treeptr	tp;
{
	treeptr	tq;
	integer	i;

	while (tp != (struct S61 *)NIL) {
		indent();
		switch (tp->tt) {
		  case nvar:  case nvalpar:  case nvarpar:
			if (tp->U.V14.tattr == aregister)
				(void)fprintf(output.fp, "%s", registr), Putl(output, 0);
			etypedef(tp->U.V14.tbind);
			break ;
		  case nparproc:  case nparfunc:
			if (tp->tt == nparproc)
				(void)fprintf(output.fp, "%s", voidtyp), Putl(output, 0);
			else
				etypedef(tp->U.V15.tpartyp);
			tq = tp->U.V15.tparid;
			(void)fprintf(output.fp, "%c(*", tab1), Putl(output, 0);
			printid(tq->U.V43.tsym->U.V6.lid);
			(void)fprintf(output.fp, ")()"), Putl(output, 0);
			goto L555;
			break ;
		  default:
			Caseerror(Line);
		}
		Putchr(tab1, output);
		tq = tp->U.V14.tidl;
		i = 1;
		do {
			if (tp->tt == nvarpar)
				Putchr('*', output);
			printid(tq->U.V43.tsym->U.V6.lid);
			tq = tq->tnext;
			if (tq != (struct S61 *)NIL) {
				if (i >= 6) {
					i = 1;
					Putchr(',', output),Putchr('\n', output);
					indent();
					Putchr(tab1, output);
				} else {
					i = i + 1;
					(void)fprintf(output.fp, ", "), Putl(output, 0);
				}
			}
		} while (!(tq == (struct S61 *)NIL));
	L555:
		Putchr(';', output),Putchr('\n', output);
		if (tp->tt == nvarpar)
			if (tp->U.V14.tbind->tt == nconfarr) {
				indent();
				etypedef(tp->U.V14.tbind->U.V22.tindtyp);
				Putchr(tab1, output);
				tq = tp->U.V14.tbind->U.V22.tcindx->U.V19.thi;
				printid(tq->U.V43.tsym->U.V6.lid);
				Putchr(';', output),Putchr('\n', output);
			}
		tp = tp->tnext;
	}
}


void estmt();

 void
ewithtype(tp)
	treeptr	tp;
{
	treeptr	tq;

	tq = typeof(tp);
	(void)fprintf(output.fp, "struct "), Putl(output, 0);
	printid(tq->U.V21.tuid);
}

 void
echoise(tp)
	treeptr	tp;
{
	treeptr	tq;
	integer	i;

	while (tp != (struct S61 *)NIL) {
		tq = tp->U.V36.tchocon;
		i = 0;
		indent();
		while (tq != (struct S61 *)NIL) {
			(void)fprintf(output.fp, "  case "), Putl(output, 0);
			(*G194_conflag) = true;
			eexpr(tq);
			(*G194_conflag) = false;
			Putchr(':', output);
			i = i + 1;
			tq = tq->tnext;
			if ((tq == (struct S61 *)NIL) || (i % 4 == 0)) {
				Putchr('\n', output);
				if (tq != (struct S61 *)NIL)
					indent();
				i = 0;
			}
		}
		increment();
		if (tp->U.V36.tchostmt->tt == nbegin)
			estmt(tp->U.V36.tchostmt->U.V24.tbegin);
		else
			estmt(tp->U.V36.tchostmt);
		indent();
		(void)fprintf(output.fp, "break ;\n"), Putl(output, 1);
		decrement();
		tp = tp->tnext;
		if (tp != (struct S61 *)NIL)
			if (tp->U.V36.tchocon == (struct S61 *)NIL)
				tp = (struct S61 *)NIL;
	}
}

 void
cenv(ip, dp)
	idptr	ip;
	declptr	dp;
{
	treeptr	tp;
	symptr	sp;
	idptr	np;
	register hashtyp	h;

	{
		register struct S60 *W54 = &*dp;

		{
			hashtyp	B55 = 0,
				B56 = hashmax - 1;

			if (B55 <= B56)
				for (h = B55; ; h++) {
					sp = W54->ddecl.A[h];
					while (sp != (struct S62 *)NIL) {
						if (sp->lt == lfield) {
							np = sp->U.V6.lid;
							tp = sp->lsymdecl->tup->tup;
							if ((tp->tup->tt == nvariant) && (tp->U.V21.tuid != (struct S59 *)NIL))
								np = mkconc('.', tp->U.V21.tuid, np);
							np = mkconc('>', ip, np);
							sp->U.V6.lid = np;
						}
						sp = sp->lnext;
					}
					if (h == B56) break;
				}
		}
	}
}

 void
eglobid(tp)
	treeptr	tp;
{
	toknidx	j;
	toknbuf	w;

	gettokn(tp->U.V43.tsym->U.V6.lid->istr, &w);
	j = 1;
	if (w.A[1 - 1] == '*')
		j = 2;
	while (w.A[j - 1] != null) {
		Putchr(w.A[j - 1], output);
		j = j + 1;
	}
}

 void
estmt(tp)
	treeptr	tp;
{
	treeptr	tq;
	idptr	locid1, locid2;
	boolean	stusd;
	char	opc1, opc2;

	while (tp != (struct S61 *)NIL) {
		switch (tp->tt) {
		  case nbegin:
			if (Member((unsigned)(tp->tup->tt), Conset[156]))
				indent();
			Putchr('{', output),Putchr('\n', output);
			increment();
			estmt(tp->U.V24.tbegin);
			decrement();
			indent();
			Putchr('}', output);
			if (tp->tup->tt != nif)
				Putchr('\n', output);
			break ;
		  case nrepeat:
			indent();
			(void)fprintf(output.fp, "do {\n"), Putl(output, 1);
			increment();
			estmt(tp->U.V33.treptstmt);
			decrement();
			indent();
			(void)fprintf(output.fp, "} while (!("), Putl(output, 0);
			eexpr(tp->U.V33.treptxp);
			(void)fprintf(output.fp, "));\n"), Putl(output, 1);
			break ;
		  case nwhile:
			indent();
			(void)fprintf(output.fp, "while ("), Putl(output, 0);
			increment();
			eexpr(tp->U.V32.twhixp);
			stusd = (*G196_setused);
			if (tp->U.V32.twhistmt->tt == nbegin) {
				decrement();
				(void)fprintf(output.fp, ") "), Putl(output, 0);
				estmt(tp->U.V32.twhistmt);
			} else {
				Putchr(')', output),Putchr('\n', output);
				estmt(tp->U.V32.twhistmt);
				decrement();
			}
			(*G196_setused) = (boolean)(stusd || (*G196_setused));
			break ;
		  case nfor:
			indent();
			if (tp->U.V34.tincr) {
				opc1 = '+';
				opc2 = '<';
			} else {
				opc1 = '-';
				opc2 = '>';
			}
			if (!lazyfor) {
				locid1 = mkvariable('B');
				locid2 = mkvariable('B');
				Putchr('{', output),Putchr('\n', output);
				increment();
				indent();
				tq = idup(tp->U.V34.tforid);
				etypedef(tq->U.V14.tbind);
				tq = typeof(tq->U.V14.tbind);
				Putchr(tab1, output);
				printid(locid1);
				(void)fprintf(output.fp, " = "), Putl(output, 0);
				eexpr(tp->U.V34.tfrom);
				Putchr(',', output),Putchr('\n', output);
				indent();
				Putchr(tab1, output);
				printid(locid2);
				(void)fprintf(output.fp, " = "), Putl(output, 0);
				eexpr(tp->U.V34.tto);
				Putchr(';', output),Putchr('\n', output);
				Putchr('\n', output);
				indent();
				(void)fprintf(output.fp, "if ("), Putl(output, 0);
				if (tq->tt == nscalar) {
					(void)fprintf(output.fp, "(integer)("), Putl(output, 0);
					printid(locid1);
					Putchr(')', output);
				} else
					printid(locid1);
				(void)fprintf(output.fp, " %c= ", opc2), Putl(output, 0);
				if (tq->tt == nscalar) {
					(void)fprintf(output.fp, "(integer)("), Putl(output, 0);
					printid(locid2);
					Putchr(')', output);
				} else
					printid(locid2);
				Putchr(')', output),Putchr('\n', output);
				increment();
				indent();
				tp->U.V34.tfrom = newid(locid1);
				tp->U.V34.tfrom->tup = tp;
			}
			(void)fprintf(output.fp, "for ("), Putl(output, 0);
			increment();
			eexpr(tp->U.V34.tforid);
			tq = typeof(tp->U.V34.tforid);
			(void)fprintf(output.fp, " = "), Putl(output, 0);
			eexpr(tp->U.V34.tfrom);
			(void)fprintf(output.fp, "; "), Putl(output, 0);
			if (lazyfor) {
				if (tq->tt == nscalar) {
					(void)fprintf(output.fp, "(integer)("), Putl(output, 0);
					eexpr(tp->U.V34.tforid);
					Putchr(')', output);
				} else
					eexpr(tp->U.V34.tforid);
				(void)fprintf(output.fp, " %c= ", opc2), Putl(output, 0);
				if (tq->tt == nscalar) {
					(void)fprintf(output.fp, "(integer)("), Putl(output, 0);
					eexpr(tp->U.V34.tto);
					Putchr(')', output);
				} else
					eexpr(tp->U.V34.tto);
			}
			(void)fprintf(output.fp, "; "), Putl(output, 0);
			eexpr(tp->U.V34.tforid);
			if (tq->tt == nscalar) {
				(void)fprintf(output.fp, " = ("), Putl(output, 0);
				eexpr(tq->tup->U.V14.tidl);
				(void)fprintf(output.fp, ")((integer)("), Putl(output, 0);
				eexpr(tp->U.V34.tforid);
				(void)fprintf(output.fp, ")%c1)", opc1), Putl(output, 0);
			} else
				(void)fprintf(output.fp, "%c%c", opc1, opc1), Putl(output, 0);
			if (!lazyfor) {
				if (tp->U.V34.tforstmt->tt != nbegin) {
					tq = mknode(nbegin);
					tq->U.V24.tbegin = tp->U.V34.tforstmt;
					tq->U.V24.tbegin->tup = tq;
					tp->U.V34.tforstmt = tq;
					tq->tup = tp;
				}
				tq = tp->U.V34.tforstmt->U.V24.tbegin;
				while (tq->tnext != (struct S61 *)NIL)
					tq = tq->tnext;
				tq->tnext = mknode(nbreak);
				tq = tq->tnext;
				tq->tup = tp->U.V34.tforstmt;
				tq->U.V29.tbrkid = tp->U.V34.tforid;
				tq->U.V29.tbrkxp = newid(locid2);
				tq->U.V29.tbrkxp->tup = tq;
			}
			if (tp->U.V34.tforstmt->tt == nbegin) {
				decrement();
				(void)fprintf(output.fp, ") "), Putl(output, 0);
				estmt(tp->U.V34.tforstmt);
			} else {
				Putchr(')', output),Putchr('\n', output);
				estmt(tp->U.V34.tforstmt);
				decrement();
			}
			if (!lazyfor) {
				decrement();
				decrement();
				indent();
				Putchr('}', output),Putchr('\n', output);
			}
			break ;
		  case nif:
			indent();
			(void)fprintf(output.fp, "if ("), Putl(output, 0);
			increment();
			eexpr(tp->U.V31.tifxp);
			stusd = (*G196_setused);
			(*G196_setused) = false;
			if (tp->U.V31.tthen->tt == nbegin) {
				decrement();
				(void)fprintf(output.fp, ") "), Putl(output, 0);
				estmt(tp->U.V31.tthen);
				if (tp->U.V31.telse != (struct S61 *)NIL)
					Putchr(space, output);
				else
					Putchr('\n', output);
			} else {
				Putchr(')', output),Putchr('\n', output);
				estmt(tp->U.V31.tthen);
				decrement();
				if (tp->U.V31.telse != (struct S61 *)NIL)
					indent();
			}
			if (tp->U.V31.telse != (struct S61 *)NIL) {
				(void)fprintf(output.fp, "else"), Putl(output, 0);
				if (tp->U.V31.telse->tt == nbegin) {
					Putchr(space, output);
					estmt(tp->U.V31.telse);
					Putchr('\n', output);
				} else {
					increment();
					Putchr('\n', output);
					estmt(tp->U.V31.telse);
					decrement();
				}
			}
			(*G196_setused) = (boolean)(stusd || (*G196_setused));
			break ;
		  case ncase:
			indent();
			(void)fprintf(output.fp, "switch ("), Putl(output, 0);
			increment();
			eexpr(tp->U.V35.tcasxp);
			(void)fprintf(output.fp, ") {\n"), Putl(output, 1);
			decrement();
			echoise(tp->U.V35.tcaslst);
			indent();
			(void)fprintf(output.fp, "  default:\n"), Putl(output, 1);
			increment();
			if (tp->U.V35.tcasother == (struct S61 *)NIL) {
				indent();
				(void)fprintf(output.fp, "Caseerror(Line);\n"), Putl(output, 1);
			} else
				estmt(tp->U.V35.tcasother);
			decrement();
			indent();
			Putchr('}', output),Putchr('\n', output);
			break ;
		  case nwith:
			indent();
			Putchr('{', output),Putchr('\n', output);
			increment();
			tq = tp->U.V37.twithvar;
			while (tq != (struct S61 *)NIL) {
				indent();
				(void)fprintf(output.fp, "%s", registr), Putl(output, 0);
				ewithtype(tq->U.V38.texpw);
				(void)fprintf(output.fp, " *"), Putl(output, 0);
				locid1 = mkvariable('W');
				printid(locid1);
				(void)fprintf(output.fp, " = "), Putl(output, 0);
				eaddr(tq->U.V38.texpw);
				Putchr(';', output),Putchr('\n', output);
				cenv(locid1, tq->U.V38.tenv);
				tq = tq->tnext;
			}
			Putchr('\n', output);
			if (tp->U.V37.twithstmt->tt == nbegin)
				estmt(tp->U.V37.twithstmt->U.V24.tbegin);
			else
				estmt(tp->U.V37.twithstmt);
			decrement();
			indent();
			Putchr('}', output),Putchr('\n', output);
			break ;
		  case ngoto:
			indent();
			if (islocal(tp->U.V26.tlabel))
				(void)fprintf(output.fp, "goto L%1d;\n", tp->U.V26.tlabel->U.V43.tsym->U.V9.lno), Putl(output, 1);
			else {
				tq = idup(tp->U.V26.tlabel);
				(void)fprintf(output.fp, "longjmp(J[%1d].jb, %1d);\n", tq->U.V13.tstat, tp->U.V26.tlabel->U.V43.tsym->U.V9.lno), Putl(output, 1);
			}
			break ;
		  case nlabstmt:
			decrement();
			indent();
			(void)fprintf(output.fp, "L%1d:\n", tp->U.V25.tlabno->U.V43.tsym->U.V9.lno), Putl(output, 1);
			increment();
			estmt(tp->U.V25.tstmt);
			break ;
		  case nassign:
			indent();
			eexpr(tp);
			Putchr(';', output),Putchr('\n', output);
			break ;
		  case ncall:
			indent();
			tq = idup(tp->U.V30.tcall);
			if ((Member((unsigned)(tq->tt), Conset[157])) && (tq->U.V13.tsubstmt != (struct S61 *)NIL))
				if (tq->U.V13.tsubstmt->tt == npredef)
					epredef(tq, tp);
				else {
					ecall(tp);
					Putchr(';', output),Putchr('\n', output);
				}
			else {
				ecall(tp);
				Putchr(';', output),Putchr('\n', output);
			}
			break ;
		  case npush:
			indent();
			eglobid(tp->U.V28.ttmp);
			(void)fprintf(output.fp, " = "), Putl(output, 0);
			eglobid(tp->U.V28.tglob);
			Putchr(';', output),Putchr('\n', output);
			indent();
			eglobid(tp->U.V28.tglob);
			(void)fprintf(output.fp, " = "), Putl(output, 0);
			if (tp->U.V28.tloc->tt == nid) {
				tq = idup(tp->U.V28.tloc);
				if (Member((unsigned)(tq->tt), Conset[158]))
					printid(tp->U.V28.tloc->U.V43.tsym->U.V6.lid);
				else
					eaddr(tp->U.V28.tloc);
			} else
				eaddr(tp->U.V28.tloc);
			Putchr(';', output),Putchr('\n', output);
			break ;
		  case npop:
			indent();
			eglobid(tp->U.V28.tglob);
			(void)fprintf(output.fp, " = "), Putl(output, 0);
			eglobid(tp->U.V28.ttmp);
			Putchr(';', output),Putchr('\n', output);
			break ;
		  case nbreak:
			indent();
			(void)fprintf(output.fp, "if ("), Putl(output, 0);
			eexpr(tp->U.V29.tbrkid);
			(void)fprintf(output.fp, " == "), Putl(output, 0);
			eexpr(tp->U.V29.tbrkxp);
			(void)fprintf(output.fp, ") break;\n"), Putl(output, 1);
			break ;
		  case nempty:
			if (!(Member((unsigned)(tp->tup->tt), Conset[159]))) {
				indent();
				Putchr(';', output),Putchr('\n', output);
			}
			break ;
		  default:
			Caseerror(Line);
		}
		if ((*G196_setused) && (Member((unsigned)(tp->tup->tt), Conset[160]))) {
			indent();
			(void)fprintf(output.fp, "Claimset();\n"), Putl(output, 1);
			(*G196_setused) = false;
		}
		tp = tp->tnext;
	}
}

 void
elabel(tp)
	treeptr	tp;
{
	treeptr	tq;
	integer	i;

	i = 0;
	tq = tp->U.V13.tsublab;
	while (tq != (struct S61 *)NIL) {
		if (tq->U.V43.tsym->U.V9.lgo)
			i = i + 1;
		tq = tq->tnext;
	}
	if (i == 1) {
		tq = tp->U.V13.tsublab;
		while (!tq->U.V43.tsym->U.V9.lgo)
			tq = tq->tnext;
		indent();
		(void)fprintf(output.fp, "if (setjmp(J[%1d].jb))\n", tp->U.V13.tstat), Putl(output, 1);
		(void)fprintf(output.fp, "%cgoto L%1d;\n", tab1, tq->U.V43.tsym->U.V9.lno), Putl(output, 1);
	} else
		if (i > 1) {
			indent();
			(void)fprintf(output.fp, "switch (setjmp(J[%1d].jb)) {\n", tp->U.V13.tstat), Putl(output, 1);
			indent();
			(void)fprintf(output.fp, "  case 0:\n"), Putl(output, 1);
			indent();
			(void)fprintf(output.fp, "%cbreak\n", tab1), Putl(output, 1);
			tq = tp->U.V13.tsublab;
			while (tq != (struct S61 *)NIL) {
				if (tq->U.V43.tsym->U.V9.lgo) {
					indent();
					(void)fprintf(output.fp, "  case %1d:\n", tq->U.V43.tsym->U.V9.lno), Putl(output, 1);
					indent();
					(void)fprintf(output.fp, "%cgoto L%1d;\n", tab1, tq->U.V43.tsym->U.V9.lno), Putl(output, 1);
				}
				tq = tq->tnext;
			}
			indent();
			(void)fprintf(output.fp, "  default:\n"), Putl(output, 1);
			indent();
			(void)fprintf(output.fp, "%cCaseerror(Line)\n", tab1), Putl(output, 1);
			indent();
			Putchr('}', output),Putchr('\n', output);
		}
}

 void
econf(tp)
	treeptr	tp;
{
	treeptr	tq;

	while (tp != (struct S61 *)NIL) {
		if (tp->tt == nvarpar)
			if (tp->U.V14.tbind->tt == nconfarr) {
				indent();
				etypedef(tp->U.V14.tbind->U.V22.tindtyp);
				Putchr(tab1, output);
				tq = tp->U.V14.tbind->U.V22.tcindx->U.V19.tlo;
				printid(tq->U.V43.tsym->U.V6.lid);
				(void)fprintf(output.fp, " = ("), Putl(output, 0);
				etypedef(tp->U.V14.tbind->U.V22.tindtyp);
				(void)fprintf(output.fp, ")0;\n"), Putl(output, 1);
			}
		tp = tp->tnext;
	}
}

 void
esubr(tp)
	treeptr	tp;
{
	treeptr	tq, ti;

	while (tp != (struct S61 *)NIL) {
		if (tp->U.V13.tsubsub != (struct S61 *)NIL) {
			etypedef(tp->U.V13.tfuntyp);
			Putchr(space, output);
			printid(tp->U.V13.tsubid->U.V43.tsym->U.V6.lid);
			(void)fprintf(output.fp, "();\n"), Putl(output, 1);
			Putchr('\n', output);
			esubr(tp->U.V13.tsubsub);
		}
		if (tp->U.V13.tsubstmt == (struct S61 *)NIL) {
			if (tp->U.V13.tsubid->U.V43.tsym->lsymdecl->tup == tp)
				(void)fprintf(output.fp, "%s", xtern), Putl(output, 0);
			etypedef(tp->U.V13.tfuntyp);
			Putchr(space, output);
			printid(tp->U.V13.tsubid->U.V43.tsym->U.V6.lid);
			(void)fprintf(output.fp, "();\n"), Putl(output, 1);
			goto L999;
		}
		Putchr(space, output);
		etypedef(tp->U.V13.tfuntyp);
		Putchr('\n', output);
		printid(tp->U.V13.tsubid->U.V43.tsym->U.V6.lid);
		Putchr('(', output);
		tq = tp->U.V13.tsubpar;
		while (tq != (struct S61 *)NIL) {
			switch (tq->tt) {
			  case nvarpar:  case nvalpar:
				ti = tq->U.V14.tidl;
				while (ti != (struct S61 *)NIL) {
					printid(ti->U.V43.tsym->U.V6.lid);
					ti = ti->tnext;
					if (ti != (struct S61 *)NIL)
						(void)fprintf(output.fp, ", "), Putl(output, 0);
				}
				if (tq->U.V14.tbind->tt == nconfarr) {
					ti = tq->U.V14.tbind->U.V22.tcindx->U.V19.thi;
					(void)fprintf(output.fp, ", "), Putl(output, 0);
					printid(ti->U.V43.tsym->U.V6.lid);
				}
				break ;
			  case nparproc:  case nparfunc:
				ti = tq->U.V15.tparid;
				printid(ti->U.V43.tsym->U.V6.lid);
				break ;
			  default:
				Caseerror(Line);
			}
			tq = tq->tnext;
			if (tq != (struct S61 *)NIL)
				(void)fprintf(output.fp, ", "), Putl(output, 0);
		}
		Putchr(')', output),Putchr('\n', output);
		increment();
		evar(tp->U.V13.tsubpar);
		Putchr('{', output),Putchr('\n', output);
		econf(tp->U.V13.tsubpar);
		econst(tp->U.V13.tsubconst);
		etype(tp->U.V13.tsubtype);
		evar(tp->U.V13.tsubvar);
		if ((tp->U.V13.tsubconst != (struct S61 *)NIL) || (tp->U.V13.tsubtype != (struct S61 *)NIL) || (tp->U.V13.tsubvar != (struct S61 *)NIL))
			Putchr('\n', output);
		elabel(tp);
		estmt(tp->U.V13.tsubstmt);
		if (tp->tt == nfunc) {
			indent();
			(void)fprintf(output.fp, "return "), Putl(output, 0);
			printid(tp->U.V13.tsubvar->U.V14.tidl->U.V43.tsym->U.V6.lid);
			Putchr(';', output),Putchr('\n', output);
		}
		decrement();
		Putchr('}', output),Putchr('\n', output);
	L999:
		Putchr('\n', output);
		tp = tp->tnext;
	}
}

 boolean
use(d)
	predefs	d;
{
	register boolean	R174;

	R174 = defnams.A[(int)(d)]->U.V6.lused;
	return R174;
}

void eprogram();

 void
capital(sp)
	symptr	sp;
{
	toknbuf	tb;

	if (sp->U.V6.lid->inref > 1) {
		gettokn(sp->U.V6.lid->istr, &tb);
		tb.A[1 - 1] = uppercase(tb.A[1 - 1]);
		sp->U.V6.lid = saveid(&tb);
	}
}

 void
etextdef()
{
	treeptr	tq;

	(void)fprintf(output.fp, "typedef "), Putl(output, 0);
	tq = mknode(nfileof);
	tq->U.V18.tof = typnods.A[(int)(tchar)];
	etypedef(tq);
	(void)fprintf(output.fp, "%ctext;\n", tab1), Putl(output, 1);
}

 void
eprogram(tp)
	treeptr	tp;
{
	if (tp->U.V13.tsubid != (struct S61 *)NIL) {
		(void)fprintf(output.fp, "/*\n"), Putl(output, 1);
		(void)fprintf(output.fp, "**	Code derived from program "), Putl(output, 0);
		printid(tp->U.V13.tsubid->U.V43.tsym->U.V6.lid);
		Putchr('\n', output);
		(void)fprintf(output.fp, "*/\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s%cexit();\n", xtern, voidtyp, tab1), Putl(output, 1);
	}
	if (usecase || usesets || use(dinput) || use(doutput) || use(dwrite) || use(dwriteln) || use(dmessage) || use(deof) || use(deoln) || use(dflush) || use(dpage) || use(dread) || use(dreadln) || use(dclose) || use(dreset) || use(drewrite) || use(dget) || use(dput)) {
		(void)fprintf(output.fp, "/*\n"), Putl(output, 1);
		(void)fprintf(output.fp, "**	Definitions for i/o\n"), Putl(output, 1);
		(void)fprintf(output.fp, "*/\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%s<stdio.h>\n", C24_include), Putl(output, 1);
	}
	if (use(dinput) || use(doutput) || use(dtext)) {
		etextdef();
		if (use(dinput)) {
			if (tp->U.V13.tsubid == (struct S61 *)NIL)
				(void)fprintf(output.fp, "%s", xtern), Putl(output, 0);
			(void)fprintf(output.fp, "text%c", tab1), Putl(output, 0);
			printid(defnams.A[(int)(dinput)]->U.V6.lid);
			if (tp->U.V13.tsubid != (struct S61 *)NIL)
				(void)fprintf(output.fp, " = { stdin, 0, 0 }"), Putl(output, 0);
			Putchr(';', output),Putchr('\n', output);
		}
		if (use(doutput)) {
			if (tp->U.V13.tsubid == (struct S61 *)NIL)
				(void)fprintf(output.fp, "%s", xtern), Putl(output, 0);
			(void)fprintf(output.fp, "text%c", tab1), Putl(output, 0);
			printid(defnams.A[(int)(doutput)]->U.V6.lid);
			if (tp->U.V13.tsubid != (struct S61 *)NIL)
				(void)fprintf(output.fp, " = { stdout, 0, 0 }"), Putl(output, 0);
			Putchr(';', output),Putchr('\n', output);
		}
	}
	if (use(dinput) || use(dget) || use(dread) || use(dreadln) || use(deof) || use(deoln) || use(dreset) || use(drewrite)) {
		(void)fprintf(output.fp, "%sFread(x, f) fread((char *)&x, sizeof(x), 1, f)\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%sGet(f) Fread((f).buf, (f).fp)\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%sGetx(f) (f).init = 1, (f).eoln = (((f).buf = fgetc((f).fp)) == %s) ? (((f).buf = %s), 1) : 0\n", C4_define, nlchr, spchr), Putl(output, 1);
		(void)fprintf(output.fp, "%sGetchr(f) (f).buf, Getx(f)\n", C4_define), Putl(output, 1);
	}
	if (use(dread) || use(dreadln)) {
		(void)fprintf(output.fp, "%sFILE%c*Tmpfil;\n", C50_static, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%slong%cTmplng;\n", C50_static, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sdouble%cTmpdbl;\n", C50_static, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sFscan(f) (f).init ? ungetc((f).buf, (f).fp) : 0, Tmpfil = (f).fp\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%sScan(p, a) Scanck(fscanf(Tmpfil, p, a))\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%s%cScanck();\n", voidtyp, tab1), Putl(output, 1);
		if (use(dreadln))
			(void)fprintf(output.fp, "%s%cGetl();\n", voidtyp, tab1), Putl(output, 1);
	}
	if (use(deoln))
		(void)fprintf(output.fp, "%sEoln(f) ((f).eoln ? true : false)\n", C4_define), Putl(output, 1);
	if (use(deof))
		(void)fprintf(output.fp, "%sEof(f) ((((f).init == 0) ? (Get(f)) : 0, ((f).eof ? 1 : feof((f).fp))) ? true : false)\n", C4_define), Putl(output, 1);
	if (use(doutput) || use(dput) || use(dwrite) || use(dwriteln) || use(dreset) || use(drewrite) || use(dclose)) {
		(void)fprintf(output.fp, "%sFwrite(x, f) fwrite((char *)&x, sizeof(x), 1, f)\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%sPut(f) Fwrite((f).buf, (f).fp)\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%sPutx(f) (f).eoln = ((f).buf == %s), %sfputc((f).buf, (f).fp)\n", C4_define, nlchr, voidcast), Putl(output, 1);
		(void)fprintf(output.fp, "%sPutchr(c, f) (f).buf = (c), Putx(f)\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%sPutl(f, v) (f).eoln = v\n", C4_define), Putl(output, 1);
	}
	if (use(dreset) || use(drewrite) || use(dclose))
		(void)fprintf(output.fp, "%sFinish(f) ((f).out && !(f).eoln) ? (Putchr(%s, f), 0) : 0, rewind((f).fp)\n", C4_define, nlchr), Putl(output, 1);
	if (use(dclose)) {
		(void)fprintf(output.fp, "%sClose(f) (f).init = ((f).init ? (fclose((f).fp), 0) : 0), (f).fp = NULL\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%sClosex(f) (f).init = ((f).init ? (Finish(f), fclose((f).fp), 0) : 0), (f).fp = NULL\n", C4_define), Putl(output, 1);
	}
	if (use(dreset)) {
		(void)fprintf(output.fp, "%sREADONLY\n", ifdef), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s%cRmode[] = \"r\";\n", C50_static, chartyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s\n", elsif), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s%cRmode[] = \"r+\";\n", C50_static, chartyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s\n", endif), Putl(output, 1);
		(void)fprintf(output.fp, "%sReset(f, n) (f).init = (f).init ? rewind((f).fp) : (((f).fp = Fopen(n, Rmode)), 1), (f).eof = (f).out = 0, Get(f)\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%sResetx(f, n) (f).init = (f).init ? (Finish(f)) : (((f).fp = Fopen(n, Rmode)), 1), (f).eof = (f).out = 0, Getx(f)\n", C4_define), Putl(output, 1);
		usefopn = true;
	}
	if (use(drewrite)) {
		(void)fprintf(output.fp, "%sWRITEONLY\n", ifdef), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s%cWmode[] = \"w\";\n", C50_static, chartyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s\n", elsif), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s%cWmode[] = \"w+\";\n", C50_static, chartyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s\n", endif), Putl(output, 1);
		(void)fprintf(output.fp, "%sRewrite(f, n) (f).init = (f).init ? rewind((f).fp) : (((f).fp = Fopen(n, Wmode)), 1), (f).out = (f).eof = 1\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%sRewritex(f, n) (f).init = (f).init ? (Finish(f)) : (((f).fp = Fopen(n, Wmode)), 1), (f).out = (f).eof = (f).eoln = 1\n", C4_define), Putl(output, 1);
		usefopn = true;
	}
	if (usefopn) {
		(void)fprintf(output.fp, "FILE	*Fopen();\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%sMAXFILENAME 256\n", C4_define), Putl(output, 1);
	}
	if (usecase || usejmps) {
		(void)fprintf(output.fp, "/*\n"), Putl(output, 1);
		(void)fprintf(output.fp, "**	Definitions for case-statements\n"), Putl(output, 1);
		(void)fprintf(output.fp, "**	and for non-local gotos\n"), Putl(output, 1);
		(void)fprintf(output.fp, "*/\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%sLine __LINE__\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%s%cCaseerror();\n", voidtyp, tab1), Putl(output, 1);
	}
	if (usejmps) {
		(void)fprintf(output.fp, "%s<setjmp.h>\n", C24_include), Putl(output, 1);
		(void)fprintf(output.fp, "%sstruct Jb { jmp_buf%cjb; } J[%1d];\n", C50_static, tab1, (maxlevel + 1)), Putl(output, 1);
	}
	if (use(dinteger) || use(dmaxint) || use(dboolean) || use(dfalse) || use(dtrue) || use(deof) || use(deoln) || use(dexp) || use(dln) || use(dsqr) || use(dsin) || use(dcos) || use(dtan) || use(darctan) || use(dsqrt) || use(dreal)) {
		(void)fprintf(output.fp, "/*\n"), Putl(output, 1);
		(void)fprintf(output.fp, "**	Definitions for standard types\n"), Putl(output, 1);
		(void)fprintf(output.fp, "*/\n"), Putl(output, 1);
	}
	if (usecomp) {
		(void)fprintf(output.fp, "%s%s strncmp();\n", xtern, inttyp), Putl(output, 1);
		(void)fprintf(output.fp, "%sCmpstr(x, y) strncmp((x), (y), sizeof(x))\n", C4_define), Putl(output, 1);
	}
	if (use(dboolean) || use(dfalse) || use(dtrue) || use(deof) || use(deoln) || usesets) {
		capital(defnams.A[(int)(dboolean)]);
		(void)fprintf(output.fp, "%s%s%c", typdef, chartyp, tab1), Putl(output, 0);
		printid(defnams.A[(int)(dboolean)]->U.V6.lid);
		Putchr(';', output),Putchr('\n', output);
		capital(defnams.A[(int)(dfalse)]);
		(void)fprintf(output.fp, "%s", C4_define), Putl(output, 0);
		printid(defnams.A[(int)(dfalse)]->U.V6.lid);
		(void)fprintf(output.fp, " ("), Putl(output, 0);
		printid(defnams.A[(int)(dboolean)]->U.V6.lid);
		(void)fprintf(output.fp, ")0\n"), Putl(output, 1);
		capital(defnams.A[(int)(dtrue)]);
		(void)fprintf(output.fp, "%s", C4_define), Putl(output, 0);
		printid(defnams.A[(int)(dtrue)]->U.V6.lid);
		(void)fprintf(output.fp, " ("), Putl(output, 0);
		printid(defnams.A[(int)(dboolean)]->U.V6.lid);
		(void)fprintf(output.fp, ")1\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s%c*Bools[];\n", xtern, chartyp, tab1), Putl(output, 1);
	}
	capital(defnams.A[(int)(dinteger)]);
	if (use(dinteger)) {
		(void)fprintf(output.fp, "%s%s%c", typdef, inttyp, tab1), Putl(output, 0);
		printid(defnams.A[(int)(dinteger)]->U.V6.lid);
		Putchr(';', output),Putchr('\n', output);
	}
	if (use(dmaxint))
		(void)fprintf(output.fp, "%smaxint%c%1d\n", C4_define, tab1, maxint), Putl(output, 1);
	capital(defnams.A[(int)(dreal)]);
	if (use(dreal)) {
		(void)fprintf(output.fp, "%s%s%c", typdef, realtyp, tab1), Putl(output, 0);
		printid(defnams.A[(int)(dreal)]->U.V6.lid);
		Putchr(';', output),Putchr('\n', output);
	}
	if (use(dexp))
		(void)fprintf(output.fp, "%s%s exp();\n", xtern, doubletyp), Putl(output, 1);
	if (use(dln))
		(void)fprintf(output.fp, "%s%s log();\n", xtern, doubletyp), Putl(output, 1);
	if (use(dsqr))
		(void)fprintf(output.fp, "%s%s pow();\n", xtern, doubletyp), Putl(output, 1);
	if (use(dsin))
		(void)fprintf(output.fp, "%s%s sin();\n", xtern, doubletyp), Putl(output, 1);
	if (use(dcos))
		(void)fprintf(output.fp, "%s%s cos();\n", xtern, doubletyp), Putl(output, 1);
	if (use(dtan))
		(void)fprintf(output.fp, "%s%s tan();\n", xtern, doubletyp), Putl(output, 1);
	if (use(darctan))
		(void)fprintf(output.fp, "%s%s atan();\n", xtern, doubletyp), Putl(output, 1);
	if (use(dsqrt))
		(void)fprintf(output.fp, "%s%s sqrt();\n", xtern, doubletyp), Putl(output, 1);
	if (use(dabs) && use(dreal))
		(void)fprintf(output.fp, "%s%s fabs();\n", xtern, doubletyp), Putl(output, 1);
	if (use(dhalt))
		(void)fprintf(output.fp, "%s%s abort();\n", xtern, voidtyp), Putl(output, 1);
	if (use(dnew) || usenilp) {
		(void)fprintf(output.fp, "/*\n"), Putl(output, 1);
		(void)fprintf(output.fp, "**	Definitions for pointers\n"), Putl(output, 1);
		(void)fprintf(output.fp, "*/\n"), Putl(output, 1);
	}
	if (use(dnew)) {
		(void)fprintf(output.fp, "%sUnionoffs\n", ifndef), Putl(output, 1);
		(void)fprintf(output.fp, "%sUnionoffs(p, m) (((long)(&(p)->m))-((long)(p)))\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%s\n", endif), Putl(output, 1);
	}
	if (usenilp)
		(void)fprintf(output.fp, "%sNIL 0\n", C4_define), Putl(output, 1);
	if (use(dnew))
		(void)fprintf(output.fp, "%s%s *malloc();\n", xtern, chartyp), Putl(output, 1);
	if (use(ddispose))
		(void)fprintf(output.fp, "%s%s free();\n", xtern, voidtyp), Putl(output, 1);
	if (usesets) {
		(void)fprintf(output.fp, "/*\n"), Putl(output, 1);
		(void)fprintf(output.fp, "**	Definitions for set-operations\n"), Putl(output, 1);
		(void)fprintf(output.fp, "*/\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%sClaimset() %sCurrset(0, (%s)0)\n", C4_define, voidcast, setptyp), Putl(output, 1);
		(void)fprintf(output.fp, "%sNewset() Currset(1, (%s)0)\n", C4_define, setptyp), Putl(output, 1);
		(void)fprintf(output.fp, "%sSaveset(s) Currset(2, s)\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%ssetbits %1d\n", C4_define, C37_setbits), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s%c%s;\n", typdef, wordtype, tab1, setwtyp), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s *%c%s;\n", typdef, setwtyp, tab1, setptyp), Putl(output, 1);
		printid(defnams.A[(int)(dboolean)]->U.V6.lid);
		(void)fprintf(output.fp, "%cMember(), Le(), Ge(), Eq(), Ne();\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s%cUnion(), Diff();\n", setptyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s%cInsmem(), Mksubr();\n", setptyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s%cCurrset(), Inter();\n", setptyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s%cTmpset;\n", C50_static, setptyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s%cConset[];\n", xtern, setptyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s%cSetncpy();\n", voidtyp, tab1), Putl(output, 1);
	}
	(void)fprintf(output.fp, "%s%s *strncpy();\n", xtern, chartyp), Putl(output, 1);
	if (use(dargc) || use(dargv)) {
		(void)fprintf(output.fp, "/*\n"), Putl(output, 1);
		(void)fprintf(output.fp, "**	Definitions for argv-operations\n"), Putl(output, 1);
		(void)fprintf(output.fp, "*/\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%s%cargc;\n", inttyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s%c**argv;\n", chartyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, " void\n"), Putl(output, 1);
		(void)fprintf(output.fp, "Argvgt(n, cp, l)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%s%cn;\n", inttyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s%cl;\n", registr, inttyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s%c*cp;\n", registr, chartyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s%c*sp;\n", tab1, registr, chartyp, tab1), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%cfor (sp = argv[n]; l > 0 && *sp; l--)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s*cp++ = *sp++;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (l-- > 0)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s*cp++ = %s;\n", tab2, spchr), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if ((tp->U.V13.tsubconst != (struct S61 *)NIL) || (tp->U.V13.tsubtype != (struct S61 *)NIL) || (tp->U.V13.tsubvar != (struct S61 *)NIL) || (tp->U.V13.tsubsub != (struct S61 *)NIL)) {
		(void)fprintf(output.fp, "/*\n"), Putl(output, 1);
		(void)fprintf(output.fp, "**	Start of program definitions\n"), Putl(output, 1);
		(void)fprintf(output.fp, "*/\n"), Putl(output, 1);
	}
	econst(tp->U.V13.tsubconst);
	etype(tp->U.V13.tsubtype);
	evar(tp->U.V13.tsubvar);
	if (tp->U.V13.tsubsub != (struct S61 *)NIL)
		Putchr('\n', output);
	esubr(tp->U.V13.tsubsub);
	if (tp->U.V13.tsubid != (struct S61 *)NIL) {
		(void)fprintf(output.fp, "/*\n"), Putl(output, 1);
		(void)fprintf(output.fp, "**	Start of program code\n"), Putl(output, 1);
		(void)fprintf(output.fp, "*/\n"), Putl(output, 1);
		if (use(dargc) || use(dargv)) {
			(void)fprintf(output.fp, "main(_ac, _av)\n"), Putl(output, 1);
			(void)fprintf(output.fp, "%s%c_ac;\n", inttyp, tab1), Putl(output, 1);
			(void)fprintf(output.fp, "%s%c*_av[];\n", chartyp, tab1), Putl(output, 1);
			Putchr('{', output),Putchr('\n', output);
			Putchr('\n', output);
			(void)fprintf(output.fp, "%cargc = _ac;\n", tab1), Putl(output, 1);
			(void)fprintf(output.fp, "%cargv = _av;\n", tab1), Putl(output, 1);
		} else {
			(void)fprintf(output.fp, "main()\n"), Putl(output, 1);
			Putchr('{', output),Putchr('\n', output);
		}
		increment();
		elabel(tp);
		estmt(tp->U.V13.tsubstmt);
		indent();
		(void)fprintf(output.fp, "exit(0);\n"), Putl(output, 1);
		decrement();
		Putchr('}', output),Putchr('\n', output);
		(void)fprintf(output.fp, "/*\n"), Putl(output, 1);
		(void)fprintf(output.fp, "**	End of program code\n"), Putl(output, 1);
		(void)fprintf(output.fp, "*/\n"), Putl(output, 1);
	}
}

void econset();

 integer
size(tp)
	treeptr	tp;
{
	register integer	R175;
	integer	r, x;

	r = 0;
	while (tp != (struct S61 *)NIL) {
		if (tp->tt == nrange)
			x = cvalof(tp->U.V41.texpr);
		else
			if (tp->tt == nempty)
				x = 0;
			else
				x = cvalof(tp);
		if (x > r)
			r = x;
		tp = tp->tnext;
	}
	R175 = csetwords(r + 1);
	return R175;
}

void ebits();

 void
eword(s)
	bitset	s;
{
# define bitshex 4
	integer	n;
	register integer	i;
	unsigned char	x;

	n = 0;
	while (n <= C37_setbits)
		n = n + bitshex;
	n = n - bitshex;
	while (n >= 0) {
		x = 0;
		{
			integer	B57 = 0,
				B58 = bitshex - 1;

			if (B57 <= B58)
				for (i = B57; ; i++) {
					if (Member((unsigned)((n + i)), s.S))
						switch (i) {
						  case 0:
							x = x + 1;
							break ;
						  case 1:
							x = x + 2;
							break ;
						  case 2:
							x = x + 4;
							break ;
						  case 3:
							x = x + 8;
							break ;
						  default:
							Caseerror(Line);
						}
					if (i == B58) break;
				}
		}
		Putchr(hexdig.A[x], output);
		n = n - bitshex;
	}
}

 void
ebits(tp)
	treeptr	tp;
{
	typedef struct { bitset	A[maxsetrange + 1]; }	T74;
	T74	sets;
	integer	s;
	register integer	m;
	register integer	n;

	s = size(tp);
	{
		integer	B59 = 0,
			B60 = s - 1;

		if (B59 <= B60)
			for (n = B59; ; n++) {
				Setncpy(sets.A[n].S, Conset[161], sizeof(sets.A[n].S));
				if (n == B60) break;
			}
	}
	while (tp != (struct S61 *)NIL) {
		if (tp->tt == nrange)
			{
				integer	B61 = cvalof(tp->U.V41.texpl),
					B62 = cvalof(tp->U.V41.texpr);

				if (B61 <= B62)
					for (m = B61; ; m++) {
						n = m / (C37_setbits + 1);
						Setncpy(sets.A[n].S, Union(sets.A[n].S, Saveset((Tmpset = Newset(), (void)Insmem((unsigned)(m % (C37_setbits + 1)), Tmpset), Tmpset))), sizeof(sets.A[n].S));
						Claimset();
						if (m == B62) break;
					}
			}
		else
			if (tp->tt != nempty) {
				m = cvalof(tp);
				n = m / (C37_setbits + 1);
				Setncpy(sets.A[n].S, Union(sets.A[n].S, Saveset((Tmpset = Newset(), (void)Insmem((unsigned)(m % (C37_setbits + 1)), Tmpset), Tmpset))), sizeof(sets.A[n].S));
				Claimset();
			}
		tp = tp->tnext;
	}
	(void)fprintf(output.fp, "%c%1d", tab1, s), Putl(output, 0);
	{
		integer	B63 = 0,
			B64 = s - 1;

		if (B63 <= B64)
			for (n = B63; ; n++) {
				Putchr(',', output);
				if (n % 6 == 0)
					Putchr('\n', output);
				(void)fprintf(output.fp, "%c0x", tab1), Putl(output, 0);
				eword(sets.A[n]);
				if (n == B64) break;
			}
	}
	Putchr('\n', output);
}

 void
econset(tp, len)
	treeptr	tp;
	integer	len;
{
	register integer	i;

	i = 0;
	while (tp != (struct S61 *)NIL) {
		(void)fprintf(output.fp, "%s%s%cQ%1d[] = {\n", C50_static, setwtyp, tab1, i), Putl(output, 1);
		ebits(tp->U.V42.texps);
		(void)fprintf(output.fp, "};\n"), Putl(output, 1);
		i = i + 1;
		tp = tp->tnext;
	}
	(void)fprintf(output.fp, "%s%s%c*Conset[] = {\n", C50_static, setwtyp, tab1), Putl(output, 1);
	{
		integer	B65 = len - 1,
			B66 = 1;

		if (B65 >= B66)
			for (i = B65; ; i--) {
				(void)fprintf(output.fp, "%cQ%1d,", tab1, i), Putl(output, 0);
				if (i % 6 == 5)
					Putchr('\n', output);
				if (i == B66) break;
			}
	}
	(void)fprintf(output.fp, "%cQ0\n", tab1), Putl(output, 1);
	(void)fprintf(output.fp, "};\n"), Putl(output, 1);
}
