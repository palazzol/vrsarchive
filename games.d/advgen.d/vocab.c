#include	"advgen.h"

static wordtable	noun_table = { 1, 1, 0, { { "--", NOTFOUND } } };
static wordtable	verb_table = { 1, 1, 0, { { "--", NOTFOUND } } };
static wordtable	*cur_table = WNULL;

void
set_nouns()
{
    cur_table = &noun_table;
}

void
set_verbs()
{
    cur_table = &verb_table;
}

void
addword(sym, incr)
    symbol	*sym;
    int		incr;
{
    void	toomanywords();
    int		i;
    bool	dup = FALSE;
    int		val = cur_table->wt_value + incr;
    char	*s = sym->s_sym;

    if (cur_table == WNULL)
    {
	addspecialword(s);
	return;
    }
    for (i = 0; i < cur_table->wt_size; i++)
    {
	if (equivalent(cur_table->wt_vocab[i].v_word, s))
	{
	    if (incr || cur_table->wt_value != cur_table->wt_vocab[i].v_value)
	    {
		gramerror(FALSE, "warning - '%s' is equivalent to '%s'", s,
				cur_table->wt_vocab[i].v_word);
		val = cur_table->wt_vocab[i].v_value;
	    }
	    dup = TRUE;
	    break;
	}
    }
    if (cur_table->wt_size == MAXVOCAB)
    {
	toomanywords();
    }
    if (!dup)
    {
	cur_table->wt_wcount++;
	if (incr != 0)
	{
	    cur_table->wt_value += incr;
	}
    }
    cur_table->wt_vocab[cur_table->wt_size].v_dup = dup;
    cur_table->wt_vocab[cur_table->wt_size].v_word = s;
    cur_table->wt_vocab[cur_table->wt_size].v_value = val;
    if (cur_table == &noun_table)
    {
	sym->s_noun = cur_table->wt_size++;
    }
    else
    {
	sym->s_verb = cur_table->wt_size++;
    }
}

static void
toomanywords()
{
    fatal("too many %ss", cur_table == &noun_table ? "noun" : "verb");
}

int
findnoun(s, flag)
    symbol	*s;
    bool	flag;
{
    int		val;

    if (s == SNULL)
    {
	return NOTFOUND;
    }
    else
    {
	val = s->s_noun;
	if (flag && val==0)
	{
	    gramerror(TRUE, "%s is not a noun", s->s_sym);
	    add_error();
	}
	return val == 0 ? NOTFOUND : noun_table.wt_vocab[val].v_value;
    }
}

int
findverb(s, flag)
    symbol	*s;
    bool	flag;
{
    int		val;

    if (s == SNULL)
    {
	return NOTFOUND;
    }
    else
    {
	val = s->s_verb;
	if (flag && val == 0)
	{
	    gramerror(TRUE, "%s is not a verb", s->s_sym);
	    add_error();
	}
	return val == 0 ? NOTFOUND : verb_table.wt_vocab[val].v_value;
    }
}

char *
noun_name(i)
    smallint	i;
{
    return noun_table.wt_vocab[i].v_word;
}

void
printvocab(f)
    FILE	*f;
{
    void	printtable();

    printtable(f, &verb_table);
    printtable(f, &noun_table);    
}

static void
printtable(f, table)
    FILE	*f;
    wordtable	*table;
{
    char	*upcase();
    int		i, count = 10, val = 0;
    
    fprintf(f, "%d", table->wt_wcount);
    for (i = 0; i < table->wt_size; i++)
    {
	if (table->wt_vocab[i].v_dup)
	{
	    continue;
	}
	if (count++ == 10)
	{
	    count = 0;
	    (void) putc('\n', f);
	}
	else
	{
	    (void) putc(',', f);
	}
	if (val == table->wt_vocab[i].v_value)
	{
	    (void) putc('*', f);
	}
	else
	{
	    val = table->wt_vocab[i].v_value;
	}
	fprintf(f, "%s", upcase(table->wt_vocab[i].v_word));
    }
    putc('\n', f);
}

#include	<ctype.h>

static char
up(c)
    char	c;
{
    return islower(c) ? toupper(c) : c;
}

static char *
upcase(s)
    char	*s;
{
    extern int	wsize;
    static char	image[500];
    char	*cp;

    (void) strncpy(image, s, wsize);
    image[wsize] = '\0';
    for (cp = image; *cp; cp++)
    {
	*cp = up(*cp);
    }
    return image;
}

static bool
equivalent(s1, s2)
    char	*s1;
    char	*s2;
{
    extern int	wsize;
    int		i;
    int		limit = wsize ? wsize : DEFWORDSIZE;

    for (i=0; i<limit; i++)
    {
	if (up(s1[i]) != up(s2[i]))
	{
	    return FALSE;
	}
	if (s1[i] == '\0')
	{
	    return TRUE;
	}
    }
    return TRUE;
}
