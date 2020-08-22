




























  














































	




























extern int kmemstart;	





































































typedef	struct { int r[1]; }	*physadr;
typedef	long		daddr_t;
typedef	char		*caddr_t;
typedef	unsigned short	ushort;
typedef	unsigned short	ino_t;
typedef	char		cnt_t;
typedef	long		time_t;
typedef	int		label_t[6];	
typedef	short		dev_t;
typedef	long		off_t;
typedef	long		paddr_t;
typedef	unsigned short	mloc_t;		
typedef	unsigned short	msize_t;	


typedef	unsigned short	OFF_t;		
typedef	unsigned short	SEG_t;		

	











typedef unsigned char uchar_t;
typedef unsigned long ulong_t;





struct saddr {			
	unsigned short	sa_seg;
	long		sa_off;
};










						












		               


                   






struct inode *rootdir;		
struct proc *runq;		
struct proc *Hogproc;           
short   cputype;                
time_t	lbolt;			
time_t	time;			
int     Hz;                     
int     Timezone;               
int     Dstflag;                

int	mpid;			
char	runin;			
char	runout;			
char	runrun;			
char    runtxt;                 
char	curpri;			
unsigned maxmem;		
int	physmem;		
daddr_t	swplo;			
int	nswap;			
int	updlock;		
daddr_t	rablock;		
int     Cmask;                  
extern	char	regloc[];	
extern short	reglocc;	
dev_t	rootdev;		
dev_t	swapdev;		
dev_t	pipedev;		
extern	short   icode[];	
ushort  szicode;                
int	blkacty;		





dev_t getmdev();
daddr_t	bmap();
faddr_t makefar();
struct inode *ialloc();
struct inode *iget();
struct inode *owner();
struct inode *maknode();
struct inode *namei();
struct buf *alloc();
struct buf *getblk();
struct buf *getablk();
struct buf *bread();
struct buf *breada();
struct filsys *getfs();
struct file *getf();
struct file *falloc();
int	uchar();

extern	char	maxmask;	

























































































































































































































struct termio {
	unsigned short	c_iflag;	
	unsigned short	c_oflag;	
	unsigned short	c_cflag;	
	unsigned short	c_lflag;	
	char		c_line;		
	uchar_t		c_cc[8];	
};



struct clist {
	int	c_cc;		
	struct cblock	*c_cf;	
	struct cblock	*c_cl;	
};


struct tty {
	struct clist	t_rawq;	
	struct clist	t_canq;	
	struct clist	t_outq;	
	struct cblock	*t_buf;	
	int	(*t_proc)();	
	ushort	t_iflag;	
	ushort	t_oflag;	
	ushort	t_cflag;	
	ushort	t_lflag;	
	ushort	t_xflag;	
	short	t_state;	
	short	t_pgrp;		
	char	t_line;		
	char	t_delct;	
	char	t_col;		
	char	t_row;		
	uchar_t	t_cc[8+2];	
	short 	t_addr;		
	struct tty   *t_chan;	
};



struct cblock {
	struct cblock	*c_next;
	char	c_first;
	char	c_last;
	char	c_data[24];
};

extern	struct cblock	cfree[];
extern	struct cblock	*getcb();
extern	struct cblock	*getcf();
extern	struct clist	ttnulq;

struct chead {
	struct cblock	*c_next;
	int	c_size;
};
extern	struct chead	cfreelist;

struct inter {
	int	cnt;
};

	

	





extern	int	ttlowat[], tthiwat[];




		

		

		

	

		



















































struct	direct
{
	ino_t	d_ino;
	char	d_name[14		];
};










typedef	char far	*faddr_t;









struct xexec {			    
	unsigned short	x_magic;	
	unsigned short	x_ext;		
	long		x_text;		
	long		x_data;		
	long		x_bss;		
	long		x_syms;		
	long		x_reloc;	
	long		x_entry;	
	char		x_cpu;		
	char		x_relsym;	
	unsigned short	x_renv;		
};


struct xext {			    
	long		xe_trsize;	
	long		xe_drsize;	
	long		xe_tbase;	
	long		xe_dbase;	
	long		xe_stksize;	
				
	long		xe_segpos;	
	long		xe_segsize;	
	long		xe_mdtpos;	
	long		xe_mdtsize;	
	char		xe_mdttype;	
	char		xe_pagesize;	
	char		xe_ostype;	
	char		xe_osvers;	
	unsigned short	xe_eseg;	
	unsigned short	xe_sres;	
};


struct xseg {			    
	unsigned short	xs_type;	
	unsigned short	xs_attr;	
	unsigned short	xs_seg;		
	unsigned short	xs_sres;	
	long		xs_filpos;	
	long		xs_psize;	
	long		xs_vsize;	
	long		xs_rbase;	
	long		xs_lres;	
	long		xs_lres2;	
};


struct xiter {			    
	long		xi_size;	
	long		xi_rep;		
	long		xi_offset;	
};


struct xlist { 			    
	unsigned short	xl_type;	
	unsigned short	xl_seg;		
	long		xl_value;	
	char		*xl_name;	
};










	




















	










	










































































    






    


















































struct aexec {			    
	unsigned short	xa_magic;       
	unsigned short	xa_text;        
	unsigned short	xa_data;        
	unsigned short	xa_bss;         
	unsigned short	xa_syms;        
	unsigned short	xa_entry;       
	unsigned short	xa_unused;      
	unsigned short	xa_flag;        
};


struct nlist {			    
	char		n_name[8];	
	int		n_type;		
	unsigned	n_value;	
};































struct bexec {		    
	long	xb_magic;	
	long	xb_text;	
	long	xb_data;	
	long	xb_bss;		
	long	xb_syms;	
	long	xb_trsize;	
	long	xb_drsize;	
	long	xb_entry;	
};










 
struct	user
{                               
	char	u_kstack[1024		];   
	label_t u_rsav;             
	label_t u_qsav;             
	label_t u_ssav;             
	char    u_segflg;           
	char    u_error;            
	ushort  u_uid;              
	ushort  u_gid;              
	ushort  u_ruid;             
	ushort  u_rgid;             
	struct proc *u_procp;       
	int     *u_ap;              
	union {                     
		struct	{
			int     r_val1;  
			int     r_val2; 
		}r_reg;
		off_t	r_off;
		time_t	r_time;
		long	r_long;
		faddr_t	r_faddr;
	} u_r;
	faddr_t u_base;             
	unsigned u_count;           
	off_t   u_offset;           
	short   u_fmode;            
	short   u_errcnt;           
	struct inode *u_cdir;       
	struct inode *u_rdir;       
	faddr_t u_dirp;             
	struct direct u_dent;       
	struct inode *u_pdir;       
	struct file *u_ofile[20		];
	char    u_pofile[20		];   
	int     u_arg[16];          
	unsigned u_tsize;           
	unsigned u_dsize;           
	unsigned u_ssize;           
	faddr_t u_signal[20];     
	time_t  u_utime;            
	time_t  u_stime;            
	time_t  u_cutime;           
	time_t  u_cstime;           
	int     *u_ar0;             
	struct {                    
		faddr_t  pr_base;   
		unsigned pr_size;   
		long     pr_off;    
		unsigned pr_scale;  
		faddr_t  pr_syspc;  
	} u_prof;
	char    u_intflg;           
	char    u_sep;              
	unsigned short	u_renv;	     
	struct tty *u_ttyp;         
	dev_t   u_ttyd;             
	struct xexec u_exdata;      
	struct xext     u_exext;    
	char    u_comm[14		];     
	time_t  u_start;            
	time_t  u_ticks;           
	long    u_mem;            
	long    u_ior;           
	long    u_iow;          
	long    u_iosw;        
	long    u_ioch;       
	char    u_acflag;    
	char	u_nfsflgs;	    
	short   u_cmask;            
	daddr_t u_limit;            
	int     u_t[32];            
	int     u_sdsaved;          

	int     u_fper;             
	char    u_fpsaved;          
	char    u_fpused;           
	struct {                    
		int	fp_cntrl;	
		int	fp_stat;	
		int	fp_tag;		
		int	fp_ip[2];	
		int	fp_op[2];	
		char    fp_regs[100];   
		int     fp_spare;       
	} u_fps;
	unsigned short  u_lxrw;      
	faddr_t	 u_stkbot;	    
	faddr_t	 u_edata;	    
	long 	 u_segoff[154		];  

	char	 u_ovlstk[6];		
	int	 u_stktop;
	struct inode *u_ip;		
};
extern struct user u;

























































































extern struct bdevsw
{
	int	(*d_open)();
	int	(*d_close)();
	int	(*d_strategy)();
	struct iobuf *d_tab;
} bdevsw[];


extern struct cdevsw
{
	int	(*d_open)();
	int	(*d_close)();
	int	(*d_read)();
	int	(*d_write)();
	int	(*d_ioctl)();
} cdevsw[];

int	bdevcnt;
int	cdevcnt;


extern struct linesw
{
	int	(*l_open)();
	int	(*l_close)();
	int	(*l_read)();
	int	(*l_write)();
	int	(*l_ioctl)();
	int	(*l_input)();
	int	(*l_output)();
	int	(*l_mdmint)();
} linesw[];

int	linecnt;










struct	file
{
	char	f_flag;
	char	f_res;		
	ushort	f_count;	
	struct inode *f_inode;	
	union {
		off_t	f_off;		
		struct file *f_slnk;    
	} f_un;
};



extern struct file file[];	






















struct	proc {
	char	p_stat;
	char	p_flag;
	char	p_pri;		
	char	p_time;		
	char	p_cpu;		
	char	p_nice;		
	long	p_sig;		
	ushort	p_uid;		
	short	p_pgrp;		
	short	p_pid;		
	short	p_ppid;		
	union {
		unsigned short p_daddr;	
		mloc_t	p_caddr;	
	} p_addr;
	unsigned short p_fcsel;  
	unsigned short p_fdsel;  
	unsigned short p_ldsel;  
	caddr_t p_wchan;	
	struct text *p_textp;	
	struct proc *p_link;	
	int	p_clktim;	
	struct sd *p_sdp;	
};

extern struct proc proc[];	

























struct	xproc {
	char	xp_stat;
	char	xp_flag;
	char	xp_pri;		
	char	xp_time;	
	char	xp_cpu;		
	char	xp_nice;	
	long	xp_sig;		
	ushort	xp_uid;		
	short	xp_pgrp;	
	short	xp_pid;		
	short	xp_ppid;	
	short	xp_xstat;	
	time_t	xp_utime;	
	time_t	xp_stime;	
};





extern struct	tty pt_tty[];
extern struct	pt_ioctl {
	int	pt_flags;
	int	pt_gensym;
	struct	proc *pt_selr, *pt_selw;
	int	pt_send;
} pt_ioctl[];










ptsopen(dev, flag)
	dev_t dev;
{
	register struct tty *tp;

	if (((dev)&0377) >= npty)
		return (6);
	tp = &pt_tty[((dev)&0377)];
	if ((tp->t_state & TS_ISOPEN) == 0) {
		ttychars(tp);		
		tp->t_ispeed = tp->t_ospeed = 0000017;
		tp->t_flags = 0;	
	} else if (tp->t_state&TS_XCLUDE && u.u_uid != 0)
		return (16);
	if (tp->t_oproc)			
		tp->t_state |= TS_CARR_ON;
	while ((tp->t_state & TS_CARR_ON) == 0) {
		tp->t_state |= TS_WOPEN;
		sleep((caddr_t)&tp->t_rawq, 28);
	}
	return ((*linesw[tp->t_line].l_open)(dev, tp));
}

ptsclose(dev)
	dev_t dev;
{
	register struct tty *tp;

	tp = &pt_tty[((dev)&0377)];
	(*linesw[tp->t_line].l_close)(tp);
	ttyclose(tp);
}

ptsread(dev, uio)
	dev_t dev;
	struct uio *uio;
{
	register struct tty *tp = &pt_tty[((dev)&0377)];
	register struct pt_ioctl *pti = &pt_ioctl[((dev)&0377)];
	int error = 0;

again:
	if (pti->pt_flags & 0x20		) {
		while (tp == u.u_ttyp && u.u_procp->p_pgrp != tp->t_pgrp) {

			if ((u.u_procp->p_sigignore & (1<<(SIGTTIN-1))) ||
			    (u.u_procp->p_sigmask & (1<<(SIGTTIN-1))) ||
	
			    u.u_procp->p_flag&SVFORK)
				return (5);
			gsignal(u.u_procp->p_pgrp, SIGTTIN);
			sleep((caddr_t)&lbolt, 28);
		}

		if (tp->t_rawq.c_cc == 0) {
			if (tp->t_state & TS_NBIO)
				return (EWOULDBLOCK);
			sleep((caddr_t)&tp->t_rawq, 28);
			goto again;
		}
		while (tp->t_rawq.c_cc > 1 && uio->uio_resid > 0)
			if (ureadc(getc(&tp->t_rawq), uio) < 0) {
				error = 14;
				break;
			}
		if (tp->t_rawq.c_cc == 1)
			(void) getc(&tp->t_rawq);
		if (tp->t_rawq.c_cc)
			return (error);
	} else
		if (tp->t_oproc)
			error = (*linesw[tp->t_line].l_read)(tp, uio);
	wakeup((caddr_t)&tp->t_rawq.c_cf);
	if (pti->pt_selw) {
		selwakeup(pti->pt_selw, pti->pt_flags & 0x02);
		pti->pt_selw = 0;
		pti->pt_flags &= ~0x02;
	}
	return (error);
}


ptswrite(dev, uio)
	dev_t dev;
	struct uio *uio;
{
	register struct tty *tp;

	tp = &pt_tty[((dev)&0377)];
	if (tp->t_oproc == 0)
		return (5);
	return ((*linesw[tp->t_line].l_write)(tp, uio));
}


ptsstart(tp)
	struct tty *tp;
{
	register struct pt_ioctl *pti = &pt_ioctl[((tp->t_dev)&0377)];

	if (tp->t_state & TS_TTSTOP)
		return;
	if (pti->pt_flags & 0x10		) {
		pti->pt_flags &= ~0x10		;
		pti->pt_send = TIOCPKT_START;
	}
	ptcwakeup(tp);
}

ptcwakeup(tp)
	struct tty *tp;
{
	struct pt_ioctl *pti = &pt_ioctl[((tp->t_dev)&0377)];

	if (pti->pt_selr) {
		selwakeup(pti->pt_selr, pti->pt_flags & 0x01);
		pti->pt_selr = 0;
		pti->pt_flags &= ~0x01;
	}
	wakeup((caddr_t)&tp->t_outq.c_cf);
}


ptcopen(dev, flag)
	dev_t dev;
	int flag;
{
	register struct tty *tp;
	struct pt_ioctl *pti;

	if (((dev)&0377) >= npty)
		return (6);
	tp = &pt_tty[((dev)&0377)];
	if (tp->t_oproc)
		return (5);
	tp->t_oproc = ptsstart;
	if (tp->t_state & TS_WOPEN)
		wakeup((caddr_t)&tp->t_rawq);
	tp->t_state |= TS_CARR_ON;
	pti = &pt_ioctl[((dev)&0377)];
	pti->pt_flags = 0;
	pti->pt_send = 0;
	return (0);
}

ptcclose(dev)
	dev_t dev;
{
	register struct tty *tp;

	tp = &pt_tty[((dev)&0377)];
	if (tp->t_state & TS_ISOPEN)
		gsignal(tp->t_pgrp, 1	);
	tp->t_state &= ~TS_CARR_ON;	
	ttyflush(tp, 00001|00002);
	tp->t_oproc = 0;		
}

ptcread(dev, uio)
	dev_t dev;
	struct uio *uio;
{
	register struct tty *tp = &pt_tty[((dev)&0377)];
	struct pt_ioctl *pti;
	int error = 0;

	if ((tp->t_state&(TS_CARR_ON|TS_ISOPEN)) == 0)
		return (5);
	pti = &pt_ioctl[((dev)&0377)];
	if (pti->pt_flags & 0x08		) {
		if (pti->pt_send) {
			error = ureadc(pti->pt_send, uio);
			if (error)
				return (error);
			pti->pt_send = 0;
			return (0);
		}
		error = ureadc(0, uio);
	}
	while (tp->t_outq.c_cc == 0 || (tp->t_state&TS_TTSTOP)) {
		if (pti->pt_flags&0x04)
			return (EWOULDBLOCK);
		sleep((caddr_t)&tp->t_outq.c_cf, 28);
	}
	while (tp->t_outq.c_cc && uio->uio_resid > 0)
		if (ureadc(getc(&tp->t_outq), uio) < 0) {
			error = 14;
			break;
		}
	if (tp->t_outq.c_cc <= TTLOWAT(tp)) {
		if (tp->t_state&TS_ASLEEP) {
			tp->t_state &= ~TS_ASLEEP;
			wakeup((caddr_t)&tp->t_outq);
		}
		if (tp->t_wsel) {
			selwakeup(tp->t_wsel, tp->t_state & TS_WCOLL);
			tp->t_wsel = 0;
			tp->t_state &= ~TS_WCOLL;
		}
	}
	return (error);
}

ptsstop(tp, flush)
	register struct tty *tp;
	int flush;
{
	struct pt_ioctl *pti = &pt_ioctl[((tp->t_dev)&0377)];

	
	if (flush == 0) {
		flush = TIOCPKT_STOP;
		pti->pt_flags |= 0x10		;
	} else {
		pti->pt_flags &= ~0x10		;
	}
	pti->pt_send |= flush;
	ptcwakeup(tp);
}

ptcselect(dev, rw)
	dev_t dev;
	int rw;
{
	register struct tty *tp = &pt_tty[((dev)&0377)];
	struct pt_ioctl *pti = &pt_ioctl[((dev)&0377)];
	struct proc *p;
	int s;

	if ((tp->t_state&(TS_CARR_ON|TS_ISOPEN)) == 0)
		return (1);
	s = spl5();
	switch (rw) {

	case 00001:
		if (tp->t_outq.c_cc && (tp->t_state&TS_TTSTOP) == 0) {
			splx(s);
			return (1);
		}
		if ((p = pti->pt_selr) && p->p_wchan == (caddr_t)&selwait)
			pti->pt_flags |= 0x01;
		else
			pti->pt_selr = u.u_procp;
		break;

	case 00002:
		if ((pti->pt_flags & 0x20		) == 0 || tp->t_rawq.c_cc == 0) {
			splx(s);
			return (1);
		}
		if ((p = pti->pt_selw) && p->p_wchan == (caddr_t)&selwait)
			pti->pt_flags |= 0x02;
		else
			pti->pt_selw = u.u_procp;
		break;
	}
	splx(s);
	return (0);
}

ptcwrite(dev, uio)
	dev_t dev;
	struct uio *uio;
{
	register struct tty *tp = &pt_tty[((dev)&0377)];
	register char *cp, *ce;
	register int cc;
	char locbuf[100		];
	int cnt = 0;
	struct pt_ioctl *pti = &pt_ioctl[((dev)&0377)];
	int error = 0;

	if ((tp->t_state&(TS_CARR_ON|TS_ISOPEN)) == 0)
		return (5);
	do {
		register struct iovec *iov;

		if (uio->uio_iovcnt == 0)
			break;
		iov = uio->uio_iov;
		if (iov->iov_len == 0) {
			uio->uio_iovcnt--;	
			uio->uio_iov++;
			if (uio->uio_iovcnt < 0)
				panic("ptcwrite");
			continue;
		}
		cc = MIN(iov->iov_len, 100		);
		cp = locbuf;
		error = uiomove(cp, cc, UIO_WRITE, uio);
		if (error)
			break;
		ce = cp + cc;
again:
		if (pti->pt_flags & 0x20		) {
			if (tp->t_rawq.c_cc) {
				if (pti->pt_flags & 0x04) {
					iov->iov_base -= ce - cp;
					iov->iov_len += ce - cp;
					uio->uio_resid += ce - cp;
					uio->uio_offset -= ce - cp;
					return (EWOULDBLOCK);
				}
				sleep((caddr_t)&tp->t_rawq.c_cf, 29);
				goto again;
			}
			(void) b_to_q(cp, cc, &tp->t_rawq);
			(void) putc(0, &tp->t_rawq);
			wakeup((caddr_t)&tp->t_rawq);
			return (0);
		}
		while (cp < ce) {
			while ((tp->t_delct || tp->t_canq.c_cc)
			&& (tp->t_rawq.c_cc + tp->t_canq.c_cc) >= 256 - 2) {
				wakeup((caddr_t)&tp->t_rawq);
				if (tp->t_state & TS_NBIO) {
					iov->iov_base -= ce - cp;
					iov->iov_len += ce - cp;
					uio->uio_resid += ce - cp;
					uio->uio_offset -= ce - cp;
					if (cnt == 0)
						return (EWOULDBLOCK);
					return (0);
				}
				
				
				sleep((caddr_t)&tp->t_rawq.c_cf, 29);
				goto again;
			}
			(*linesw[tp->t_line].l_rint)(*cp++, tp);
			cnt++;
		}
	} while (uio->uio_resid);
	return (error);
}


ptyioctl(dev, cmd, data, flag)
	caddr_t data;
	dev_t dev;
{
	register struct tty *tp = &pt_tty[((dev)&0377)];
	register struct pt_ioctl *pti = &pt_ioctl[((dev)&0377)];
	int error;

	
	if (cdevsw[(((unsigned)(dev)>>8))].d_open == ptcopen)
		switch (cmd) {

		case TIOCPKT:
			if (*(int *)data)
				pti->pt_flags |= 0x08		;
			else
				pti->pt_flags &= ~0x08		;
			return (0);

		case TIOCREMOTE:
			if (*(int *)data)
				pti->pt_flags |= 0x20		;
			else
				pti->pt_flags &= ~0x20		;
			ttyflush(tp, 00001|00002);
			return (0);

		case FIONBIO:
			if (*(int *)data)
				pti->pt_flags |= 0x04;
			else
				pti->pt_flags &= ~0x04;
			return (0);

		case (('t'<<8)|9):
			while (getc(&tp->t_outq) >= 0)
				;
			break;
		}
	error = ttioctl(tp, cmd, data, dev);
	if (error < 0)
		error = 25;
	{ int stop = (tp->t_stopc == ('s'&037) &&
		      tp->t_startc == ('q'&037));
	if (pti->pt_flags & 0x40) {
		if (stop) {
			pti->pt_send &= TIOCPKT_NOSTOP;
			pti->pt_send |= TIOCPKT_DOSTOP;
			pti->pt_flags &= ~0x40;
			ptcwakeup(tp);
		}
	} else {
		if (stop == 0) {
			pti->pt_send &= ~TIOCPKT_DOSTOP;
			pti->pt_send |= TIOCPKT_NOSTOP;
			pti->pt_flags |= 0x40;
			ptcwakeup(tp);
		}
	}
	}
	return (error);
}
