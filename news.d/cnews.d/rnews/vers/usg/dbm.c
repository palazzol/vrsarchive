/*
 * Incredibly slow Uglix dbm simulation.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAXFILE 140
#define MAXLINE 1024

static char pagname[MAXFILE];
static FILE *db;
static int dbrdonly;

typedef struct {
	char *dptr;
	int dsize;
} datum;

dbminit(file)
char *file;
{
	char *strcat(), *strcpy();

	dbrdonly = 0;
	strcpy(pagname, file);
	strcat(pagname, ".pag");
	if ((db = fopen(pagname, "r+")) == NULL) {
		db = fopen(pagname, "r");
		dbrdonly = 1;
	}
	if (db == NULL) {
		warning("cannot open database %s\n", file);
		return -1;
	}
	return 0;
}

datum
fetch(key)
datum key;
{
	datum item;

	rewind(db);
	while (getitem(&item, db) != EOF)		/* read key */
		if (strncmp(item.dptr, key.dptr, key.dsize) == 0)
			if (getitem(&item, db) == EOF)	/* read data */
				break;
			else
				return item;
	/* EOF */
	item.dptr = NULL;
	item.dsize = 0;
	return item;
}

delete(key)
datum key;
{
	datum item;
	FILE *temp;
	FILE *tmpfile();

	if (dbrdonly)
		return -1;
	temp = tmpfile();
	if (temp == NULL)
		return -1;
	/* copy from db to temp, omitting key & its data */
	rewind(db);
	while (getitem(&item, db) != EOF)
		if (strncmp(item.dptr, key.dptr, key.dsize) == 0) {
			if (getitem(&item, db) == EOF)	/* toss data too */
				return -1;
		} else
			if (putitem(&item, temp) == EOF)
				return -1;
	/* copy back from temp to db */
	rewind(temp);
	db = freopen(pagname, "w+", db);
	while (getitem(&item, temp) != EOF)
		if (putitem(&item, db) == EOF)
			return -1;
	return 0;
}

store(key, dat)
datum key, dat;
{
	if (dbrdonly)
		return -1;
#ifdef REALDBM			/* else, it's only for news */
	if (delete(key) == -1)
		return -1;
#endif
	if (putitem(&key, db) == EOF || putitem(&dat, db) == EOF)
		return -1;
	return 0;
}

datum
firstkey()
{
	datum trash;
	datum nextkey();

	rewind(db);
	return nextkey(trash);
}

datum
nextkey(key)			/* simplistic version, ignores key */
datum key;
{
	static datum dat;

	if (getitem(&dat, db) == EOF)
		dat.dptr = NULL;
	return dat;
}

static int
getitem(datump, fp)			/* points at static storage */
datum *datump;
FILE *fp;
{
	static char data[MAXLINE];

	if (fread((char *)&datump->dsize, sizeof datump->dsize, 1, fp) != 1)
		return EOF;
	datump->dptr = data;
	if (fread(data, datump->dsize, 1, fp) != 1)
		return EOF;
	return 0;
}

static int
putitem(datump, fp)
datum *datump;
FILE *fp;
{
	if (fwrite((char *)&datump->dsize, sizeof datump->dsize, 1, fp) != 1)
		return EOF;
	if (fwrite(datump->dptr, datump->dsize, 1, fp) != 1)
		return EOF;
	return 0;
}
