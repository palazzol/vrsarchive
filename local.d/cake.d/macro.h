/*
**	Common macros for Cake
**
**	$Header: /home/Vince/cvs/local.d/cake.d/macro.h,v 1.1 1987-11-16 17:20:59 vrs Exp $
*/

#define	checkpatlen(p)	if (strlen(p) > MAXPATSIZE) \
			printf("Pattern %s too long\n", p), exit_cake(FALSE)
#define	cdebug		if (cakedebug) printf

#ifndef	CAKEDEBUG
#define	put_trail(func, event)
#define	get_trail(fp)
#endif

/*	Flag handling				*/

#define	on_node(n, f)		((n)->n_flag & (f))
#define	off_node(n, f)		(!((n)->n_flag & (f)))
#define	set_node(n, f)		(n)->n_flag |= (f)
#define	reset_node(n, f)	(n)->n_flag &= ~(f)

#define	on_pat(p, f)		((p)->p_flag & (f))
#define	off_pat(p, f)		(!((p)->p_flag & (f)))
#define	set_pat(p, f)		(p)->p_flag |= (f)
#define	reset_pat(p, f)		(p)->p_flag &= ~(f)

#define	on_act(a, f)		((a)->a_flag & (f))
#define	off_act(a, f)		(!((a)->a_flag & (f)))
#define	set_act(a, f)		(a)->a_flag |= (f)
#define	reset_act(a, f)		(a)->a_flag &= ~(f)

#define	is_ok(n)		((n)->n_kind == n_OK)
#define	is_cando(n)		((n)->n_kind == n_CANDO)
#define	is_noway(n)		((n)->n_kind == n_NOWAY)
