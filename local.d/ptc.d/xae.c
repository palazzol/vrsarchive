#include "globals.h"

 void
emit()
{
	static char	usigned[]	= "unsigned ";
	boolean	conflag, setused, dropset, donearr;
	integer	doarrow, indnt;
	boolean	*F195;
	boolean	*F197;
	boolean	*F199;
	boolean	*F201;
	integer	*F203;
	integer	*F205;

	F205 = G204_indnt;
	G204_indnt = &indnt;
	F203 = G202_doarrow;
	G202_doarrow = &doarrow;
	F201 = G200_donearr;
	G200_donearr = &donearr;
	F199 = G198_dropset;
	G198_dropset = &dropset;
	F197 = G196_setused;
	G196_setused = &setused;
	F195 = G194_conflag;
	G194_conflag = &conflag;
	(*G204_indnt) = 0;
	varno = 0;
	(*G194_conflag) = false;
	(*G196_setused) = false;
	(*G198_dropset) = false;
	(*G202_doarrow) = 0;
	eprogram(top);
	if (usebool)
		(void)fprintf(output.fp, "%s%c*Bools[] = { \"false\", \"true\" };\n", chartyp, tab1), Putl(output, 1);
	if (usescan) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s%s\n", C50_static, voidtyp), Putl(output, 1);
		(void)fprintf(output.fp, "Scanck(n)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%s%cn;\n", inttyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%cif (n != 1) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s%sfprintf(stderr, \"Bad input\\n\");\n", tab2, voidcast), Putl(output, 1);
		(void)fprintf(output.fp, "%sexit(1);\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (usegetl) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s%s\n", C50_static, voidtyp), Putl(output, 1);
		(void)fprintf(output.fp, "Getl(f)\n"), Putl(output, 1);
		(void)fprintf(output.fp, " text%c*f;\n", tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%cwhile (f->eoln == 0)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sGetx(*f);\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%cGetx(*f);\n", tab1), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (usefopn) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%sFILE *\n", C50_static), Putl(output, 1);
		(void)fprintf(output.fp, "Fopen(n, m)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%s%c*n, *m;\n", chartyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%cFILE%s*f;\n", tab1, tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%c*s;\n", tab1, registr, chartyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%cch = %cA%c;\n", tab1, C50_static, chartyp, tab1, quote, quote), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%ctmp[MAXFILENAME];\n", tab1, C50_static, chartyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%cunlink();\n", tab1, xtern, inttyp, tab1), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%cif (n == NULL)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%ssprintf(tmp, %sch++);\n", tab2, tmpfilename), Putl(output, 1);
		(void)fprintf(output.fp, "%celse {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sstrncpy(tmp, n, sizeof(tmp));\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sfor (s = &tmp[sizeof(tmp)-1]; *s == %s || *s == %s; )\n", tab2, spchr, nulchr), Putl(output, 1);
		(void)fprintf(output.fp, "%s*s-- = %s;\n", tab3, nulchr), Putl(output, 1);
		(void)fprintf(output.fp, "%sif (tmp[sizeof(tmp)-1]) {\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%s%sfprintf(stderr, \"Too long filename %c%%s%c\\n\", n);\n", tab3, voidcast, quote, quote), Putl(output, 1);
		(void)fprintf(output.fp, "%sexit(1);\n", tab3), Putl(output, 1);
		(void)fprintf(output.fp, "%s}\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cs = tmp;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cif ((f = fopen(s, m)) == NULL) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s%sfprintf(stderr, \"Cannot open: %%s\\n\", s);\n", tab2, voidcast), Putl(output, 1);
		(void)fprintf(output.fp, "%sexit(1);\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cif (n == NULL)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sunlink(tmp);\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%creturn (f);\n", tab1), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%s%s%crewind();\n", xtern, inttyp, tab1), Putl(output, 1);
	}
	if (setcnt > 0)
		econset(setlst, setcnt);
	if (useunion) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s%s\n", C50_static, setptyp), Putl(output, 1);
		(void)fprintf(output.fp, "Union(p1, p2)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%cp1, p2;\n", tab1, registr, setptyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s%si, j, k;\n", tab1, registr, inttyp, tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%ssp = Newset(),\n", tab1, registr, setptyp, tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sp3 = sp;\n", tab4), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%cj = *p1;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%c*p3 = j;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cif (j > *p2)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sj = *p2;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%celse\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s*p3 = *p2;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%ck = *p1 - *p2;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cp1++, p2++, p3++;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cfor (i = 0; i < j; i++)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s*p3++ = (*p1++ | *p2++);\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (k > 0) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s*p3++ = *p1++;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sk--;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (k < 0) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s*p3++ = *p2++;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sk++;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%creturn (Saveset(sp));\n", tab1), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (usediff) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s%s\n", C50_static, setptyp), Putl(output, 1);
		(void)fprintf(output.fp, "Diff(p1, p2)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%cp1, p2;\n", tab1, registr, setptyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s%si, j, k;\n", tab1, registr, inttyp, tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%ssp = Newset(),\n", tab1, registr, setptyp, tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sp3 = sp;\n", tab4), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%cj = *p1;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%c*p3 = j;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cif (j > *p2)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sj = *p2;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%ck = *p1 - *p2;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cp1++, p2++, p3++;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cfor (i = 0; i < j; i++)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s*p3++ = (*p1++ & ~ (*p2++));\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (k > 0) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s*p3++ = *p1++;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sk--;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%creturn (Saveset(sp));\n", tab1), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (useintr) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s%s\n", C50_static, setptyp), Putl(output, 1);
		(void)fprintf(output.fp, "Inter(p1, p2)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%cp1, p2;\n", tab1, registr, setptyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s%si, j, k;\n", tab1, registr, inttyp, tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%ssp = Newset(),\n", tab1, registr, setptyp, tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sp3 = sp;\n", tab4), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%cif ((j = *p1) > *p2)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sj = *p2;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c*p3 = j;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cp1++, p2++, p3++;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cfor (i = 0; i < j; i++)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s*p3++ = (*p1++ & *p2++);\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%creturn (Saveset(sp));\n", tab1), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (usememb) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s", C50_static), Putl(output, 0);
		printid(defnams.A[(int)(dboolean)]->U.V6.lid);
		Putchr('\n', output);
		(void)fprintf(output.fp, "Member(m, sp)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%s%cm;\n", tab1, registr, usigned, inttyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%csp;\n", tab1, registr, setptyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s%s%ci = m / (setbits+1) + 1;\n", tab1, registr, usigned, inttyp, tab1), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%cif ((i <= *sp) && (sp[i] & (1 << (m %% (setbits+1)))))\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (", tab2), Putl(output, 0);
		printid(defnams.A[(int)(dtrue)]->U.V6.lid);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%creturn (", tab1), Putl(output, 0);
		printid(defnams.A[(int)(dfalse)]->U.V6.lid);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (useseq || usesne) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s", C50_static), Putl(output, 0);
		printid(defnams.A[(int)(dboolean)]->U.V6.lid);
		Putchr('\n', output);
		(void)fprintf(output.fp, "Eq(p1, p2)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%cp1, p2;\n", tab1, registr, setptyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s%ci, j;\n", tab1, registr, inttyp, tab1), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%ci = *p1++;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cj = *p2++;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (i != 0 && j != 0) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sif (*p1++ != *p2++)\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (", tab3), Putl(output, 0);
		printid(defnams.A[(int)(dfalse)]->U.V6.lid);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%si--, j--;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (i != 0) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sif (*p1++ != 0)\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (", tab3), Putl(output, 0);
		printid(defnams.A[(int)(dfalse)]->U.V6.lid);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%si--;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (j != 0) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sif (*p2++ != 0)\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (", tab3), Putl(output, 0);
		printid(defnams.A[(int)(dfalse)]->U.V6.lid);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%sj--;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%creturn (", tab1), Putl(output, 0);
		printid(defnams.A[(int)(dtrue)]->U.V6.lid);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (usesne) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s", C50_static), Putl(output, 0);
		printid(defnams.A[(int)(dboolean)]->U.V6.lid);
		Putchr('\n', output);
		(void)fprintf(output.fp, "Ne(p1, p2)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%cp1, p2;\n", tab1, registr, setptyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%creturn (!Eq(p1, p2));", tab1), Putl(output, 0);
		Putchr('}', output),Putchr('\n', output);
	}
	if (usesle) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s", C50_static), Putl(output, 0);
		printid(defnams.A[(int)(dboolean)]->U.V6.lid);
		Putchr('\n', output);
		(void)fprintf(output.fp, "Le(p1, p2)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%cp1, p2;\n", tab1, registr, setptyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s%ci, j;\n", tab1, registr, inttyp, tab1), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%ci = *p1++;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cj = *p2++;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (i != 0 && j != 0) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sif ((*p1++ & ~ *p2++) != 0)\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (", tab3), Putl(output, 0);
		printid(defnams.A[(int)(dfalse)]->U.V6.lid);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%si--, j--;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (i != 0) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sif (*p1++ != 0)\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (", tab3), Putl(output, 0);
		printid(defnams.A[(int)(dfalse)]->U.V6.lid);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%si--;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%creturn (", tab1), Putl(output, 0);
		printid(defnams.A[(int)(dtrue)]->U.V6.lid);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (usesge) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s", C50_static), Putl(output, 0);
		printid(defnams.A[(int)(dboolean)]->U.V6.lid);
		Putchr('\n', output);
		(void)fprintf(output.fp, "Ge(p1, p2)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%cp1, p2;\n", tab1, registr, setptyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s%ci, j;\n", tab1, registr, inttyp, tab1), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%ci = *p1++;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cj = *p2++;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (i != 0 && j != 0) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sif ((*p2++ & ~ *p1++) != 0)\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (false);\n", tab3), Putl(output, 1);
		(void)fprintf(output.fp, "%si--, j--;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (j != 0) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sif (*p2++ != 0)\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (", tab3), Putl(output, 0);
		printid(defnams.A[(int)(dfalse)]->U.V6.lid);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%sj--;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%creturn (", tab1), Putl(output, 0);
		printid(defnams.A[(int)(dtrue)]->U.V6.lid);
		(void)fprintf(output.fp, ");\n"), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (usemksub) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s%s\n", C50_static, setptyp), Putl(output, 1);
		(void)fprintf(output.fp, "Mksubr(lo, hi, sp)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%s%clo, hi;\n", tab1, registr, usigned, inttyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%csp;\n", tab1, registr, setptyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s%ci, k;\n", tab1, registr, inttyp, tab1), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%cif (hi < lo)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (sp);\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%ci = hi / (setbits+1) + 1;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cfor (k = *sp + 1; k <= i; k++)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%ssp[k] = 0;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%cif (*sp < i)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s*sp = i;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%cfor (k = lo; k <= hi; k++)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%ssp[k / (setbits+1) + 1] |= (1 << (k %% (setbits+1)));\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%creturn (sp);\n", tab1), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (useins) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s%s\n", C50_static, setptyp), Putl(output, 1);
		(void)fprintf(output.fp, "Insmem(m, sp)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%s%cm;\n", tab1, registr, usigned, inttyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%csp;\n", tab1, registr, setptyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s%ci,\n", tab1, registr, inttyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s%cj = m / (setbits+1) + 1;\n", tab3, tab1), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%cif (*sp < j)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sfor (i = *sp + 1, *sp = j; i <= *sp; i++)\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%ssp[i] = 0;\n", tab3), Putl(output, 1);
		(void)fprintf(output.fp, "%csp[j] |= (1 << (m %% (setbits+1)));\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%creturn (sp);\n", tab1), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (usesets) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%sSETSPACE\n", ifndef), Putl(output, 1);
		(void)fprintf(output.fp, "%sSETSPACE 256\n", C4_define), Putl(output, 1);
		(void)fprintf(output.fp, "%s\n", endif), Putl(output, 1);
		(void)fprintf(output.fp, "%s%s\n", C50_static, setptyp), Putl(output, 1);
		(void)fprintf(output.fp, "Currset(n,sp)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%cn;\n", tab1, inttyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%csp;\n", tab1, setptyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s%cSpace[SETSPACE];\n", tab1, C50_static, setwtyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%cTop = Space;\n", tab1, C50_static, setptyp, tab1), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%cswitch (n) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%c  case 0:\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sTop = Space;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (0);\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c  case 1:\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sif (&Space[SETSPACE] - Top <= %1d) {\n", tab2, maxsetrange), Putl(output, 1);
		(void)fprintf(output.fp, "%s%sfprintf(stderr, \"Set-space exhausted\\n\");\n", tab3, voidcast), Putl(output, 1);
		(void)fprintf(output.fp, "%sexit(1);\n", tab3), Putl(output, 1);
		(void)fprintf(output.fp, "%s}\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%s*Top = 0;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (Top);\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c  case 2:\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sif (Top <= &sp[*sp])\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%sTop = &sp[*sp + 1];\n", tab3), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (sp);\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%c/* NOTREACHED */\n", tab1), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (usescpy) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s%s\n", C50_static, voidtyp), Putl(output, 1);
		(void)fprintf(output.fp, "Setncpy(S1, S2, N)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%cS1, S2;\n", tab1, registr, setptyp, tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%s%s%cN;\n", tab1, registr, usigned, inttyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s%s%cm;\n", tab1, registr, usigned, inttyp, tab1), Putl(output, 1);
		Putchr('\n', output);
		(void)fprintf(output.fp, "%cN /= sizeof(%s);\n", tab1, setwtyp), Putl(output, 1);
		(void)fprintf(output.fp, "%c*S1++ = --N;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cm = *S2++;\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (m != 0 && N != 0) {\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s*S1++ = *S2++;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%s--N;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%s--m;\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%c}\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%cwhile (N-- != 0)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%s*S1++ = 0;\n", tab2), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (usecase) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s%s\n", C50_static, voidtyp), Putl(output, 1);
		(void)fprintf(output.fp, "Caseerror(n)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%cn;\n", tab1, inttyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%sfprintf(stderr, \"Missing case limb: line %%d\\n\", n);\n", tab1, voidcast), Putl(output, 1);
		(void)fprintf(output.fp, "%cexit(1);\n", tab1), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (usemax) {
		Putchr('\n', output);
		(void)fprintf(output.fp, "%s%s\n", C50_static, inttyp), Putl(output, 1);
		(void)fprintf(output.fp, "Max(m, n)\n"), Putl(output, 1);
		(void)fprintf(output.fp, "%c%s%cm, n;\n", tab1, inttyp, tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%cif (m > n)\n", tab1), Putl(output, 1);
		(void)fprintf(output.fp, "%sreturn (m);\n", tab2), Putl(output, 1);
		(void)fprintf(output.fp, "%creturn (n);\n", tab1), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (use(dtrunc)) {
		(void)fprintf(output.fp, "%s%s\n", C50_static, inttyp), Putl(output, 1);
		(void)fprintf(output.fp, "Trunc(f)\n"), Putl(output, 1);
		printid(defnams.A[(int)(dreal)]->U.V6.lid);
		(void)fprintf(output.fp, "%cf;\n", tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%creturn f;\n", tab1), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	if (use(dround)) {
		(void)fprintf(output.fp, "%s%s\n", C50_static, inttyp), Putl(output, 1);
		(void)fprintf(output.fp, "Round(f)\n"), Putl(output, 1);
		printid(defnams.A[(int)(dreal)]->U.V6.lid);
		(void)fprintf(output.fp, "%cf;\n", tab1), Putl(output, 1);
		Putchr('{', output),Putchr('\n', output);
		(void)fprintf(output.fp, "%c%s%s floor();\n", tab1, xtern, doubletyp), Putl(output, 1);
		(void)fprintf(output.fp, "%creturn floor(%s(0.5+f));\n", tab1, dblcast), Putl(output, 1);
		Putchr('}', output),Putchr('\n', output);
	}
	G194_conflag = F195;
	G196_setused = F197;
	G198_dropset = F199;
	G200_donearr = F201;
	G202_doarrow = F203;
	G204_indnt = F205;
}

void initialize();

 void
defname(cn, str)
	cnames	cn;
	keyword	str;
{
	toknbuf	w;
	register toknidx	i;

	{   register int	_j, _i = 1 - 1;
	    for (_j = 0; _j < 10; )
		w.A[_i++] = str.A[_j++];
	}
	{
		toknidx	B67 = 1,
			B68 = keywordlen;

		if (B67 <= B68)
			for (i = B67; ; i++) {
				if (w.A[i - 1] == space) {
					w.A[i - 1] = null;
					goto L999;
				}
				if (i == B68) break;
			}
	}
	w.A[keywordlen + 1 - 1] = null;
L999:
	ctable.A[(int)(cn)] = saveid(&w);
}

 void
defid(nt, did, str)
	treetyp	nt;
	predefs	did;
	keyword	str;
{
	toknbuf	w;
	register toknidx	i;
	treeptr	tp, tq, tv;

	{
		toknidx	B69 = 1,
			B70 = keywordlen;

		if (B69 <= B70)
			for (i = B69; ; i++) {
				if (str.A[i - 1] == space) {
					w.A[i - 1] = null;
					goto L999;
				} else
					w.A[i - 1] = str.A[i - 1];
				if (i == B70) break;
			}
	}
	w.A[keywordlen + 1 - 1] = null;
L999:
	tp = newid(saveid(&w));
	defnams.A[(int)(did)] = tp->U.V43.tsym;
	if (Member((unsigned)(nt), Conset[162])) {
		tv = mknode(npredef);
		tv->U.V12.tdef = did;
		tv->U.V12.tobtyp = tnone;
	} else
		tv = (struct S61 *)NIL;
	switch (nt) {
	  case nscalar:
		tv = mknode(nscalar);
		tv->U.V17.tscalid = (struct S61 *)NIL;
		tq = mknode(ntype);
		tq->U.V14.tbind = tv;
		tq->U.V14.tidl = tp;
		tp = tq;
		break ;
	  case nconst:  case ntype:  case nfield:  case nvar:
		tq = mknode(nt);
		tq->U.V14.tbind = tv;
		tq->U.V14.tidl = tp;
		tq->U.V14.tattr = anone;
		tp = tq;
		break ;
	  case nfunc:  case nproc:
		tq = mknode(nt);
		tq->U.V13.tsubid = tp;
		tq->U.V13.tsubstmt = tv;
		tq->U.V13.tfuntyp = (struct S61 *)NIL;
		tq->U.V13.tsubpar = (struct S61 *)NIL;
		tq->U.V13.tsublab = (struct S61 *)NIL;
		tq->U.V13.tsubconst = (struct S61 *)NIL;
		tq->U.V13.tsubtype = (struct S61 *)NIL;
		tq->U.V13.tsubvar = (struct S61 *)NIL;
		tq->U.V13.tsubsub = (struct S61 *)NIL;
		tq->U.V13.tscope = (struct S60 *)NIL;
		tq->U.V13.tstat = 0;
		tp = tq;
		break ;
	  case nid:
		break ;
	  default:
		Caseerror(Line);
	}
	deftab.A[(int)(did)] = tp;
}

 void
defkey(s, w)
	symtyp	s;
	keyword	w;
{
	register unsigned char	i;

	{
		unsigned char	B71 = 1,
			B72 = keywordlen;

		if (B71 <= B72)
			for (i = B71; ; i++) {
				if (w.A[i - 1] == space)
					w.A[i - 1] = null;
				if (i == B72) break;
			}
	}
	{
		register struct S206 *W73 = &keytab.A[(unsigned)(s)];

		W73->wrd = w;
		W73->sym = s;
	}
}

 void
fixinit(i)
	strindx	i;
{
	toknbuf	t;

	gettokn(i, &t);
	t.A[1 - 1] = 'i';
	puttokn(i, &t);
}

 void
defmach(lo, hi, str)
	long	lo, hi;
	machdefstr	str;
{
	register toknidx	i;
	toknbuf	w;

	{   register int	_j, _i = 1 - 1;
	    for (_j = 0; _j < 16; )
		w.A[_i++] = str.A[_j++];
	}
	if (w.A[machdeflen - 1] != space)
		error(ebadmach);
	{
		toknidx	B74 = machdeflen - 1,
			B75 = 1;

		if (B74 >= B75)
			for (i = B74; ; i--) {
				if (w.A[i - 1] != space) {
					w.A[i + 1 - 1] = null;
					goto L999;
				}
				if (i == B75) break;
			}
	}
	error(ebadmach);
L999:
	if (nmachdefs >= maxmachdefs)
		error(emanymachs);
	nmachdefs = nmachdefs + 1;
	{
		register struct S193 *W76 = &machdefs.A[nmachdefs - 1];

		W76->lolim = lo;
		W76->hilim = hi;
		W76->typstr = savestr(&w);
	}
}

 void
initstrstore()
{
	register strbcnt	i;

	{
		strbcnt	B77 = 1,
			B78 = maxblkcnt;

		if (B77 <= B78)
			for (i = B77; ; i++) {
				strstor.A[i] = (strblk *)NIL;
				if (i == B78) break;
			}
	}
	strstor.A[0] = (strblk *)malloc((unsigned)(sizeof(*strstor.A[0])));
	strstor.A[0]->A[0] = null;
	strfree = 1;
	strleft = maxstrblk;
}


 void
initialize()
{
	register hashtyp	s;
	register pretyps	t;
	register predefs	d;

	lineno = 1;
	colno = 0;
	initstrstore();
	setlst = (struct S61 *)NIL;
	setcnt = 0;
	(void)strncpy(hexdig.A, "0123456789ABCDEF", sizeof(hexdig.A));
	symtab = (struct S60 *)NIL;
	statlvl = 0;
	maxlevel = -1;
	enterscope((declptr)NIL);
	varno = 0;
	usenilp = false;
	usesets = false;
	useunion = false;
	usediff = false;
	usemksub = false;
	useintr = false;
	usesge = false;
	usesle = false;
	usesne = false;
	useseq = false;
	usememb = false;
	useins = false;
	usescpy = false;
	usefopn = false;
	usescan = false;
	usegetl = false;
	usecase = false;
	usejmps = false;
	usebool = false;
	usecomp = false;
	usemax = false;
	{
		hashtyp	B79 = 0,
			B80 = hashmax;

		if (B79 <= B80)
			for (s = B79; ; s++) {
				idtab.A[s] = (struct S59 *)NIL;
				if (s == B80) break;
			}
	}
	{
		predefs	B81 = dabs,
			B82 = dztring;

		if ((int)(B81) <= (int)(B82))
			for (d = B81; ; d = (predefs)((int)(d)+1)) {
				deftab.A[(int)(d)] = (struct S61 *)NIL;
				defnams.A[(int)(d)] = (struct S62 *)NIL;
				if (d == B82) break;
			}
	}
	defkey(sand, *((keyword *)"and       "));
	defkey(sarray, *((keyword *)"array     "));
	defkey(sbegin, *((keyword *)"begin     "));
	defkey(scase, *((keyword *)"case      "));
	defkey(sconst, *((keyword *)"const     "));
	defkey(sdiv, *((keyword *)"div       "));
	defkey(sdo, *((keyword *)"do        "));
	defkey(sdownto, *((keyword *)"downto    "));
	defkey(selse, *((keyword *)"else      "));
	defkey(send, *((keyword *)"end       "));
	defkey(sextern, *((keyword *)externsym));
	defkey(sfile, *((keyword *)"file      "));
	defkey(sfor, *((keyword *)"for       "));
	defkey(sforward, *((keyword *)"forward   "));
	defkey(sfunc, *((keyword *)"function  "));
	defkey(sgoto, *((keyword *)"goto      "));
	defkey(sif, *((keyword *)"if        "));
	defkey(sinn, *((keyword *)"in        "));
	defkey(slabel, *((keyword *)"label     "));
	defkey(smod, *((keyword *)"mod       "));
	defkey(snil, *((keyword *)"nil       "));
	defkey(snot, *((keyword *)"not       "));
	defkey(sof, *((keyword *)"of        "));
	defkey(sor, *((keyword *)"or        "));
	defkey(sother, *((keyword *)othersym));
	defkey(spacked, *((keyword *)"packed    "));
	defkey(sproc, *((keyword *)"procedure "));
	defkey(spgm, *((keyword *)"program   "));
	defkey(srecord, *((keyword *)"record    "));
	defkey(srepeat, *((keyword *)"repeat    "));
	defkey(sset, *((keyword *)"set       "));
	defkey(sthen, *((keyword *)"then      "));
	defkey(sto, *((keyword *)"to        "));
	defkey(stype, *((keyword *)"type      "));
	defkey(suntil, *((keyword *)"until     "));
	defkey(svar, *((keyword *)"var       "));
	defkey(swhile, *((keyword *)"while     "));
	defkey(swith, *((keyword *)"with      "));
	defkey(seof, *((keyword *)dummysym));
	cprio.A[(int)(nformat) - (int)(nassign)] = 0;
	cprio.A[(int)(nrange) - (int)(nassign)] = 0;
	cprio.A[(int)(nin) - (int)(nassign)] = 0;
	cprio.A[(int)(nset) - (int)(nassign)] = 0;
	cprio.A[(int)(nassign) - (int)(nassign)] = 0;
	cprio.A[(int)(nor) - (int)(nassign)] = 1;
	cprio.A[(int)(nand) - (int)(nassign)] = 2;
	cprio.A[(int)(neq) - (int)(nassign)] = 3;
	cprio.A[(int)(nne) - (int)(nassign)] = 3;
	cprio.A[(int)(nlt) - (int)(nassign)] = 3;
	cprio.A[(int)(nle) - (int)(nassign)] = 3;
	cprio.A[(int)(ngt) - (int)(nassign)] = 3;
	cprio.A[(int)(nge) - (int)(nassign)] = 3;
	cprio.A[(int)(nplus) - (int)(nassign)] = 4;
	cprio.A[(int)(nminus) - (int)(nassign)] = 4;
	cprio.A[(int)(nmul) - (int)(nassign)] = 5;
	cprio.A[(int)(ndiv) - (int)(nassign)] = 5;
	cprio.A[(int)(nmod) - (int)(nassign)] = 5;
	cprio.A[(int)(nquot) - (int)(nassign)] = 5;
	cprio.A[(int)(nnot) - (int)(nassign)] = 6;
	cprio.A[(int)(numinus) - (int)(nassign)] = 6;
	cprio.A[(int)(nuplus) - (int)(nassign)] = 7;
	cprio.A[(int)(nindex) - (int)(nassign)] = 7;
	cprio.A[(int)(nselect) - (int)(nassign)] = 7;
	cprio.A[(int)(nderef) - (int)(nassign)] = 7;
	cprio.A[(int)(ncall) - (int)(nassign)] = 7;
	cprio.A[(int)(nid) - (int)(nassign)] = 7;
	cprio.A[(int)(nchar) - (int)(nassign)] = 7;
	cprio.A[(int)(ninteger) - (int)(nassign)] = 7;
	cprio.A[(int)(nreal) - (int)(nassign)] = 7;
	cprio.A[(int)(nstring) - (int)(nassign)] = 7;
	cprio.A[(int)(nnil) - (int)(nassign)] = 7;
	pprio.A[(int)(nassign) - (int)(nassign)] = 0;
	pprio.A[(int)(nformat) - (int)(nassign)] = 0;
	pprio.A[(int)(nrange) - (int)(nassign)] = 1;
	pprio.A[(int)(nin) - (int)(nassign)] = 1;
	pprio.A[(int)(neq) - (int)(nassign)] = 1;
	pprio.A[(int)(nne) - (int)(nassign)] = 1;
	pprio.A[(int)(nlt) - (int)(nassign)] = 1;
	pprio.A[(int)(nle) - (int)(nassign)] = 1;
	pprio.A[(int)(ngt) - (int)(nassign)] = 1;
	pprio.A[(int)(nge) - (int)(nassign)] = 1;
	pprio.A[(int)(nor) - (int)(nassign)] = 2;
	pprio.A[(int)(nplus) - (int)(nassign)] = 2;
	pprio.A[(int)(nminus) - (int)(nassign)] = 2;
	pprio.A[(int)(nand) - (int)(nassign)] = 3;
	pprio.A[(int)(nmul) - (int)(nassign)] = 3;
	pprio.A[(int)(ndiv) - (int)(nassign)] = 3;
	pprio.A[(int)(nmod) - (int)(nassign)] = 3;
	pprio.A[(int)(nquot) - (int)(nassign)] = 3;
	pprio.A[(int)(nnot) - (int)(nassign)] = 4;
	pprio.A[(int)(numinus) - (int)(nassign)] = 4;
	pprio.A[(int)(nuplus) - (int)(nassign)] = 5;
	pprio.A[(int)(nset) - (int)(nassign)] = 6;
	pprio.A[(int)(nindex) - (int)(nassign)] = 6;
	pprio.A[(int)(nselect) - (int)(nassign)] = 6;
	pprio.A[(int)(nderef) - (int)(nassign)] = 6;
	pprio.A[(int)(ncall) - (int)(nassign)] = 6;
	pprio.A[(int)(nid) - (int)(nassign)] = 6;
	pprio.A[(int)(nchar) - (int)(nassign)] = 6;
	pprio.A[(int)(ninteger) - (int)(nassign)] = 6;
	pprio.A[(int)(nreal) - (int)(nassign)] = 6;
	pprio.A[(int)(nstring) - (int)(nassign)] = 6;
	pprio.A[(int)(nnil) - (int)(nassign)] = 6;
	defname(cabort, *((keyword *)"abort     "));
	defname(cbreak, *((keyword *)"break     "));
	defname(ccontinue, *((keyword *)"continue  "));
	defname(cdefine, *((keyword *)"define    "));
	defname(cdefault, *((keyword *)"default   "));
	defname(cdouble, *((keyword *)"double    "));
	defname(cedata, *((keyword *)"edata     "));
	defname(cenum, *((keyword *)"enum      "));
	defname(cetext, *((keyword *)"etext     "));
	defname(cextern, *((keyword *)"extern    "));
	defname(cfclose, *((keyword *)"fclose    "));
	defname(cfflush, *((keyword *)"fflush    "));
	defname(cfgetc, *((keyword *)"fgetc     "));
	defname(cfloat, *((keyword *)"float     "));
	defname(cfloor, *((keyword *)"floor     "));
	defname(cfprintf, *((keyword *)"fprintf   "));
	defname(cfputc, *((keyword *)"fputc     "));
	defname(cfread, *((keyword *)"fread     "));
	defname(cfscanf, *((keyword *)"fscanf    "));
	defname(cfwrite, *((keyword *)"fwrite    "));
	defname(cgetc, *((keyword *)"getc      "));
	defname(cgetpid, *((keyword *)"getpid    "));
	defname(cint, *((keyword *)"int       "));
	defname(cinclude, *((keyword *)"include   "));
	defname(clong, *((keyword *)"long      "));
	defname(clog, *((keyword *)"log       "));
	defname(cmain, *((keyword *)"main      "));
	defname(cmalloc, *((keyword *)"malloc    "));
	defname(cprintf, *((keyword *)"printf    "));
	defname(cpower, *((keyword *)"pow       "));
	defname(cputc, *((keyword *)"putc      "));
	defname(cread, *((keyword *)"read      "));
	defname(creturn, *((keyword *)"return    "));
	defname(cregister, *((keyword *)"register  "));
	defname(crewind, *((keyword *)"rewind    "));
	defname(cscanf, *((keyword *)"scanf     "));
	defname(csetbits, *((keyword *)"setbits   "));
	defname(csetword, *((keyword *)"setword   "));
	defname(csetptr, *((keyword *)"setptr    "));
	defname(cshort, *((keyword *)"short     "));
	defname(csigned, *((keyword *)"signed    "));
	defname(csizeof, *((keyword *)"sizeof    "));
	defname(csprintf, *((keyword *)"sprintf   "));
	defname(cstatic, *((keyword *)"static    "));
	defname(cstdin, *((keyword *)"stdin     "));
	defname(cstdout, *((keyword *)"stdout    "));
	defname(cstderr, *((keyword *)"stderr    "));
	defname(cstrncmp, *((keyword *)"strncmp   "));
	defname(cstrncpy, *((keyword *)"strncpy   "));
	defname(cstruct, *((keyword *)"struct    "));
	defname(cswitch, *((keyword *)"switch    "));
	defname(ctypedef, *((keyword *)"typedef   "));
	defname(cundef, *((keyword *)"undef     "));
	defname(cungetc, *((keyword *)"ungetc    "));
	defname(cunion, *((keyword *)"union     "));
	defname(cunlink, *((keyword *)"unlink    "));
	defname(cunsigned, *((keyword *)"unsigned  "));
	defname(cwrite, *((keyword *)"write     "));
	defid(nfunc, dabs, *((keyword *)"abs       "));
	defid(nfunc, darctan, *((keyword *)"arctan    "));
	defid(nvar, dargc, *((keyword *)"argc      "));
	defid(nproc, dargv, *((keyword *)"argv      "));
	defid(nscalar, dboolean, *((keyword *)"boolean   "));
	defid(ntype, dchar, *((keyword *)"char      "));
	defid(nfunc, dchr, *((keyword *)"chr       "));
	defid(nproc, dclose, *((keyword *)"close     "));
	defid(nfunc, dcos, *((keyword *)"cos       "));
	defid(nproc, ddispose, *((keyword *)"dispose   "));
	defid(nid, dfalse, *((keyword *)"false     "));
	defid(nfunc, deof, *((keyword *)"eof       "));
	defid(nfunc, deoln, *((keyword *)"eoln      "));
	defid(nproc, dexit, *((keyword *)"exit      "));
	defid(nfunc, dexp, *((keyword *)"exp       "));
	defid(nproc, dflush, *((keyword *)"flush     "));
	defid(nproc, dget, *((keyword *)"get       "));
	defid(nproc, dhalt, *((keyword *)"halt      "));
	defid(nvar, dinput, *((keyword *)"input     "));
	defid(ntype, dinteger, *((keyword *)"integer   "));
	defid(nfunc, dln, *((keyword *)"ln        "));
	defid(nconst, dmaxint, *((keyword *)"maxint    "));
	defid(nproc, dmessage, *((keyword *)"message   "));
	defid(nproc, dnew, *((keyword *)"new       "));
	defid(nfunc, dodd, *((keyword *)"odd       "));
	defid(nfunc, dord, *((keyword *)"ord       "));
	defid(nvar, doutput, *((keyword *)"output    "));
	defid(nproc, dpack, *((keyword *)"pack      "));
	defid(nproc, dpage, *((keyword *)"page      "));
	defid(nfunc, dpred, *((keyword *)"pred      "));
	defid(nproc, dput, *((keyword *)"put       "));
	defid(nproc, dread, *((keyword *)"read      "));
	defid(nproc, dreadln, *((keyword *)"readln    "));
	defid(ntype, dreal, *((keyword *)"real      "));
	defid(nproc, dreset, *((keyword *)"reset     "));
	defid(nproc, drewrite, *((keyword *)"rewrite   "));
	defid(nfunc, dround, *((keyword *)"round     "));
	defid(nfunc, dsin, *((keyword *)"sin       "));
	defid(nfunc, dsqr, *((keyword *)"sqr       "));
	defid(nfunc, dsqrt, *((keyword *)"sqrt      "));
	defid(nfunc, dsucc, *((keyword *)"succ      "));
	defid(ntype, dtext, *((keyword *)"text      "));
	defid(nid, dtrue, *((keyword *)"true      "));
	defid(nfunc, dtrunc, *((keyword *)"trunc     "));
	defid(nfunc, dtan, *((keyword *)"tan       "));
	defid(nproc, dunpack, *((keyword *)"unpack    "));
	defid(nproc, dwrite, *((keyword *)"write     "));
	defid(nproc, dwriteln, *((keyword *)"writeln   "));
	defid(nfield, dzinit, *((keyword *)"$nit      "));
	defid(ntype, dztring, *((keyword *)"$ztring   "));
	deftab.A[(int)(dboolean)]->U.V14.tbind->U.V17.tscalid = deftab.A[(int)(dfalse)];
	deftab.A[(int)(dfalse)]->tnext = deftab.A[(int)(dtrue)];
	currsym.st = sinteger;
	currsym.U.V3.vint = maxint;
	deftab.A[(int)(dmaxint)]->U.V14.tbind = mklit();
	deftab.A[(int)(dargc)]->U.V14.tbind = deftab.A[(int)(dinteger)]->U.V14.tbind;
	deftab.A[(int)(dinput)]->U.V14.tbind = deftab.A[(int)(dtext)]->U.V14.tbind;
	deftab.A[(int)(doutput)]->U.V14.tbind = deftab.A[(int)(dtext)]->U.V14.tbind;
	{
		pretyps	B83 = tnone,
			B84 = terror;

		if ((int)(B83) <= (int)(B84))
			for (t = B83; ; t = (pretyps)((int)(t)+1)) {
				switch (t) {
				  case tboolean:
					typnods.A[(int)(t)] = deftab.A[(int)(dboolean)];
					break ;
				  case tchar:
					typnods.A[(int)(t)] = deftab.A[(int)(dchar)]->U.V14.tbind;
					break ;
				  case tinteger:
					typnods.A[(int)(t)] = deftab.A[(int)(dinteger)]->U.V14.tbind;
					break ;
				  case treal:
					typnods.A[(int)(t)] = deftab.A[(int)(dreal)]->U.V14.tbind;
					break ;
				  case ttext:
					typnods.A[(int)(t)] = deftab.A[(int)(dtext)]->U.V14.tbind;
					break ;
				  case tstring:
					typnods.A[(int)(t)] = deftab.A[(int)(dztring)]->U.V14.tbind;
					break ;
				  case tnil:  case tset:  case tpoly:  case tnone:
					typnods.A[(int)(t)] = mknode(npredef);
					break ;
				  case terror:
					break ;
				  default:
					Caseerror(Line);
				}
				if (Member((unsigned)(t), Conset[163]))
					typnods.A[(int)(t)]->U.V12.tobtyp = t;
				if (t == B84) break;
			}
	}
	fixinit(defnams.A[(int)(dzinit)]->U.V6.lid->istr);
	deftab.A[(int)(dzinit)]->U.V14.tbind = deftab.A[(int)(dinteger)]->U.V14.tbind;
	{
		predefs	B85 = dabs,
			B86 = dztring;

		if ((int)(B85) <= (int)(B86))
			for (d = B85; ; d = (predefs)((int)(d)+1)) {
				linkup((treeptr)NIL, deftab.A[(int)(d)]);
				if (d == B86) break;
			}
	}
	deftab.A[(int)(dchr)]->U.V13.tfuntyp = typnods.A[(int)(tchar)];
	deftab.A[(int)(deof)]->U.V13.tfuntyp = typnods.A[(int)(tboolean)];
	deftab.A[(int)(deoln)]->U.V13.tfuntyp = typnods.A[(int)(tboolean)];
	deftab.A[(int)(dodd)]->U.V13.tfuntyp = typnods.A[(int)(tboolean)];
	deftab.A[(int)(dord)]->U.V13.tfuntyp = typnods.A[(int)(tinteger)];
	deftab.A[(int)(dround)]->U.V13.tfuntyp = typnods.A[(int)(tinteger)];
	deftab.A[(int)(dtrunc)]->U.V13.tfuntyp = typnods.A[(int)(tinteger)];
	deftab.A[(int)(darctan)]->U.V13.tfuntyp = typnods.A[(int)(treal)];
	deftab.A[(int)(dcos)]->U.V13.tfuntyp = typnods.A[(int)(treal)];
	deftab.A[(int)(dsin)]->U.V13.tfuntyp = typnods.A[(int)(treal)];
	deftab.A[(int)(dtan)]->U.V13.tfuntyp = typnods.A[(int)(treal)];
	deftab.A[(int)(dsqrt)]->U.V13.tfuntyp = typnods.A[(int)(treal)];
	deftab.A[(int)(dexp)]->U.V13.tfuntyp = typnods.A[(int)(treal)];
	deftab.A[(int)(dln)]->U.V13.tfuntyp = typnods.A[(int)(treal)];
	deftab.A[(int)(dsqr)]->U.V13.tfuntyp = typnods.A[(int)(tpoly)];
	deftab.A[(int)(dabs)]->U.V13.tfuntyp = typnods.A[(int)(tpoly)];
	deftab.A[(int)(dpred)]->U.V13.tfuntyp = typnods.A[(int)(tpoly)];
	deftab.A[(int)(dsucc)]->U.V13.tfuntyp = typnods.A[(int)(tpoly)];
	deftab.A[(int)(dargv)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(ddispose)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dexit)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dget)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dhalt)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dnew)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dpack)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dput)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dread)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dreadln)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dreset)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(drewrite)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dwrite)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dwriteln)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dmessage)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	deftab.A[(int)(dunpack)]->U.V13.tfuntyp = typnods.A[(int)(tnone)];
	nmachdefs = 0;
	defmach(0L, 255L, *((machdefstr *)"unsigned char   "));
	defmach(-128L, 127L, *((machdefstr *)"char            "));
	defmach(0L, 65535L, *((machdefstr *)"unsigned short  "));
	defmach(-32768L, 32767L, *((machdefstr *)"short           "));
	defmach(-2147483647L, 2147483647L, *((machdefstr *)"long            "));
}

extern void exit();

 void
error(m)
	errors	m;
{
	prtmsg(m);
	exit(1);
	longjmp(J[0].jb, 9999);
}

 void
fatal(m)
	errors	m;
{
	prtmsg(m);
	abort();
}

/*
**	Start of program code
*/
main()
{
	if (setjmp(J[0].jb))
	goto L9999;
	initialize();
	if (echo)
		(void)fprintf(output.fp, "# ifdef PASCAL\n"), Putl(output, 1);
	parse();
	if (echo)
		(void)fprintf(output.fp, "# else\n"), Putl(output, 1);
	lineno = 0;
	lastline = 0;
	transform();
	emit();
	if (echo)
		(void)fprintf(output.fp, "# endif\n"), Putl(output, 1);
L9999:
	;
	exit(0);
}
/*
**	End of program code
*/
setword	Q0[] = {
	1,
	0x03FD
};
setword	Q1[] = {
	1,
	0x004C
};
setword	Q2[] = {
	1,
	0x0000
};
setword	Q3[] = {
	2,
	0x000E,	0x5210
};
setword	Q4[] = {
	2,
	0x000E,	0x1210
};
setword	Q5[] = {
	1,
	0x0C00
};
setword	Q6[] = {
	1,
	0x000C
};
setword	Q7[] = {
	2,
	0x000E,	0x0210
};
setword	Q8[] = {
	3,
	0x0000,	0x0000,	0x0060
};
setword	Q9[] = {
	4,
	0x0002,	0x0000,	0x0064,	0x0800
};
setword	Q10[] = {
	1,
	0x0C00
};
setword	Q11[] = {
	1,
	0x000C
};
setword	Q12[] = {
	4,
	0x0000,	0x0000,	0x4FF0,	0x0008
};
setword	Q13[] = {
	3,
	0x0000,	0x0000,	0x0780
};
setword	Q14[] = {
	3,
	0x0000,	0x0000,	0x0780
};
setword	Q15[] = {
	5,
	0x0000,	0x0001,	0x0000,	0x0000,	0x0001
};
setword	Q16[] = {
	3,
	0x0000,	0x0000,	0x07E0
};
setword	Q17[] = {
	3,
	0x0000,	0x0000,	0xB000
};
setword	Q18[] = {
	4,
	0x0000,	0x0008,	0x0000,	0x0040
};
setword	Q19[] = {
	3,
	0x0000,	0x0000,	0xB560
};
setword	Q20[] = {
	4,
	0x0000,	0x0000,	0x4FF0,	0x0008
};
setword	Q21[] = {
	1,
	0x0C00
};
setword	Q22[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1700
};
setword	Q23[] = {
	2,
	0x0000,	0x0003
};
setword	Q24[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1700
};
setword	Q25[] = {
	3,
	0x0000,	0x0040,	0x4000
};
setword	Q26[] = {
	1,
	0x000E
};
setword	Q27[] = {
	1,
	0x000E
};
setword	Q28[] = {
	1,
	0x000C
};
setword	Q29[] = {
	1,
	0x000E
};
setword	Q30[] = {
	1,
	0x000E
};
setword	Q31[] = {
	2,
	0x8000,	0x0045
};
setword	Q32[] = {
	1,
	0x000E
};
setword	Q33[] = {
	1,
	0x000E
};
setword	Q34[] = {
	3,
	0x0000,	0x0000,	0x0040
};
setword	Q35[] = {
	3,
	0x4010,	0x0C00,	0x000A
};
setword	Q36[] = {
	5,
	0x0300,	0x0000,	0x0004,	0x2000,	0x0001
};
setword	Q37[] = {
	4,
	0x0300,	0x0000,	0x0004,	0x2000
};
setword	Q38[] = {
	3,
	0x0000,	0x0000,	0x0080
};
setword	Q39[] = {
	4,
	0x0040,	0x0000,	0x0000,	0x0800
};
setword	Q40[] = {
	1,
	0x0040
};
setword	Q41[] = {
	3,
	0x0080,	0x0000,	0x0001
};
setword	Q42[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x4000
};
setword	Q43[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q44[] = {
	4,
	0x0000,	0x0000,	0x0004,	0x2000
};
setword	Q45[] = {
	1,
	0x0040
};
setword	Q46[] = {
	2,
	0x0000,	0x8000
};
setword	Q47[] = {
	4,
	0x0300,	0x0000,	0x0004,	0x2000
};
setword	Q48[] = {
	4,
	0x930C,	0x2001,	0x0834,	0x2000
};
setword	Q49[] = {
	4,
	0x930C,	0x2001,	0x0834,	0x3000
};
setword	Q50[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1800
};
setword	Q51[] = {
	2,
	0x0200,	0x0100
};
setword	Q52[] = {
	3,
	0x0200,	0x0100,	0x3C80
};
setword	Q53[] = {
	2,
	0x0000,	0x0040
};
setword	Q54[] = {
	4,
	0x0000,	0x0000,	0x4FF0,	0x0008
};
setword	Q55[] = {
	4,
	0x03E1,	0x80CA,	0xF005,	0xBFFE
};
setword	Q56[] = {
	4,
	0x0000,	0x0030,	0x3F80,	0x001A
};
setword	Q57[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x0478
};
setword	Q58[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x0478
};
setword	Q59[] = {
	1,
	0x0C0C
};
setword	Q60[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q61[] = {
	5,
	0x0000,	0x0000,	0x0000,	0x000B,	0x0001
};
setword	Q62[] = {
	5,
	0x03E1,	0x80CA,	0xF005,	0xFFFF,	0x0001
};
setword	Q63[] = {
	4,
	0x930C,	0x2001,	0x0834,	0x2000
};
setword	Q64[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1000
};
setword	Q65[] = {
	4,
	0x930C,	0x2001,	0x08B4,	0x2000
};
setword	Q66[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2004
};
setword	Q67[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q68[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1000
};
setword	Q69[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1002
};
setword	Q70[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2004
};
setword	Q71[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2006
};
setword	Q72[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2004
};
setword	Q73[] = {
	3,
	0x0002,	0x0000,	0x0800
};
setword	Q74[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q75[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1000
};
setword	Q76[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q77[] = {
	3,
	0x4000,	0x0400,	0x0808
};
setword	Q78[] = {
	3,
	0x0002,	0x0000,	0x0800
};
setword	Q79[] = {
	2,
	0x0000,	0x0040
};
setword	Q80[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2010
};
setword	Q81[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q82[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2010
};
setword	Q83[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q84[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x0008
};
setword	Q85[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1000
};
setword	Q86[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q87[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x8000
};
setword	Q88[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q89[] = {
	3,
	0x4004,	0x0400,	0x0040
};
setword	Q90[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2000
};
setword	Q91[] = {
	1,
	0x2400
};
setword	Q92[] = {
	3,
	0x4014,	0x0404,	0x000A
};
setword	Q93[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2000
};
setword	Q94[] = {
	3,
	0x6414,	0x0404,	0x000A
};
setword	Q95[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2000
};
setword	Q96[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q97[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q98[] = {
	3,
	0x4004,	0x0400,	0x0840
};
setword	Q99[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1800
};
setword	Q100[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q101[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1800
};
setword	Q102[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q103[] = {
	3,
	0x4004,	0x0400,	0x0848
};
setword	Q104[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x0020
};
setword	Q105[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q106[] = {
	2,
	0x0000,	0x0040
};
setword	Q107[] = {
	2,
	0x0000,	0x0040
};
setword	Q108[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x0010
};
setword	Q109[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x0008
};
setword	Q110[] = {
	4,
	0x0200,	0x0000,	0x0000,	0x2004
};
setword	Q111[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q112[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x0004
};
setword	Q113[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q114[] = {
	3,
	0x0000,	0x0000,	0x3C80
};
setword	Q115[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x8000
};
setword	Q116[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x8000
};
setword	Q117[] = {
	4,
	0x0200,	0x0000,	0x0000,	0x2814
};
setword	Q118[] = {
	2,
	0x0802,	0x5000
};
setword	Q119[] = {
	4,
	0x0802,	0x5200,	0x3C80,	0x0003
};
setword	Q120[] = {
	4,
	0x0200,	0x0000,	0x0000,	0x2004
};
setword	Q121[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x0002
};
setword	Q122[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1800
};
setword	Q123[] = {
	3,
	0x0000,	0x0000,	0x3C80
};
setword	Q124[] = {
	2,
	0x0000,	0x0040
};
setword	Q125[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q126[] = {
	4,
	0x0000,	0x0040,	0x0000,	0x1000
};
setword	Q127[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q128[] = {
	3,
	0x0008,	0x0000,	0x0800
};
setword	Q129[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1000
};
setword	Q130[] = {
	3,
	0x0008,	0x0000,	0x0800
};
setword	Q131[] = {
	3,
	0x0000,	0x0000,	0x0880
};
setword	Q132[] = {
	3,
	0x0000,	0x0000,	0x0980
};
setword	Q133[] = {
	3,
	0x0000,	0x0000,	0x3000
};
setword	Q134[] = {
	3,
	0x4004,	0x0400,	0x084A
};
setword	Q135[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2000
};
setword	Q136[] = {
	3,
	0x0000,	0x0000,	0x3F80
};
setword	Q137[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x0020
};
setword	Q138[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q139[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1824
};
setword	Q140[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q141[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x1824
};
setword	Q142[] = {
	3,
	0x4014,	0x0400,	0x000A
};
setword	Q143[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2800
};
setword	Q144[] = {
	3,
	0x0000,	0x0000,	0x0080
};
setword	Q145[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2000
};
setword	Q146[] = {
	5,
	0x0000,	0x0000,	0x0000,	0x0000,	0x0001
};
setword	Q147[] = {
	3,
	0x4014,	0x0404,	0x000A
};
setword	Q148[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2000
};
setword	Q149[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q150[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x2002
};
setword	Q151[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q152[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x0804
};
setword	Q153[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q154[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x0804
};
setword	Q155[] = {
	3,
	0x0000,	0x0000,	0x0800
};
setword	Q156[] = {
	4,
	0x0000,	0x0000,	0x0000,	0x0804
};
setword	Q157[] = {
	3,
	0x0004,	0x0000,	0x0040
};
setword	Q158[] = {
	1,
	0x0018
};
setword	Q159[] = {
	1,
	0x00C0
};
setword	Q160[] = {
	1,
	0x001A
};
setword	Q161[] = {
	1,
	0x001E
};
setword	Q162[] = {
	1,
	0x001F
};
setword	Q163[] = {
	1,
	0x1FE7
};
setword	*Conset[] = {
	Q163,	Q162,	Q161,
	Q160,	Q159,	Q158,	Q157,	Q156,	Q155,
	Q154,	Q153,	Q152,	Q151,	Q150,	Q149,
	Q148,	Q147,	Q146,	Q145,	Q144,	Q143,
	Q142,	Q141,	Q140,	Q139,	Q138,	Q137,
	Q136,	Q135,	Q134,	Q133,	Q132,	Q131,
	Q130,	Q129,	Q128,	Q127,	Q126,	Q125,
	Q124,	Q123,	Q122,	Q121,	Q120,	Q119,
	Q118,	Q117,	Q116,	Q115,	Q114,	Q113,
	Q112,	Q111,	Q110,	Q109,	Q108,	Q107,
	Q106,	Q105,	Q104,	Q103,	Q102,	Q101,
	Q100,	Q99,	Q98,	Q97,	Q96,	Q95,
	Q94,	Q93,	Q92,	Q91,	Q90,	Q89,
	Q88,	Q87,	Q86,	Q85,	Q84,	Q83,
	Q82,	Q81,	Q80,	Q79,	Q78,	Q77,
	Q76,	Q75,	Q74,	Q73,	Q72,	Q71,
	Q70,	Q69,	Q68,	Q67,	Q66,	Q65,
	Q64,	Q63,	Q62,	Q61,	Q60,	Q59,
	Q58,	Q57,	Q56,	Q55,	Q54,	Q53,
	Q52,	Q51,	Q50,	Q49,	Q48,	Q47,
	Q46,	Q45,	Q44,	Q43,	Q42,	Q41,
	Q40,	Q39,	Q38,	Q37,	Q36,	Q35,
	Q34,	Q33,	Q32,	Q31,	Q30,	Q29,
	Q28,	Q27,	Q26,	Q25,	Q24,	Q23,
	Q22,	Q21,	Q20,	Q19,	Q18,	Q17,
	Q16,	Q15,	Q14,	Q13,	Q12,	Q11,
	Q10,	Q9,	Q8,	Q7,	Q6,	Q5,
	Q4,	Q3,	Q2,	Q1,	Q0
};

setptr
Union(p1, p2)
	register setptr	p1, p2;
{
	register int		i, j, k;
	register setptr		sp = Newset(),
				p3 = sp;

	j = *p1;
	*p3 = j;
	if (j > *p2)
		j = *p2;
	else
		*p3 = *p2;
	k = *p1 - *p2;
	p1++, p2++, p3++;
	for (i = 0; i < j; i++)
		*p3++ = (*p1++ | *p2++);
	while (k > 0) {
		*p3++ = *p1++;
		k--;
	}
	while (k < 0) {
		*p3++ = *p2++;
		k++;
	}
	return (Saveset(sp));
}

boolean
Member(m, sp)
	register unsigned int	m;
	register setptr	sp;
{
	register unsigned int	i = m / (setbits+1) + 1;

	if ((i <= *sp) && (sp[i] & (1 << (m % (setbits+1)))))
		return (true);
	return (false);
}

setptr
Insmem(m, sp)
	register unsigned int	m;
	register setptr	sp;
{
	register int	i,
				j = m / (setbits+1) + 1;

	if (*sp < j)
		for (i = *sp + 1, *sp = j; i <= *sp; i++)
			sp[i] = 0;
	sp[j] |= (1 << (m % (setbits+1)));
	return (sp);
}

# ifndef SETSPACE
# define SETSPACE 256
# endif
setptr
Currset(n,sp)
	int	n;
	setptr	sp;
{
	static setword	Space[SETSPACE];
	static setptr	Top = Space;

	switch (n) {
	  case 0:
		Top = Space;
		return (0);
	  case 1:
		if (&Space[SETSPACE] - Top <= 15) {
			(void)fprintf(stderr, "Set-space exhausted\n");
			exit(1);
		}
		*Top = 0;
		return (Top);
	  case 2:
		if (Top <= &sp[*sp])
			Top = &sp[*sp + 1];
		return (sp);
	}
	/* NOTREACHED */
}

void
Setncpy(S1, S2, N)
	register setptr	S1, S2;
	register unsigned int	N;
{
	register unsigned int	m;

	N /= sizeof(setword);
	*S1++ = --N;
	m = *S2++;
	while (m != 0 && N != 0) {
		*S1++ = *S2++;
		--N;
		--m;
	}
	while (N-- != 0)
		*S1++ = 0;
}

void
Caseerror(n)
	int	n;
{
	(void)fprintf(stderr, "Missing case limb: line %d\n", n);
	exit(1);
}
