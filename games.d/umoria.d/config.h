/* Person to bother if something goes wrong */
#define WIZARD	"Jim Wilson"
/* Wizard password */
#define PASSWD1 "Gandalf"
/* God password, for debugging */
#define PASSWD2 "Mithrandir"
/* Wizard/God user id */
#define UID 0

#ifndef LIBDIR
#define LIBDIR		"/usr/games/lib/moriadir
#endif
#ifdef __STDC__
#define STR(x)	#x
#define STRING(x)	STR(x)
#define FILENM(x)	STRING(LIBDIR) "/" STRING(x)
#else
#define STRING(x)	"x
#define FILENM(x)	STRING(LIBDIR)/x"
#endif
/* related files */
#define MORIA_HOU  FILENM(Moria_hours)
#define MORIA_MOR  FILENM(Moria_news)
#define MORIA_MAS  FILENM(Character)
#define MORIA_TOP  FILENM(Highscores)
#define MORIA_HLP  FILENM(Helpfile)
