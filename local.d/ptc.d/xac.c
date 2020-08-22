#include "globals.h"

 void
extract(tp)
	treeptr	tp;
{
	treeptr	vp;

	while (tp != (struct S61 *)NIL) {
		tp->U.V13.tsubtype = xtrenum(tp->U.V13.tsubtype, tp);
		tp->U.V13.tsubvar = xtrenum(tp->U.V13.tsubvar, tp);
		vp = tp->U.V13.tsubvar;
		while (vp != (struct S61 *)NIL) {
			if (Member((unsigned)(vp->U.V14.tbind->tt), Conset[132]))
				vp->U.V14.tbind = xtrit(vp->U.V14.tbind, tp, true);
			vp = vp->tnext;
		}
		extract(tp->U.V13.tsubsub);
		tp = tp->tnext;
	}
}

void global();

 void
markdecl(xp)
	treeptr	xp;
{
	while (xp != (struct S61 *)NIL) {
		switch (xp->tt) {
		  case nid:
			xp->U.V43.tsym->U.V6.lused = false;
			break ;
		  case nconst:
			markdecl(xp->U.V14.tidl);
			break ;
		  case ntype:  case nvar:  case nvalpar:  case nvarpar:
		  case nfield:
			markdecl(xp->U.V14.tidl);
			if (xp->U.V14.tbind->tt != nid)
				markdecl(xp->U.V14.tbind);
			break ;
		  case nscalar:
			markdecl(xp->U.V17.tscalid);
			break ;
		  case nrecord:
			markdecl(xp->U.V21.tflist);
			markdecl(xp->U.V21.tvlist);
			break ;
		  case nvariant:
			markdecl(xp->U.V20.tvrnt);
			break ;
		  case nconfarr:
			if (xp->U.V22.tcelem->tt != nid)
				markdecl(xp->U.V22.tcelem);
			break ;
		  case narray:
			if (xp->U.V23.taelem->tt != nid)
				markdecl(xp->U.V23.taelem);
			break ;
		  case nsetof:  case nfileof:
			if (xp->U.V18.tof->tt != nid)
				markdecl(xp->U.V18.tof);
			break ;
		  case nparproc:  case nparfunc:
			markdecl(xp->U.V15.tparid);
			break ;
		  case nptr:  case nsubrange:
			break ;
		  default:
			Caseerror(Line);
		}
		xp = xp->tnext;
	}
}

 treeptr
movedecl(tp)
	treeptr	tp;
{
	register treeptr	R166;
	treeptr	ip, np;
	symptr	sp;
	boolean	move;

	if (tp != (struct S61 *)NIL) {
		move = false;
		switch (tp->tt) {
		  case nconst:  case ntype:
			ip = tp->U.V14.tidl;
			break ;
		  default:
			Caseerror(Line);
		}
		while (ip != (struct S61 *)NIL) {
			if (ip->U.V43.tsym->U.V6.lused) {
				move = true;
				sp = ip->U.V43.tsym;
				if (sp->U.V6.lid->inref > 1) {
					sp->U.V6.lid = mkrename('M', sp->U.V6.lid);
					sp->U.V6.lid->inref = sp->U.V6.lid->inref - 1;
				}
				ip = (struct S61 *)NIL;
			} else
				ip = ip->tnext;
		}
		if (move) {
			np = tp->tnext;
			tp->tnext = (struct S61 *)NIL;
			ip = tp;
			while (ip->tt != npgm)
				ip = ip->tup;
			tp->tup = ip;
			switch (tp->tt) {
			  case nconst:
				if (ip->U.V13.tsubconst == (struct S61 *)NIL)
					ip->U.V13.tsubconst = tp;
				else {
					ip = ip->U.V13.tsubconst;
					while (ip->tnext != (struct S61 *)NIL)
						ip = ip->tnext;
					ip->tnext = tp;
				}
				break ;
			  case ntype:
				if (ip->U.V13.tsubtype == (struct S61 *)NIL)
					ip->U.V13.tsubtype = tp;
				else {
					ip = ip->U.V13.tsubtype;
					while (ip->tnext != (struct S61 *)NIL)
						ip = ip->tnext;
					ip->tnext = tp;
				}
				break ;
			  default:
				Caseerror(Line);
			}
			tp = movedecl(np);
		} else
			tp->tnext = movedecl(tp->tnext);
	}
	R166 = tp;
	return R166;
}

void movevars();

 void
moveglob(tp, dp)
	treeptr	tp, dp;
{
	while (tp->tt != npgm)
		tp = tp->tup;
	dp->tup = tp;
	dp->tnext = tp->U.V13.tsubvar;
	tp->U.V13.tsubvar = dp;
}

 treeptr
stackop(decl, glob, loc)
	treeptr	decl, glob, loc;
{
	register treeptr	R167;
	treeptr	op, ip, dp, tp;

	ip = newid(mkvariable('F'));
	switch ((*G189_vp)->tt) {
	  case nvarpar:  case nvalpar:  case nvar:
		dp = mknode(nvarpar);
		dp->U.V14.tattr = areference;
		dp->U.V14.tidl = ip;
		dp->U.V14.tbind = decl->U.V14.tbind;
		break ;
	  case nparproc:  case nparfunc:
		dp = mknode((*G189_vp)->tt);
		dp->U.V15.tparid = ip;
		dp->U.V15.tparparm = (struct S61 *)NIL;
		dp->U.V15.tpartyp = (*G189_vp)->U.V15.tpartyp;
		break ;
	  default:
		Caseerror(Line);
	}
	ip->tup = dp;
	tp = decl;
	while (!(Member((unsigned)(tp->tt), Conset[133])))
		tp = tp->tup;
	dp->tup = tp;
	if (tp->U.V13.tsubvar == (struct S61 *)NIL)
		tp->U.V13.tsubvar = dp;
	else {
		tp = tp->U.V13.tsubvar;
		while (tp->tnext != (struct S61 *)NIL)
			tp = tp->tnext;
		tp->tnext = dp;
	}
	dp->tnext = (struct S61 *)NIL;
	op = mknode(npush);
	op->U.V28.tglob = glob;
	op->U.V28.tloc = loc;
	op->U.V28.ttmp = ip;
	R167 = op;
	return R167;
}

 void
addcode(tp, push)
	treeptr	tp, push;
{
	treeptr	pop;

	pop = mknode(npop);
	pop->U.V28.tglob = push->U.V28.tglob;
	pop->U.V28.ttmp = push->U.V28.ttmp;
	pop->U.V28.tloc = (struct S61 *)NIL;
	push->tnext = tp->U.V13.tsubstmt;
	tp->U.V13.tsubstmt = push;
	push->tup = tp;
	while (push->tnext != (struct S61 *)NIL)
		push = push->tnext;
	push->tnext = pop;
	pop->tup = tp;
}

 void
movevars(tp, vp)
	treeptr	tp, vp;
{
	treeptr	ep, dp, np;
	idptr	ip;
	symptr	sp;
	treeptr	*F190;

	F190 = G189_vp;
	G189_vp = &vp;
	while ((*G189_vp) != (struct S61 *)NIL) {
		switch ((*G189_vp)->tt) {
		  case nvar:  case nvalpar:  case nvarpar:
			dp = (*G189_vp)->U.V14.tidl;
			break ;
		  case nparproc:  case nparfunc:
			dp = (*G189_vp)->U.V15.tparid;
			if (dp->U.V43.tsym->U.V6.lused) {
				ep = mknode((*G189_vp)->tt);
				ep->U.V15.tparparm = (struct S61 *)NIL;
				ep->U.V15.tpartyp = (*G189_vp)->U.V15.tpartyp;
				np = newid(mkrename('G', dp->U.V43.tsym->U.V6.lid));
				ep->U.V15.tparid = np;
				np->tup = ep;
				sp = np->U.V43.tsym;
				ip = sp->U.V6.lid;
				np->U.V43.tsym->U.V6.lid = dp->U.V43.tsym->U.V6.lid;
				dp->U.V43.tsym->U.V6.lid = ip;
				np->U.V43.tsym = dp->U.V43.tsym;
				dp->U.V43.tsym = sp;
				np->U.V43.tsym->lsymdecl = np;
				dp->U.V43.tsym->lsymdecl = dp;
				moveglob(tp, ep);
				addcode(tp, stackop((*G189_vp), np, dp));
			}
			goto L555;
			break ;
		  default:
			Caseerror(Line);
		}
		while (dp != (struct S61 *)NIL) {
			if (dp->U.V43.tsym->U.V6.lused) {
				ep = mknode(nvarpar);
				ep->U.V14.tattr = areference;
				np = newid(mkrename('G', dp->U.V43.tsym->U.V6.lid));
				ep->U.V14.tidl = np;
				np->tup = ep;
				ep->U.V14.tbind = (*G189_vp)->U.V14.tbind;
				if (ep->U.V14.tbind->tt == nid)
					ep->U.V14.tbind->U.V43.tsym->U.V6.lused = true;
				sp = np->U.V43.tsym;
				ip = sp->U.V6.lid;
				np->U.V43.tsym->U.V6.lid = dp->U.V43.tsym->U.V6.lid;
				dp->U.V43.tsym->U.V6.lid = ip;
				np->U.V43.tsym = dp->U.V43.tsym;
				dp->U.V43.tsym = sp;
				np->U.V43.tsym->lsymdecl = np;
				dp->U.V43.tsym->lsymdecl = dp;
				dp->tup->U.V14.tattr = aextern;
				moveglob(tp, ep);
				addcode(tp, stackop((*G189_vp), np, dp));
			}
			dp = dp->tnext;
		}
	L555:
		(*G189_vp) = (*G189_vp)->tnext;
	}
	G189_vp = F190;
}

 void
registervar(tp)
	treeptr	tp;
{
	treeptr	vp, xp;

	vp = idup(tp);
	tp = tp->U.V43.tsym->lsymdecl;
	if ((vp->U.V14.tidl != tp) || (tp->tnext != (struct S61 *)NIL)) {
		xp = mknode(nvar);
		xp->U.V14.tattr = anone;
		xp->U.V14.tidl = tp;
		tp->tup = xp;
		xp->tup = vp->tup;
		xp->U.V14.tbind = vp->U.V14.tbind;
		xp->tnext = vp->tnext;
		vp->tnext = xp;
		if (vp->U.V14.tidl == tp)
			vp->U.V14.tidl = tp->tnext;
		else {
			vp = vp->U.V14.tidl;
			while (vp->tnext != tp)
				vp = vp->tnext;
			vp->tnext = tp->tnext;
		}
		tp->tnext = (struct S61 *)NIL;
	}
	if (tp->tup->U.V14.tattr == anone)
		tp->tup->U.V14.tattr = aregister;
}

 void
cklevel(tp)
	treeptr	tp;
{
	tp = tp->U.V43.tsym->lsymdecl;
	while (!(Member((unsigned)(tp->tt), Conset[134])))
		tp = tp->tup;
	if (tp->U.V13.tstat > maxlevel)
		maxlevel = tp->U.V13.tstat;
}

 void
global(tp, dp, depend)
	treeptr	tp, dp;
	boolean	depend;
{
	treeptr	ip;
	boolean	dep;

	while (tp != (struct S61 *)NIL) {
		switch (tp->tt) {
		  case nproc:  case nfunc:
			markdecl(tp->U.V13.tsubid);
			markdecl(tp->U.V13.tsubpar);
			markdecl(tp->U.V13.tsubconst);
			markdecl(tp->U.V13.tsubtype);
			markdecl(tp->U.V13.tsubvar);
			global(tp->U.V13.tsubsub, tp, false);
			movevars(tp, tp->U.V13.tsubpar);
			movevars(tp, tp->U.V13.tsubvar);
			tp->U.V13.tsubtype = movedecl(tp->U.V13.tsubtype);
			tp->U.V13.tsubconst = movedecl(tp->U.V13.tsubconst);
			global(tp->U.V13.tsubstmt, tp, true);
			global(tp->U.V13.tsubpar, tp, false);
			global(tp->U.V13.tsubvar, tp, false);
			global(tp->U.V13.tsubtype, tp, false);
			global(tp->U.V13.tfuntyp, tp, false);
			break ;
		  case npgm:
			markdecl(tp->U.V13.tsubconst);
			markdecl(tp->U.V13.tsubtype);
			markdecl(tp->U.V13.tsubvar);
			global(tp->U.V13.tsubsub, tp, false);
			global(tp->U.V13.tsubstmt, tp, true);
			break ;
		  case nconst:  case ntype:  case nvar:  case nfield:
		  case nvalpar:  case nvarpar:
			ip = tp->U.V14.tidl;
			dep = depend;
			while ((ip != (struct S61 *)NIL) && !dep) {
				if (ip->U.V43.tsym->U.V6.lused)
					dep = true;
				ip = ip->tnext;
			}
			global(tp->U.V14.tbind, dp, dep);
			break ;
		  case nparproc:  case nparfunc:
			global(tp->U.V15.tparparm, dp, depend);
			global(tp->U.V15.tpartyp, dp, depend);
			break ;
		  case nsubrange:
			global(tp->U.V19.tlo, dp, depend);
			global(tp->U.V19.thi, dp, depend);
			break ;
		  case nvariant:
			global(tp->U.V20.tselct, dp, depend);
			global(tp->U.V20.tvrnt, dp, depend);
			break ;
		  case nrecord:
			global(tp->U.V21.tflist, dp, depend);
			global(tp->U.V21.tvlist, dp, depend);
			break ;
		  case nconfarr:
			global(tp->U.V22.tcindx, dp, depend);
			global(tp->U.V22.tcelem, dp, depend);
			break ;
		  case narray:
			global(tp->U.V23.taindx, dp, depend);
			global(tp->U.V23.taelem, dp, depend);
			break ;
		  case nfileof:  case nsetof:
			global(tp->U.V18.tof, dp, depend);
			break ;
		  case nptr:
			global(tp->U.V16.tptrid, dp, depend);
			break ;
		  case nscalar:
			global(tp->U.V17.tscalid, dp, depend);
			break ;
		  case nbegin:
			global(tp->U.V24.tbegin, dp, depend);
			break ;
		  case nif:
			global(tp->U.V31.tifxp, dp, depend);
			global(tp->U.V31.tthen, dp, depend);
			global(tp->U.V31.telse, dp, depend);
			break ;
		  case nwhile:
			global(tp->U.V32.twhixp, dp, depend);
			global(tp->U.V32.twhistmt, dp, depend);
			break ;
		  case nrepeat:
			global(tp->U.V33.treptstmt, dp, depend);
			global(tp->U.V33.treptxp, dp, depend);
			break ;
		  case nfor:
			ip = idup(tp->U.V34.tforid);
			if (Member((unsigned)(ip->tup->tt), Conset[135]))
				registervar(tp->U.V34.tforid);
			global(tp->U.V34.tforid, dp, depend);
			global(tp->U.V34.tfrom, dp, depend);
			global(tp->U.V34.tto, dp, depend);
			global(tp->U.V34.tforstmt, dp, depend);
			break ;
		  case ncase:
			global(tp->U.V35.tcasxp, dp, depend);
			global(tp->U.V35.tcaslst, dp, depend);
			global(tp->U.V35.tcasother, dp, depend);
			break ;
		  case nchoise:
			global(tp->U.V36.tchocon, dp, depend);
			global(tp->U.V36.tchostmt, dp, depend);
			break ;
		  case nwith:
			global(tp->U.V37.twithvar, dp, depend);
			global(tp->U.V37.twithstmt, dp, depend);
			break ;
		  case nwithvar:
			ip = typeof(tp->U.V38.texpw);
			if (ip->U.V21.tuid == (struct S59 *)NIL)
				ip->U.V21.tuid = mkvariable('S');
			global(tp->U.V38.texpw, dp, depend);
			break ;
		  case nlabstmt:
			global(tp->U.V25.tstmt, dp, depend);
			break ;
		  case neq:  case nne:  case nlt:  case nle:
		  case ngt:  case nge:
			global(tp->U.V41.texpl, dp, depend);

			global(tp->U.V41.texpr, dp, depend);
			ip = typeof(tp->U.V41.texpl);
			if ((ip == typnods.A[(int)(tstring)]) || (ip->tt == narray))
				usecomp = true;
			ip = typeof(tp->U.V41.texpr);
			if ((ip == typnods.A[(int)(tstring)]) || (ip->tt == narray))
				usecomp = true;
			break ;
		  case nin:  case nor:  case nplus:  case nminus:
		  case nand:  case nmul:  case ndiv:  case nmod:
		  case nquot:  case nformat:  case nrange:
			global(tp->U.V41.texpl, dp, depend);
			global(tp->U.V41.texpr, dp, depend);
			break ;
		  case nassign:
			global(tp->U.V27.tlhs, dp, depend);
			global(tp->U.V27.trhs, dp, depend);
			break ;
		  case nnot:  case numinus:  case nuplus:  case nderef:
			global(tp->U.V42.texps, dp, depend);
			break ;
		  case nset:
			global(tp->U.V42.texps, dp, depend);
			break ;
		  case nindex:
			global(tp->U.V39.tvariable, dp, depend);
			global(tp->U.V39.toffset, dp, depend);
			break ;
		  case nselect:
			global(tp->U.V40.trecord, dp, depend);
			break ;
		  case ncall:
			global(tp->U.V30.tcall, dp, depend);
			global(tp->U.V30.taparm, dp, depend);
			break ;
		  case nid:
			ip = idup(tp);
			if (ip == (struct S61 *)NIL)
				goto L555;
			do {
				ip = ip->tup;
				if (ip == (struct S61 *)NIL)
					goto L555;
			} while (!(Member((unsigned)(ip->tt), Conset[136])));
			if (dp == ip) {
				if (depend)
					tp->U.V43.tsym->U.V6.lused = true;
			} else {
				tp->U.V43.tsym->U.V6.lused = true;
			}
		L555:
			;
			break ;
		  case ngoto:
			if (!islocal(tp->U.V26.tlabel)) {
				tp->U.V26.tlabel->U.V43.tsym->U.V9.lgo = true;
				usejmps = true;
				cklevel(tp->U.V26.tlabel);
			}
			break ;
		  case nbreak:  case npush:  case npop:  case npredef:
		  case nempty:  case nchar:  case ninteger:  case nreal:
		  case nstring:  case nnil:
			break ;
		  default:
			Caseerror(Line);
		}
		tp = tp->tnext;
	}
}

 void
renamc()
{
	idptr	ip;
	register cnames	cn;

	{
		cnames	B49 = cabort,
			B50 = cwrite;

		if ((int)(B49) <= (int)(B50))
			for (cn = B49; ; cn = (cnames)((int)(cn)+1)) {
				ip = mkrename('C', ctable.A[(int)(cn)]);
				ctable.A[(int)(cn)]->istr = ip->istr;
				if (cn == B50) break;
			}
	}
}

 void
renamp(tp, on)
	treeptr	tp;
	boolean	on;
{
	symptr	sp;

	while (tp != (struct S61 *)NIL) {
		renamp(tp->U.V13.tsubsub, true);
		if (on && (tp->U.V13.tsubstmt != (struct S61 *)NIL)) {
			sp = tp->U.V13.tsubid->U.V43.tsym;
			if (sp->U.V6.lid->inref > 1) {
				sp->U.V6.lid = mkrename('P', sp->U.V6.lid);
				sp->U.V6.lid->inref = sp->U.V6.lid->inref - 1;
			}
		}
		tp = tp->tnext;
	}
}

void initcode();

 boolean
filevar(tp)
	treeptr	tp;
{
	register boolean	R168;
	boolean	fv;
	treeptr	tq;

	switch (tp->tt) {
	  case npredef:
		fv = (boolean)(tp == typnods.A[(int)(ttext)]);
		break ;
	  case nfileof:
		fv = true;
		break ;
	  case nconfarr:
		fv = filevar(typeof(tp->U.V22.tcelem));
		break ;
	  case narray:
		fv = filevar(typeof(tp->U.V23.taelem));
		break ;
	  case nrecord:
		fv = false;
		tq = tp->U.V21.tvlist;
		while (tq != (struct S61 *)NIL) {
			if (filevar(tq->U.V20.tvrnt))
				error(evrntfile);
			tq = tq->tnext;
		}
		tq = tp->U.V21.tflist;
		while (tq != (struct S61 *)NIL) {
			if (filevar(typeof(tq->U.V14.tbind))) {
				fv = true;
				tq = (struct S61 *)NIL;
			} else
				tq = tq->tnext;
		}
		break ;
	  case nptr:
		fv = false;
		if (!tp->U.V16.tptrflag) {
			tp->U.V16.tptrflag = true;
			if (filevar(typeof(tp->U.V16.tptrid)))
				error(evarfile);
			tp->U.V16.tptrflag = false;
		}
		break ;
	  case nsubrange:  case nscalar:  case nsetof:
		fv = false;
		break ;
	  default:
		Caseerror(Line);
	}
	R168 = fv;
	return R168;
}

 treeptr
fileinit(ti, tq, opn)
	treeptr	ti, tq;
	boolean	opn;
{
	register treeptr	R169;
	treeptr	tx, ty, tz;

	switch (tq->tt) {
	  case narray:
		tz = newid(mkvariable('I'));
		ty = mknode(nvar);
		ty->U.V14.tattr = aregister;
		ty->U.V14.tidl = tz;
		ty->U.V14.tbind = typeof(tq->U.V23.taindx);
		tz = tq;
		while (!(Member((unsigned)(tz->tt), Conset[137])))
			tz = tz->tup;
		linkup(tz, ty);
		if (tz->U.V13.tsubvar == (struct S61 *)NIL)
			tz->U.V13.tsubvar = ty;
		else {
			tz = tz->U.V13.tsubvar;
			while (tz->tnext != (struct S61 *)NIL)
				tz = tz->tnext;
			tz->tnext = ty;
		}
		ty = ty->U.V14.tidl;
		tz = mknode(nindex);
		tz->U.V39.tvariable = ti;
		tz->U.V39.toffset = ty;
		tz = fileinit(tz, tq->U.V23.taelem, opn);
		tx = mknode(nfor);
		tx->U.V34.tforid = ty;
		ty = typeof(tq->U.V23.taindx);
		if (ty->tt == nsubrange) {
			tx->U.V34.tfrom = ty->U.V19.tlo;
			tx->U.V34.tto = ty->U.V19.thi;
		} else
			if (ty->tt == nscalar) {
				ty = ty->U.V17.tscalid;
				tx->U.V34.tfrom = ty;
				while (ty->tnext != (struct S61 *)NIL)
					ty = ty->tnext;
				tx->U.V34.tto = ty;
			} else
				if (ty == typnods.A[(int)(tchar)]) {
					currsym.st = schar;
					currsym.U.V2.vchr = minchar;
					tx->U.V34.tfrom = mklit();
					currsym.st = schar;
					currsym.U.V2.vchr = maxchar;
					tx->U.V34.tto = mklit();
				} else
					if (ty == typnods.A[(int)(tinteger)]) {
						currsym.st = sinteger;
						currsym.U.V3.vint = -maxint;
						tx->U.V34.tfrom = mklit();
						currsym.st = sinteger;
						currsym.U.V3.vint = maxint;
						tx->U.V34.tto = mklit();
					} else
						fatal(etree);
		tx->U.V34.tforstmt = tz;
		tx->U.V34.tincr = true;
		break ;
	  case npredef:  case nfileof:
		if (opn) {
			ty = mknode(nselect);
			ty->U.V40.trecord = ti;
			ty->U.V40.tfield = oldid(defnams.A[(int)(dzinit)]->U.V6.lid, lforward);
			tx = mknode(nassign);
			tx->U.V27.tlhs = ty;
			currsym.st = sinteger;
			currsym.U.V3.vint = 0;
			tx->U.V27.trhs = mklit();
		} else {
			tx = mknode(ncall);
			tx->U.V30.tcall = oldid(defnams.A[(int)(dclose)]->U.V6.lid, lidentifier);
			tx->U.V30.taparm = ti;
		}
		break ;
	  case nrecord:
		ty = (struct S61 *)NIL;
		tq = tq->U.V21.tflist;
		while (tq != (struct S61 *)NIL) {
			if (filevar(typeof(tq->U.V14.tbind))) {
				tz = tq->U.V14.tidl;
				while (tz != (struct S61 *)NIL) {
					tx = mknode(nselect);
					tx->U.V40.trecord = ti;
					tx->U.V40.tfield = tz;
					tx = fileinit(tx, typeof(tq->U.V14.tbind), opn);
					tx->tnext = ty;
					ty = tx;
					tz = tz->tnext;
				}
			}
			tq = tq->tnext;
		}
		tx = mknode(nbegin);
		tx->U.V24.tbegin = ty;
		break ;
	  default:
		Caseerror(Line);
	}
	R169 = tx;
	return R169;
}

 void
initcode(tp)
	treeptr	tp;
{
	treeptr	ti, tq, tu, tv;

	while (tp != (struct S61 *)NIL) {
		initcode(tp->U.V13.tsubsub);
		tv = tp->U.V13.tsubvar;
		while (tv != (struct S61 *)NIL) {
			tq = typeof(tv->U.V14.tbind);
			if (filevar(tq)) {
				ti = tv->U.V14.tidl;
				while (ti != (struct S61 *)NIL) {
					tu = fileinit(ti, tq, true);
					linkup(tp, tu);
					tu->tnext = tp->U.V13.tsubstmt;
					tp->U.V13.tsubstmt = tu;
					while (tu->tnext != (struct S61 *)NIL)
						tu = tu->tnext;
					tu->tnext = fileinit(ti, tq, false);
					linkup(tp, tu->tnext);
					ti = ti->tnext;
				}
			}
			tv = tv->tnext;
		}
		tp = tp->tnext;
	}
}

 void
transform()
{
	renamc();
	renamp(top->U.V13.tsubsub, false);
	extract(top);
	renamf(top);
	initcode(top->U.V13.tsubsub);
	global(top, top, false);
}

void emit();

 void
increment()
{
	(*G204_indnt) = (*G204_indnt) + indstep;
}

 void
decrement()
{
	(*G204_indnt) = (*G204_indnt) - indstep;
}

 void
indent()
{
	integer	i;

	i = (*G204_indnt);
	if (i > 60)
		i = i / tabwidth * tabwidth;
	while (i >= tabwidth) {
		Putchr(tab1, output);
		i = i - tabwidth;
	}
	while (i > 0) {
		Putchr(space, output);
		i = i - 1;
	}
}

 boolean
arithexpr(tp)
	treeptr	tp;
{
	register boolean	R170;

	tp = typeof(tp);
	if (tp->tt == nsubrange)
		if (tp->tup->tt == nconfarr)
			tp = typeof(tp->tup->U.V22.tindtyp);
		else
			tp = typeof(tp->U.V19.tlo);
	R170 = (boolean)((tp == typnods.A[(int)(tinteger)]) || (tp == typnods.A[(int)(tchar)]) || (tp == typnods.A[(int)(treal)]));
	return R170;
}

void eexpr();

 void
eselect(tp)
	treeptr	tp;
{
	(*G202_doarrow) = (*G202_doarrow) + 1;
	eexpr(tp);
	(*G202_doarrow) = (*G202_doarrow) - 1;
	if ((*G200_donearr))
		(*G200_donearr) = false;
	else
		Putchr('.', output);
}

void epredef();

 char
typeletter(tp)
	treeptr	tp;
{
	register char	R171;
	treeptr	tq;

	tq = tp;
	if (tq->tt == nformat) {
		if (tq->U.V41.texpl->tt == nformat) {
			R171 = 'f';
			goto L999;
		}
		tq = tp->U.V41.texpl;
	}
	tq = typeof(tq);
	if (tq->tt == nsubrange)
		tq = typeof(tq->U.V19.tlo);
	if (tq == typnods.A[(int)(tstring)])
		R171 = 's';
	else
		if (tq == typnods.A[(int)(tinteger)])
			R171 = 'd';
		else
			if (tq == typnods.A[(int)(tchar)])
				R171 = 'c';
			else
				if (tq == typnods.A[(int)(treal)])
					if (tp->tt == nformat)
						R171 = 'e';
					else
						R171 = 'g';
				else
					if (tq == typnods.A[(int)(tboolean)]) {
						R171 = 'b';
						(*G191_nelems) = 6;
					} else
						if (tq->tt == narray) {
							R171 = 'a';
							(*G191_nelems) = crange(tq->U.V23.taindx);
						} else
							if (tq->tt == nconfarr) {
								R171 = 'v';
								(*G191_nelems) = 0;
							} else
								fatal(etree);
L999:
	;
	return R171;
}

 void
etxt(tp)
	treeptr	tp;
{
	toknbuf	w;
	char	c;
	toknidx	i;

	switch (tp->tt) {
	  case nid:
		tp = idup(tp);
		if (tp->tt == nconst)
			etxt(tp->U.V14.tbind);
		else
			fatal(etree);
		break ;
	  case nstring:
		gettokn(tp->U.V43.tsym->U.V7.lstr, &w);
		i = 1;
		while (w.A[i - 1] != null) {
			c = w.A[i - 1];
			if ((c == cite) || (c == bslash))
				Putchr(bslash, output);
			else
				if (c == percent)
					Putchr(percent, output);
			Putchr(c, output);
			i = i + 1;
		}
		break ;
	  case nchar:
		c = tp->U.V43.tsym->U.V11.lchar;
		if ((c == cite) || (c == bslash))
			Putchr(bslash, output);
		else
			if (c == percent)
				Putchr(percent, output);
		Putchr(c, output);
		break ;
	  default:
		Caseerror(Line);
	}
}

 void
eformat(tq)
	treeptr	tq;
{
	treeptr	tx;
	integer	i;

	switch (typeletter(tq)) {
	  case 'a':
		Putchr(percent, output);
		if (tq->tt == nformat)
			if (tq->U.V41.texpr->tt == ninteger)
				eexpr(tq->U.V41.texpr);
			else
				Putchr('*', output);
		(void)fprintf(output.fp, ".%1ds", (*G191_nelems)), Putl(output, 0);
		break ;
	  case 'b':
		Putchr(percent, output);
		if (tq->tt == nformat) {
			if (tq->U.V41.texpr->tt == ninteger)
				eexpr(tq->U.V41.texpr);
			else
				Putchr('*', output);
		}
		Putchr('s', output);
		break ;
	  case 'c':
		if (tq->tt == nchar)
			etxt(tq);
		else {
			Putchr(percent, output);
			if (tq->tt == nformat)
				if (tq->U.V41.texpr->tt == ninteger)
					eexpr(tq->U.V41.texpr);
				else
					Putchr('*', output);
			Putchr('c', output);
		}
		break ;
	  case 'd':
		Putchr(percent, output);
		if (tq->tt == nformat) {
			if (tq->U.V41.texpr->tt == ninteger)
				eexpr(tq->U.V41.texpr);
			else
				Putchr('*', output);
		} else
			(void)fprintf(output.fp, "%1d", intlen), Putl(output, 0);
		Putchr('l', output);
		Putchr('d', output);
		break ;
	  case 'e':
		(void)fprintf(output.fp, "%c%c", percent, space), Putl(output, 0);
		tx = tq->U.V41.texpr;
		if (tx->tt == ninteger) {
			i = cvalof(tx);
			(void)fprintf(output.fp, "%1d.", i), Putl(output, 0);
			i = i - 7;
			if (i < 1)
				Putchr('1', output);
			else
				(void)fprintf(output.fp, "%1d", i), Putl(output, 0);
		} else
			(void)fprintf(output.fp, "*.*"), Putl(output, 0);
		Putchr('e', output);
		break ;
	  case 'f':
		Putchr(percent, output);
		tx = tq->U.V41.texpl;
		if (tx->U.V41.texpr->tt == ninteger) {
			eexpr(tx->U.V41.texpr);
			Putchr('.', output);
			tx = tq->U.V41.texpr;
			if (tx->tt == ninteger) {
				i = cvalof(tx);
				tx = tq->U.V41.texpl->U.V41.texpr;
				if (i > cvalof(tx) - 1)
					Putchr('1', output);
				else
					(void)fprintf(output.fp, "%1d", i), Putl(output, 0);
			} else
				Putchr('*', output);
		} else
			(void)fprintf(output.fp, "*.*"), Putl(output, 0);
		Putchr('f', output);
		break ;
	  case 'g':
		(void)fprintf(output.fp, "%c%1de", percent, fixlen), Putl(output, 0);
		break ;
	  case 's':
		if (tq->tt == nstring)
			etxt(tq);
		else {
			Putchr(percent, output);
			if (tq->tt == nformat)
				if (tq->U.V41.texpr->tt == ninteger)
					eexpr(tq->U.V41.texpr);
				else
					(void)fprintf(output.fp, "*.*"), Putl(output, 0);
			Putchr('s', output);
		}
		break ;
	  default:
		Caseerror(Line);
	}
}

 void
ewrite(tq)
	treeptr	tq;
{
	treeptr	tx;

	switch (typeletter(tq)) {
	  case 'a':
		(void)fprintf(output.fp, ", "), Putl(output, 0);
		tx = tq;
		if (tq->tt == nformat) {
			if (tq->U.V41.texpr->tt != ninteger) {
				eexpr(tq->U.V41.texpr);
				(void)fprintf(output.fp, ", "), Putl(output, 0);
			}
			tx = tq->U.V41.texpl;
		}
		eexpr(tx);
		(void)fprintf(output.fp, ".A"), Putl(output, 0);
		break ;
	  case 'b':
		(void)fprintf(output.fp, ", "), Putl(output, 0);
		tx = tq;
		if (tq->tt == nformat) {
			if (tq->U.V41.texpr->tt != ninteger) {
				eexpr(tq->U.V41.texpr);
				(void)fprintf(output.fp, ", "), Putl(output, 0);
			}
			tx = tq->U.V41.texpl;
		}
		usebool = true;
		(void)fprintf(output.fp, "Bools[(int)("), Putl(output, 0);
		eexpr(tx);
		(void)fprintf(output.fp, ")]"), Putl(output, 0);
		break ;
	  case 'c':
		if (tq->tt == nformat) {
			if (tq->U.V41.texpr->tt != ninteger) {
				(void)fprintf(output.fp, ", "), Putl(output, 0);
				eexpr(tq->U.V41.texpr);
			}
			(void)fprintf(output.fp, ", "), Putl(output, 0);
			eexpr(tq->U.V41.texpl);
		} else
			if (tq->tt != nchar) {
				(void)fprintf(output.fp, ", "), Putl(output, 0);
				eexpr(tq);
			}
		break ;
	  case 'd':
		(void)fprintf(output.fp, ", "), Putl(output, 0);
		tx = tq;
		if (tq->tt == nformat) {
			if (tq->U.V41.texpr->tt != ninteger) {
				eexpr(tq->U.V41.texpr);
				(void)fprintf(output.fp, ", "), Putl(output, 0);
			}
			tx = tq->U.V41.texpl;
		}
		eexpr(tx);
		break ;
	  case 'e':
		(void)fprintf(output.fp, ", "), Putl(output, 0);
		tx = tq->U.V41.texpr;
		if (tx->tt != ninteger) {
			usemax = true;
			eexpr(tx);
			(void)fprintf(output.fp, ", Max("), Putl(output, 0);
			eexpr(tx);
			(void)fprintf(output.fp, " - 7, 1), "), Putl(output, 0);
		}
		eexpr(tq->U.V41.texpl);
		break ;
	  case 'f':
		(void)fprintf(output.fp, ", "), Putl(output, 0);
		tx = tq->U.V41.texpl;
		if (tx->U.V41.texpr->tt != ninteger) {
			eexpr(tx->U.V41.texpr);
			(void)fprintf(output.fp, ", "), Putl(output, 0);
		}
		if ((tx->U.V41.texpr->tt != ninteger) || (tq->U.V41.texpr->tt != ninteger)) {
			usemax = true;
			(void)fprintf(output.fp, "Max(("), Putl(output, 0);
			eexpr(tx->U.V41.texpr);
			(void)fprintf(output.fp, ") - ("), Putl(output, 0);
			eexpr(tq->U.V41.texpr);
			(void)fprintf(output.fp, ") - 1, 1), "), Putl(output, 0);
		}
		eexpr(tq->U.V41.texpl->U.V41.texpl);
		break ;
	  case 'g':
		(void)fprintf(output.fp, ", "), Putl(output, 0);
		eexpr(tq);
		break ;
	  case 's':
		if (tq->tt == nformat) {
			if (tq->U.V41.texpr->tt != ninteger) {
				(void)fprintf(output.fp, ", "), Putl(output, 0);
				eexpr(tq->U.V41.texpr);
				(void)fprintf(output.fp, ", "), Putl(output, 0);
				eexpr(tq->U.V41.texpr);
			}
			(void)fprintf(output.fp, ", "), Putl(output, 0);
			eexpr(tq->U.V41.texpl);
		} else
			if (tq->tt != nstring) {
				(void)fprintf(output.fp, ", "), Putl(output, 0);
				eexpr(tq);
			}
		break ;
	  default:
		Caseerror(Line);
	}
}

void enewsize();

 void
esubsize(tp, tq)
	treeptr	tp, tq;
{
	treeptr	tx, ty;
	boolean	addsize;

	tx = tq->U.V20.tvrnt;
	ty = tx->U.V21.tflist;
	if (ty == (struct S61 *)NIL) {
		ty = tx->U.V21.tvlist;
		while (ty != (struct S61 *)NIL) {
			if (ty->U.V20.tvrnt->U.V21.tflist != (struct S61 *)NIL) {
				ty = ty->U.V20.tvrnt->U.V21.tflist;
				goto L555;
			}
			ty = ty->tnext;
		}
	L555:
		;
	}
	addsize = true;
	if (ty == (struct S61 *)NIL) {
		addsize = false;
		ty = tx->tup->tup->U.V21.tvlist;
		while (ty != (struct S61 *)NIL) {
			if (ty->U.V20.tvrnt->U.V21.tflist != (struct S61 *)NIL) {
				ty = ty->U.V20.tvrnt->U.V21.tflist;
				goto L666;
			}
			ty = ty->tnext;
		}
	L666:
		;
	}
	if (ty == (struct S61 *)NIL) {
		(void)fprintf(output.fp, "sizeof(*"), Putl(output, 0);
		eexpr(tp);
		Putchr(')', output);
	} else {
		(void)fprintf(output.fp, "Unionoffs("), Putl(output, 0);
		eexpr(tp);
		(void)fprintf(output.fp, ", "), Putl(output, 0);
		printid(ty->U.V14.tidl->U.V43.tsym->U.V6.lid);
		if (addsize) {
			(void)fprintf(output.fp, ") + sizeof("), Putl(output, 0);
			eexpr(tp);
			(void)fprintf(output.fp, "->"), Putl(output, 0);
			printid(tx->U.V21.tuid);
		}
		Putchr(')', output);
	}
}

 void
enewsize(tp)
	treeptr	tp;
{
	treeptr	tq, tx, ty;
	integer	v;

	if ((tp->tnext != (struct S61 *)NIL) && unionnew) {
		v = cvalof(tp->tnext);
		tq = typeof(tp);
		tq = typeof(tq->U.V16.tptrid);
		if (tq->tt != nrecord)
			fatal(etree);
		tx = tq->U.V21.tvlist;
		while (tx != (struct S61 *)NIL) {
			ty = tx->U.V20.tselct;
			while (ty != (struct S61 *)NIL) {
				if (v == cvalof(ty))
					goto L555;
				ty = ty->tnext;
			}
			tx = tx->tnext;
		}
		fatal(etag);
	L555:
		esubsize(tp, tx);
	} else {
		(void)fprintf(output.fp, "sizeof(*"), Putl(output, 0);
		eexpr(tp);
		Putchr(')', output);
	}
}

 void
epredef(ts, tp)
	treeptr	ts, tp;
{
	treeptr	tq, tv, tx;
	predefs	td;
	integer	nelems;
	char	ch;
	boolean	txtfile;
	integer	*F192;

	F192 = G191_nelems;
	G191_nelems = &nelems;
	td = ts->U.V13.tsubstmt->U.V12.tdef;
	switch (td) {
	  case dabs:
		tq = typeof(tp->U.V30.taparm);
		if ((tq == typnods.A[(int)(tinteger)]) || (tq->tt == nsubrange))
			(void)fprintf(output.fp, "abs("), Putl(output, 0);
		else
			(void)fprintf(output.fp, "fabs("), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		Putchr(')', output);
		break ;
	  case dargv:
		(void)fprintf(output.fp, "Argvgt("), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		(void)fprintf(output.fp, ", "), Putl(output, 0);
		eexpr(tp->U.V30.taparm->tnext);
		(void)fprintf(output.fp, ".A, sizeof("), Putl(output, 0);
		eexpr(tp->U.V30.taparm->tnext);
		(void)fprintf(output.fp, ".A));\n"), Putl(output, 1);
		break ;
	  case dchr:
		tq = typeof(tp->U.V30.taparm);
		if (tq->tt == nsubrange)
			if (tq->tup->tt == nconfarr)
				tq = typeof(tq->tup->U.V22.tindtyp);
			else
				tq = typeof(tq->U.V19.tlo);
		if ((tq == typnods.A[(int)(tinteger)]) || (tq == typnods.A[(int)(tchar)]))
			eexpr(tp->U.V30.taparm);
		else {
			(void)fprintf(output.fp, "(char)("), Putl(output, 0);
			eexpr(tp->U.V30.taparm);
			Putchr(')', output);
		}
		break ;
	  case ddispose:
		(void)fprintf(output.fp, "free("), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		break ;
	  case deof:
		(void)fprintf(output.fp, "Eof("), Putl(output, 0);
		if (tp->U.V30.taparm == (struct S61 *)NIL) {
			defnams.A[(int)(dinput)]->U.V6.lused = true;
			printid(defnams.A[(int)(dinput)]->U.V6.lid);
		} else
			eexpr(tp->U.V30.taparm);
		Putchr(')', output);
		break ;
	  case deoln:
		(void)fprintf(output.fp, "Eoln("), Putl(output, 0);
		if (tp->U.V30.taparm == (struct S61 *)NIL) {
			defnams.A[(int)(dinput)]->U.V6.lused = true;
			printid(defnams.A[(int)(dinput)]->U.V6.lid);
		} else
			eexpr(tp->U.V30.taparm);
		Putchr(')', output);
		break ;
	  case dexit:
		(void)fprintf(output.fp, "exit("), Putl(output, 0);
		if (tp->U.V30.taparm == (struct S61 *)NIL)
			Putchr('0', output);
		else
			eexpr(tp->U.V30.taparm);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		break ;
	  case dflush:
		(void)fprintf(output.fp, "fflush("), Putl(output, 0);
		if (tp->U.V30.taparm == (struct S61 *)NIL) {
			defnams.A[(int)(doutput)]->U.V6.lused = true;
			printid(defnams.A[(int)(doutput)]->U.V6.lid);
		} else
			eexpr(tp->U.V30.taparm);
		(void)fprintf(output.fp, ".fp);\n"), Putl(output, 1);
		break ;
	  case dpage:
		(void)fprintf(output.fp, "Putchr(%s, ", ffchr), Putl(output, 0);
		if (tp->U.V30.taparm == (struct S61 *)NIL) {
			defnams.A[(int)(doutput)]->U.V6.lused = true;
			printid(defnams.A[(int)(doutput)]->U.V6.lid);
		} else
			eexpr(tp->U.V30.taparm);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		break ;
	  case dput:  case dget:
		if (typeof(tp->U.V30.taparm) == typnods.A[(int)(ttext)])
			if (td == dget)
				(void)fprintf(output.fp, "Getx"), Putl(output, 0);
			else
				(void)fprintf(output.fp, "Putx"), Putl(output, 0);
		else {
			(void)fprintf(output.fp, "%s", voidcast), Putl(output, 0);
			if (td == dget)
				(void)fprintf(output.fp, "Get"), Putl(output, 0);
			else
				(void)fprintf(output.fp, "Put"), Putl(output, 0);
		}
		Putchr('(', output);
		eexpr(tp->U.V30.taparm);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		break ;
	  case dhalt:
		(void)fprintf(output.fp, "abort();\n"), Putl(output, 1);
		break ;
	  case dnew:
		eexpr(tp->U.V30.taparm);
		(void)fprintf(output.fp, " = ("), Putl(output, 0);
		etypedef(typeof(tp->U.V30.taparm));
		(void)fprintf(output.fp, ")malloc((unsigned)("), Putl(output, 0);
		enewsize(tp->U.V30.taparm);
		(void)fprintf(output.fp, "));\n"), Putl(output, 1);
		break ;
	  case dord:
		(void)fprintf(output.fp, "(unsigned)("), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		Putchr(')', output);
		break ;
	  case dread:  case dreadln:
		txtfile = false;
		tq = tp->U.V30.taparm;
		if (tq != (struct S61 *)NIL) {
			tv = typeof(tq);
			if (tv == typnods.A[(int)(ttext)]) {
				txtfile = true;
				tv = tq;
				tq = tq->tnext;
			} else
				if (tv->tt == nfileof) {
					txtfile = (boolean)(typeof(tv->U.V18.tof) == typnods.A[(int)(tchar)]);
					tv = tq;
					tq = tq->tnext;
				} else {
					txtfile = true;
					tv = (struct S61 *)NIL;
				}
		} else {
			tv = (struct S61 *)NIL;
			txtfile = true;
		}
		if (txtfile) {
			if (tq == (struct S61 *)NIL)
				goto L444;
			if ((tq->tt != nformat) && (tq->tnext == (struct S61 *)NIL) && (typeletter(tq) == 'c')) {
				eexpr(tq);
				(void)fprintf(output.fp, " = "), Putl(output, 0);
				(void)fprintf(output.fp, "Getchr("), Putl(output, 0);
				if (tv == (struct S61 *)NIL)
					printid(defnams.A[(int)(dinput)]->U.V6.lid);
				else
					eexpr(tv);
				Putchr(')', output);
				if (td == dreadln)
					Putchr(',', output);
				goto L444;
			}
			usescan = true;
			(void)fprintf(output.fp, "Fscan("), Putl(output, 0);
			if (tv == (struct S61 *)NIL)
				printid(defnams.A[(int)(dinput)]->U.V6.lid);
			else
				eexpr(tv);
			(void)fprintf(output.fp, "), "), Putl(output, 0);
			while (tq != (struct S61 *)NIL) {
				(void)fprintf(output.fp, "Scan(%c", cite), Putl(output, 0);
				ch = typeletter(tq);
				switch (ch) {
				  case 'a':
					(void)fprintf(output.fp, "%cs", percent), Putl(output, 0);
					break ;
				  case 'c':
					(void)fprintf(output.fp, "%cc", percent), Putl(output, 0);
					break ;
				  case 'd':
					(void)fprintf(output.fp, "%cld", percent), Putl(output, 0);
					break ;
				  case 'g':
					(void)fprintf(output.fp, "%cle", percent), Putl(output, 0);
					break ;
				  default:
					Caseerror(Line);
				}
				(void)fprintf(output.fp, "%c, ", cite), Putl(output, 0);
				switch (ch) {
				  case 'a':
					eexpr(tq);
					(void)fprintf(output.fp, ".A"), Putl(output, 0);
					break ;
				  case 'c':
					Putchr('&', output);
					eexpr(tq);
					break ;
				  case 'd':
					(void)fprintf(output.fp, "&Tmplng"), Putl(output, 0);
					break ;
				  case 'g':
					(void)fprintf(output.fp, "&Tmpdbl"), Putl(output, 0);
					break ;
				  default:
					Caseerror(Line);
				}
				Putchr(')', output);
				switch (ch) {
				  case 'd':
					(void)fprintf(output.fp, ", "), Putl(output, 0);
					eexpr(tq);
					(void)fprintf(output.fp, " = Tmplng"), Putl(output, 0);
					break ;
				  case 'g':
					(void)fprintf(output.fp, ", "), Putl(output, 0);
					eexpr(tq);
					(void)fprintf(output.fp, " = Tmpdbl"), Putl(output, 0);
					break ;
				  case 'a':  case 'c':
					break ;
				  default:
					Caseerror(Line);
				}
				tq = tq->tnext;
				if (tq != (struct S61 *)NIL) {
					Putchr(',', output),Putchr('\n', output);
					indent();
					Putchr(tab1, output);
				}
			}
			(void)fprintf(output.fp, ", Getx("), Putl(output, 0);
			if (tv == (struct S61 *)NIL)
				printid(defnams.A[(int)(dinput)]->U.V6.lid);
			else
				eexpr(tv);
			Putchr(')', output);
			if (td == dreadln)
				Putchr(',', output);
		L444:
			if (td == dreadln) {
				usegetl = true;
				(void)fprintf(output.fp, "Getl(&"), Putl(output, 0);
				if (tv == (struct S61 *)NIL)
					printid(defnams.A[(int)(dinput)]->U.V6.lid);
				else
					eexpr(tv);
				Putchr(')', output);
			}
		} else {
			increment();
			while (tq != (struct S61 *)NIL) {
				(void)fprintf(output.fp, "%sFread(", voidcast), Putl(output, 0);
				eexpr(tq);
				(void)fprintf(output.fp, ", "), Putl(output, 0);
				eexpr(tv);
				(void)fprintf(output.fp, ".fp)"), Putl(output, 0);
				tq = tq->tnext;
				if (tq != (struct S61 *)NIL) {
					Putchr(',', output),Putchr('\n', output);
					indent();
				}
			}
			decrement();
		}
		Putchr(';', output),Putchr('\n', output);
		break ;
	  case dwrite:  case dwriteln:  case dmessage:
		txtfile = false;
		tq = tp->U.V30.taparm;
		if (tq != (struct S61 *)NIL) {
			tv = typeof(tq);
			if (tv == typnods.A[(int)(ttext)]) {
				txtfile = true;
				tv = tq;
				tq = tq->tnext;
			} else
				if (tv->tt == nfileof) {
					txtfile = (boolean)(typeof(tv->U.V18.tof) == typnods.A[(int)(tchar)]);
					tv = tq;
					tq = tq->tnext;
				} else {
					txtfile = true;
					tv = (struct S61 *)NIL;
				}
		} else {
			tv = (struct S61 *)NIL;
			txtfile = true;
		}
		if (txtfile) {
			if (tq == (struct S61 *)NIL) {
				if (Member((unsigned)(td), Conset[138])) {
					(void)fprintf(output.fp, "Putchr(%s, ", nlchr), Putl(output, 0);
					if (tv == (struct S61 *)NIL)
						printid(defnams.A[(int)(doutput)]->U.V6.lid);
					else
						eexpr(tv);
					Putchr(')', output);
				}
				Putchr(';', output),Putchr('\n', output);
				goto L555;
			} else
				if ((tq->tt != nformat) && (tq->tnext == (struct S61 *)NIL))
					if (typeletter(tq) == 'c') {
						(void)fprintf(output.fp, "Putchr("), Putl(output, 0);
						eexpr(tq);
						(void)fprintf(output.fp, ", "), Putl(output, 0);
						if (tv == (struct S61 *)NIL)
							printid(defnams.A[(int)(doutput)]->U.V6.lid);
						else
							eexpr(tv);
						Putchr(')', output);
						if (td == dwriteln) {
							(void)fprintf(output.fp, ",Putchr(%s, ", nlchr), Putl(output, 0);
							if (tv == (struct S61 *)NIL)
								printid(defnams.A[(int)(doutput)]->U.V6.lid);
							else
								eexpr(tv);
							Putchr(')', output);
						}
						Putchr(';', output),Putchr('\n', output);
						goto L555;
					}
			tx = (struct S61 *)NIL;
			(void)fprintf(output.fp, "%sfprintf(", voidcast), Putl(output, 0);
			if (td == dmessage)
				(void)fprintf(output.fp, "stderr, "), Putl(output, 0);
			else {
				if (tv == (struct S61 *)NIL)
					printid(defnams.A[(int)(doutput)]->U.V6.lid);
				else
					eexpr(tv);
				(void)fprintf(output.fp, ".fp, "), Putl(output, 0);
			}
			Putchr(cite, output);
			tx = tq;
			while (tq != (struct S61 *)NIL) {
				eformat(tq);
				tq = tq->tnext;
			}
			if ((td == dmessage) || (td == dwriteln))
				(void)fprintf(output.fp, "\\n"), Putl(output, 0);
			Putchr(cite, output);
			tq = tx;
			while (tq != (struct S61 *)NIL) {
				ewrite(tq);
				tq = tq->tnext;
			}
			(void)fprintf(output.fp, "), Putl("), Putl(output, 0);
			if (tv == (struct S61 *)NIL)
				printid(defnams.A[(int)(doutput)]->U.V6.lid);
			else
				eexpr(tv);
			if (td == dwrite)
				(void)fprintf(output.fp, ", 0)"), Putl(output, 0);
			else
				(void)fprintf(output.fp, ", 1)"), Putl(output, 0);
		} else {
			increment();
			tx = typeof(tv);
			if (tx == typnods.A[(int)(ttext)])
				tx = typnods.A[(int)(tchar)];
			else
				if (tx->tt == nfileof)
					tx = typeof(tx->U.V18.tof);
				else
					fatal(etree);
			while (tq != (struct S61 *)NIL) {
				if ((Member((unsigned)(tq->tt), Conset[139])) && (tx == typeof(tq))) {
					(void)fprintf(output.fp, "%sFwrite(", voidcast), Putl(output, 0);
					eexpr(tq);
				} else {
					if (tx->tt == nsetof) {
						usescpy = true;
						(void)fprintf(output.fp, "Setncpy("), Putl(output, 0);
						eselect(tv);
						(void)fprintf(output.fp, "buf.S, "), Putl(output, 0);
						eexpr(tq);
						if (typeof(tp->U.V27.trhs) == typnods.A[(int)(tset)])
							eexpr(tq);
						else {
							eselect(tq);
							Putchr('S', output);
						}
						(void)fprintf(output.fp, ", sizeof("), Putl(output, 0);
						eexpr(tv);
						(void)fprintf(output.fp, ".buf))"), Putl(output, 0);
					} else {
						eexpr(tv);
						(void)fprintf(output.fp, ".buf = "), Putl(output, 0);
						eexpr(tq);
					}
					(void)fprintf(output.fp, ", Fwrite("), Putl(output, 0);
					eexpr(tv);
					(void)fprintf(output.fp, ".buf"), Putl(output, 0);
				}
				(void)fprintf(output.fp, ", "), Putl(output, 0);
				eexpr(tv);
				(void)fprintf(output.fp, ".fp)"), Putl(output, 0);
				tq = tq->tnext;
				if (tq != (struct S61 *)NIL) {
					Putchr(',', output),Putchr('\n', output);
					indent();
				}
			}
			decrement();
		}
		Putchr(';', output),Putchr('\n', output);
	L555:
		;
		break ;
	  case dclose:
		tq = typeof(tp->U.V30.taparm);
		txtfile = (boolean)(tq == typnods.A[(int)(ttext)]);
		if ((!txtfile) && (tq->tt == nfileof))
			if (typeof(tq->U.V18.tof) == typnods.A[(int)(tchar)])
				txtfile = true;
		if (txtfile)
			(void)fprintf(output.fp, "Closex("), Putl(output, 0);
		else
			(void)fprintf(output.fp, "Close("), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		break ;
	  case dreset:  case drewrite:
		tq = typeof(tp->U.V30.taparm);
		txtfile = (boolean)(tq == typnods.A[(int)(ttext)]);
		if ((!txtfile) && (tq->tt == nfileof))
			if (typeof(tq->U.V18.tof) == typnods.A[(int)(tchar)])
				txtfile = true;
		if (txtfile)
			if (td == dreset)
				(void)fprintf(output.fp, "Resetx("), Putl(output, 0);
			else
				(void)fprintf(output.fp, "Rewritex("), Putl(output, 0);
		else
			if (td == dreset)
				(void)fprintf(output.fp, "Reset("), Putl(output, 0);
			else
				(void)fprintf(output.fp, "Rewrite("), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		(void)fprintf(output.fp, ", "), Putl(output, 0);
		tq = tp->U.V30.taparm->tnext;
		if (tq == (struct S61 *)NIL)
			(void)fprintf(output.fp, "NULL"), Putl(output, 0);
		else {
			tq = typeof(tq);
			if (tq == typnods.A[(int)(tchar)]) {
				Putchr(cite, output);
				ch = cvalof(tp->U.V30.taparm->tnext);
				if ((ch == bslash) || (ch == cite))
					Putchr(bslash, output);
				(void)fprintf(output.fp, "%c%c", ch, cite), Putl(output, 0);
			} else
				if (tq == typnods.A[(int)(tstring)])
					eexpr(tp->U.V30.taparm->tnext);
				else
					if (Member((unsigned)(tq->tt), Conset[140])) {
						eexpr(tp->U.V30.taparm->tnext);
						(void)fprintf(output.fp, ".A"), Putl(output, 0);
					} else
						fatal(etree);
		}
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		break ;
	  case darctan:
		(void)fprintf(output.fp, "atan("), Putl(output, 0);
		if (typeof(tp->U.V30.taparm) != typnods.A[(int)(treal)])
			(void)fprintf(output.fp, "%s", dblcast), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		Putchr(')', output);
		break ;
	  case dln:
		(void)fprintf(output.fp, "log("), Putl(output, 0);
		if (typeof(tp->U.V30.taparm) != typnods.A[(int)(treal)])
			(void)fprintf(output.fp, "%s", dblcast), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		Putchr(')', output);
		break ;
	  case dexp:
		(void)fprintf(output.fp, "exp("), Putl(output, 0);
		if (typeof(tp->U.V30.taparm) != typnods.A[(int)(treal)])
			(void)fprintf(output.fp, "%s", dblcast), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		Putchr(')', output);
		break ;
	  case dcos:  case dsin:  case dsqrt:
		eexpr(tp->U.V30.tcall);
		Putchr('(', output);
		if (typeof(tp->U.V30.taparm) != typnods.A[(int)(treal)])
			(void)fprintf(output.fp, "%s", dblcast), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		Putchr(')', output);
		break ;
	  case dtan:
		(void)fprintf(output.fp, "atan("), Putl(output, 0);
		if (typeof(tp->U.V30.taparm) != typnods.A[(int)(treal)])
			(void)fprintf(output.fp, "%s", dblcast), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		Putchr(')', output);
		break ;
	  case dsucc:  case dpred:
		tq = typeof(tp->U.V30.taparm);
		if (tq->tt == nsubrange)
			if (tq->tup->tt == nconfarr)
				tq = typeof(tq->tup->U.V22.tindtyp);
			else
				tq = typeof(tq->U.V19.tlo);
		if ((tq == typnods.A[(int)(tinteger)]) || (tq == typnods.A[(int)(tchar)])) {
			(void)fprintf(output.fp, "(("), Putl(output, 0);
			eexpr(tp->U.V30.taparm);
			if (td == dpred)
				(void)fprintf(output.fp, ")-1)"), Putl(output, 0);
			else
				(void)fprintf(output.fp, ")+1)"), Putl(output, 0);
		} else {
			Putchr('(', output);
			tq = tq->tup;
			if (tq->tt == ntype) {
				Putchr('(', output);
				printid(tq->U.V14.tidl->U.V43.tsym->U.V6.lid);
				Putchr(')', output);
			}
			(void)fprintf(output.fp, "((int)("), Putl(output, 0);
			eexpr(tp->U.V30.taparm);
			if (td == dpred)
				(void)fprintf(output.fp, ")-1))"), Putl(output, 0);
			else
				(void)fprintf(output.fp, ")+1))"), Putl(output, 0);
		}
		break ;
	  case dodd:
		Putchr('(', output);
		printid(defnams.A[(int)(dboolean)]->U.V6.lid);
		(void)fprintf(output.fp, ")(("), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		(void)fprintf(output.fp, ") & 1)"), Putl(output, 0);
		break ;
	  case dsqr:
		tq = typeof(tp->U.V30.taparm);
		if ((tq == typnods.A[(int)(tinteger)]) || (tq->tt == nsubrange)) {
			(void)fprintf(output.fp, "(("), Putl(output, 0);
			eexpr(tp->U.V30.taparm);
			(void)fprintf(output.fp, ") * ("), Putl(output, 0);
			eexpr(tp->U.V30.taparm);
			(void)fprintf(output.fp, "))"), Putl(output, 0);
		} else {
			(void)fprintf(output.fp, "pow("), Putl(output, 0);
			if (typeof(tp->U.V30.taparm) != typnods.A[(int)(treal)])
				(void)fprintf(output.fp, "%s", dblcast), Putl(output, 0);
			eexpr(tp->U.V30.taparm);
			(void)fprintf(output.fp, ", 2.0)"), Putl(output, 0);
		}
		break ;
	  case dround:
		(void)fprintf(output.fp, "Round("), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		Putchr(')', output);
		break ;
	  case dtrunc:
		(void)fprintf(output.fp, "Trunc("), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		Putchr(')', output);
		break ;
	  case dpack:
		tq = typeof(tp->U.V30.taparm);
		tx = typeof(tp->U.V30.taparm->tnext->tnext);
		(void)fprintf(output.fp, "{    %s%s%c_j, _i = ", registr, inttyp, tab1), Putl(output, 0);
		if (!arithexpr(tp->U.V30.taparm->tnext))
			(void)fprintf(output.fp, "(int)"), Putl(output, 0);
		eexpr(tp->U.V30.taparm->tnext);
		if (tx->tt == narray)
			(void)fprintf(output.fp, " - %1d", clower(tq->U.V23.taindx)), Putl(output, 0);
		Putchr(';', output),Putchr('\n', output);
		indent();
		(void)fprintf(output.fp, "    for (_j = 0; _j < "), Putl(output, 0);
		if (tq->tt == nconfarr) {
			(void)fprintf(output.fp, "(int)("), Putl(output, 0);
			printid(tx->U.V22.tcindx->U.V19.thi->U.V43.tsym->U.V6.lid);
			Putchr(')', output);
		} else
			(void)fprintf(output.fp, "%1d", crange(tx->U.V23.taindx)), Putl(output, 0);
		(void)fprintf(output.fp, "; )\n"), Putl(output, 1);
		indent();
		Putchr(tab1, output);
		eexpr(tp->U.V30.taparm->tnext->tnext);
		(void)fprintf(output.fp, ".A[_j++] = "), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		(void)fprintf(output.fp, ".A[_i++];\n"), Putl(output, 1);
		indent();
		Putchr('}', output),Putchr('\n', output);
		break ;
	  case dunpack:
		tq = typeof(tp->U.V30.taparm);
		tx = typeof(tp->U.V30.taparm->tnext);
		(void)fprintf(output.fp, "{   %s%s%c_j, _i = ", registr, inttyp, tab1), Putl(output, 0);
		if (!arithexpr(tp->U.V30.taparm->tnext->tnext))
			(void)fprintf(output.fp, "(int)"), Putl(output, 0);
		eexpr(tp->U.V30.taparm->tnext->tnext);
		if (tx->tt != nconfarr)
			(void)fprintf(output.fp, " - %1d", clower(tx->U.V23.taindx)), Putl(output, 0);
		Putchr(';', output),Putchr('\n', output);
		indent();
		(void)fprintf(output.fp, "    for (_j = 0; _j < "), Putl(output, 0);
		if (tq->tt == nconfarr) {
			(void)fprintf(output.fp, "(int)("), Putl(output, 0);
			printid(tq->U.V22.tcindx->U.V19.thi->U.V43.tsym->U.V6.lid);
			Putchr(')', output);
		} else
			(void)fprintf(output.fp, "%1d", crange(tq->U.V23.taindx)), Putl(output, 0);
		(void)fprintf(output.fp, "; )\n"), Putl(output, 1);
		indent();
		Putchr(tab1, output);
		eexpr(tp->U.V30.taparm->tnext);
		(void)fprintf(output.fp, ".A[_i++] = "), Putl(output, 0);
		eexpr(tp->U.V30.taparm);
		(void)fprintf(output.fp, ".A[_j++];\n"), Putl(output, 1);
		indent();
		Putchr('}', output),Putchr('\n', output);
		break ;
	  default:
		Caseerror(Line);
	}
	G191_nelems = F192;
}

 void
eaddr(tp)
	treeptr	tp;
{
	Putchr('&', output);
	if (!(Member((unsigned)(tp->tt), Conset[141])))
		error(evarpar);
	eexpr(tp);
}

 void
ecall(tp)
	treeptr	tp;
{
	treeptr	tf, tq, tx;

	tf = idup(tp->U.V30.tcall);
	switch (tf->tt) {
	  case nproc:  case nfunc:
		tf = tf->U.V13.tsubpar;
		break ;
	  case nparproc:  case nparfunc:
		tf = tf->U.V15.tparparm;
		break ;
	  default:
		Caseerror(Line);
	}
	if (tf != (struct S61 *)NIL) {
		switch (tf->tt) {
		  case nvalpar:  case nvarpar:
			tf = tf->U.V14.tidl;
			break ;
		  case nparproc:  case nparfunc:
			tf = tf->U.V15.tparid;
			break ;
		  default:
			Caseerror(Line);
		}
	}
	eexpr(tp->U.V30.tcall);
	Putchr('(', output);
	tq = tp->U.V30.taparm;
	while (tq != (struct S61 *)NIL) {
		if (Member((unsigned)(tf->tup->tt), Conset[142])) {
			if (tq->tt == ncall)
				printid(tq->U.V30.tcall->U.V43.tsym->U.V6.lid);
			else
				printid(tq->U.V43.tsym->U.V6.lid);
		} else {
			tx = typeof(tq);
			if (tx == typnods.A[(int)(tboolean)]) {
				tx = tq;
				while (tx->tt == nuplus)
					tx = tx->U.V42.texps;
				if (Member((unsigned)(tx->tt), Conset[143])) {
					Putchr('(', output);
					printid(defnams.A[(int)(dboolean)]->U.V6.lid);
					(void)fprintf(output.fp, ")("), Putl(output, 0);
					eexpr(tq);
					Putchr(')', output);
				} else
					eexpr(tq);
			} else
				if ((tx == typnods.A[(int)(tstring)]) || (tx == typnods.A[(int)(tset)])) {
					(void)fprintf(output.fp, "*(("), Putl(output, 0);
					etypedef(tf->tup->U.V14.tbind);
					(void)fprintf(output.fp, " *)"), Putl(output, 0);
					if (tx == typnods.A[(int)(tset)]) {
						(*G198_dropset) = true;
						eexpr(tq);
						(*G198_dropset) = false;
					} else
						eexpr(tq);
					Putchr(')', output);
				} else
					if (tx == typnods.A[(int)(tnil)]) {
						Putchr('(', output);
						etypedef(tf->tup->U.V14.tbind);
						(void)fprintf(output.fp, ")NIL"), Putl(output, 0);
					} else
						if (tf->tup->U.V14.tbind->tt == nconfarr) {
							(void)fprintf(output.fp, "(struct "), Putl(output, 0);
							printid(tf->tup->U.V14.tbind->U.V22.tcuid);
							(void)fprintf(output.fp, " *)&"), Putl(output, 0);
							eexpr(tq);
							if (tq->tnext == (struct S61 *)NIL)
								(void)fprintf(output.fp, ", %1d", crange(tx->U.V23.taindx)), Putl(output, 0);
						} else {
							if (tf->tup->tt == nvarpar)
								eaddr(tq);
							else
								eexpr(tq);
						}
		}
		tq = tq->tnext;
		if (tq != (struct S61 *)NIL) {
			(void)fprintf(output.fp, ", "), Putl(output, 0);
			if (tf->tnext == (struct S61 *)NIL) {
				tf = tf->tup->tnext;
				switch (tf->tt) {
				  case nvalpar:  case nvarpar:
					tf = tf->U.V14.tidl;
					break ;
				  case nparproc:  case nparfunc:
					tf = tf->U.V15.tparid;
					break ;
				  default:
					Caseerror(Line);
				}
			} else
				tf = tf->tnext;
		}
	}
	Putchr(')', output);
}

void eexpr();

boolean constset();

 boolean
constxps(tp)
	treeptr	tp;
{
	register boolean	R173;

	switch (tp->tt) {
	  case nrange:
		if (constxps(tp->U.V41.texpr))
			R173 = constxps(tp->U.V41.texpl);
		else
			R173 = false;
		break ;
	  case nempty:  case ninteger:  case nchar:
		R173 = true;
		break ;
	  case nid:
		tp = idup(tp);
		R173 = (boolean)((tp->tt == nconst) || (tp->tt == nscalar));
		break ;
	  case nin:  case neq:  case nne:  case nlt:
	  case nle:  case ngt:  case nge:  case nor:
	  case nplus:  case nminus:  case nand:  case nmul:
	  case ndiv:  case nmod:  case nquot:  case nnot:
	  case numinus:  case nuplus:  case nset:  case nindex:
	  case nselect:  case nderef:  case ncall:  case nreal:
	  case nstring:  case nnil:
		R173 = false;
		break ;
	  default:
		Caseerror(Line);
	}
	return R173;
}

 boolean
constset(tp)
	treeptr	tp;
{
	register boolean	R172;

	R172 = true;
	while (tp != (struct S61 *)NIL)
		if (constxps(tp))
			tp = tp->tnext;
		else {
			R172 = false;
			tp = (struct S61 *)NIL;
		}
	return R172;
}
