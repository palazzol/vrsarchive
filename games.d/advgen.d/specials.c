#include	"advgen.h"

static wordlist	def_go = { 5, { "go", "run", "walk", "move", "enter" } };
static wordlist	def_get = { 4, { "get", "take", "pick", "remove" } };
static wordlist	def_drop = { 3, { "drop", "release", "leave" } };

static wordlist	go_list;
static wordlist	get_list;
static wordlist	drop_list;

static wordlist	*cur_list;

static void
badzero(s)
    char	*s;
{
    gramerror(1, "warning: invalid zero value for %s ignored", s);
}

static int		tottrs = 0;
void
add_treasure()
{
    tottrs++;
}
   
static char	*hellname = CNULL;
static int	hellroom;
void
set_hell(s)
    char	*s;
{
    if (hellname != CNULL)
    {
	muldef("hell");
    }
    hellname = s;
}

static char	*dirs[6] =
    {
	"North", "South", "East", "West", "Up", "Down"
    };
static bool	dirs_set = FALSE;
void
set_dirs(n, s, e, w, u, d)
    char	*n;
    char	*s;
    char	*e;
    char	*w;
    char	*u;
    char	*d;
{
    if (dirs_set)
    {
	muldef("direction names");
    }
    dirs_set = TRUE;
    dirs[0] = n;
    dirs[1] = s;
    dirs[2] = e;
    dirs[3] = w;
    dirs[4] = u;
    dirs[5] = d;
}

void
init_nouns()
{
    static bool	initted = FALSE;
    char	*shortname = "?";
    int		d;

    if (initted)
    {
	return;
    }
    initted = TRUE;
    set_nouns();
    for (d=0; d<6; d++)
    {
	addsym(dirs[d], 1);
	shortname[0] = dirs[d][0];
	addsym(strsave(shortname), 0);
    }
}

void
init_verbs()
{
    static bool	initted = FALSE;
    void	add_list();

    if (initted)
    {
	return;
    }
    initted = TRUE;
    set_verbs();
    add_list(&go_list, &def_go);
    add_list(&get_list, &def_get);
    add_list(&drop_list, &def_drop);
}

static bool	greeting_set = FALSE;
void
set_greeting(s)
    char	*s;
{
    if (greeting_set)
    {
	muldef("greeting message");
    }
    greeting_set = TRUE;
    greetmsg(s);
}

static char	*lampname = CNULL;
static int	lamp;
void
set_lamp(s)
    char	*s;
{
    if (lampname != CNULL)
    {
	muldef("lamp name");
    }
    lampname = s;
}

int		wsize = 0;	/*  Referenced as extern elsewhere */
void
set_wsize(n)
    int		n;
{
    if (n == 0)
    {
	badzero("wordsize");
	return;
    }
    else if (n > MAXWSIZE)
    {
	gramerror(FALSE, "warning - unreasonable wordsize (> %d)", MAXWSIZE);
    }
    if (wsize != 0)
    {
	muldef("wordsize");
    }
    wsize = n;
}

static char	*startname = CNULL;
static int	startroom;
void
set_start(s)
    char	*s;
{
    if (startname != CNULL)
    {
	muldef("start room");
    }
    startname = s;
}

static char	*trsname = CNULL;
static int	trsroom;
void
set_treasury(s)
    char	*s;
{
    if (trsname != CNULL)
    {
	muldef("treasury room");
    }
    trsname = s;
}

static int	invsize = 0;
void
set_inventory(n)
    int		n;
{
    if (n == 0)
    {
	badzero("inventory size");
	return;
    }
    else if (n > MAXINVSIZE)
    {
	gramerror(FALSE, "warning - unreasonable inventory size (> %d)",
			MAXINVSIZE);
    }
    if (invsize != 0)
    {
	muldef("inventory size");
    }
    invsize = n;
}

static int	lamplife = 0;
void
set_llife(n)
    int		n;
{
    if (n == 0)
    {
	badzero("lamp life");
	return;
    }
    if (lamplife != 0)
    {
	muldef("lamplife");
    }
    lamplife = n;
}

static int	lampwarn = 0;
void
set_lwarn(n)
    int		n;
{
    if (n == 0)
    {
	badzero("lamp warning");
	return;
    }
    if (lampwarn != 0)
    {
	muldef("lamp warning");
    }
    lampwarn = n;
}

void
set_go_list()
{
    cur_list = &go_list;
}

void
set_get_list()
{
    cur_list = &get_list;
}

void
set_drop_list()
{
    cur_list = &drop_list;
}

void
addspecialword(s)
    char	*s;
{
    if (cur_list->wl_count == MAXWORDLIST)
    {
	gramerror(FALSE,
		"warning: too many synonyms for %s", cur_list->wl_word[0]);
    }
    cur_list->wl_word[cur_list->wl_count++] = s;
}

static void
add_list(list, deflt)
    wordlist	*list;
    wordlist	*deflt;
{
    wordlist	*wl = list->wl_count ? list : deflt;
    int		i;

    addsym(wl->wl_word[0], 1);
    for (i=1; i<wl->wl_count; i++)
    {
	addsym(wl->wl_word[i], 0);
    }
}

void
checkspecials()
{
    hellroom = val(hellname, S_ROOM, "hell room");
    lamp     = val(lampname, S_ITEM, "lamp");
    startroom= val(startname,S_ROOM, "start room");
    trsroom  = val(trsname,  S_ROOM, "treasury");

    if (startname == CNULL)
    {
	gramerror(FALSE, "warning - no start room defined");
	startroom = firstroom();
    }
    if (trsname == CNULL)
    {
	gramerror(FALSE, "warning - no treasure room defined");
	trsroom = firstroom();
    }
    if (tottrs == 0)
    {
	gramerror(FALSE, "warning - no treasures among items");
    }
}

static int
val(name, type, title)
    char	*name;
    int		type;
    char	*title;
{
    symbol	*s;
    
    if (name == CNULL)
    {
	return 0;
    }
    s = lookup(name);
    if (s->s_type != type)
    {
	gramerror(FALSE, "%s - %s is undeclared", title, name);
	add_error();
    }
    return s->s_value;
}

void
printspecials(f)
    FILE	*f;
{
    if (hellname != CNULL)
    {
	fprintf(f, "!h%d\n", hellroom);
    }
    if (wsize == 0)
    {
	wsize = DEFWORDSIZE;
    }
    else
    {
	fprintf(f, "!w%d\n", wsize);
    }
    if (dirs_set)
    {
	fprintf(f, "!d%s %s %s %s %s %s\n", dirs[0], dirs[1], dirs[2],
			dirs[3], dirs[4], dirs[5]);
    }
    if (lampname != CNULL)
    {
	fprintf(f, "!l%d\n", lamp);
    }
    fprintf(f, "%d\n%d\n%d\n%d\n%d\n%d\n",
    		startroom, trsroom, tottrs,
		invsize == 0 ? DEFINVSIZE : invsize,
		lamplife == 0 ? DEFLLIFE : lamplife,
		lampwarn == 0 ? DEFLWARN : lampwarn);
}
