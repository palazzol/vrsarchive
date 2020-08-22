#include    <stdio.h>
#include    <sys/types.h>
#include    <dir.h>
#include    <sys/stat.h>
#include    <pwd.h>

/* A hack!  A hack!  My kingdom for a hack! */

/* ngstats.c
 *
 * Generate news readership statistics.  Best run by cron, perhaps from
 * /usr/lib/news/trimlib.
 *
 * Written by Qux the Barbarian (Kaufman@Yale.Arpa, Kaufman@YaleCS.Bitnet,
 * ..!decvax!yale!kaufman).  Hereby placed in the Public Domain, for what
 * that's worth;  I would appreciate hearing about bugs/fixes/extensions
 * and ports to other Operating Systems or versions of Unix.
 *
 * Tested under 4.2 BSD; should work without changes under 4.1 BSD with the
 * ndir upward compatibility routines.
 */

#ifndef ACTIVE
#define ACTIVE "/usr/lib/news/active"
#endif
#ifndef SPOOLDIR
#define SPOOLDIR "/usr/spool/news"
#endif

#define WEEKS 7 * 24 * 60 * 60
#define READ_DELTA (2*WEEKS)
/*#define RAPIDOFLEX      /* don't actually gather stats */

struct NG {
    char name[100];
    int rdrs;
    int first_art;
    int last_art;
    int du;
    int artcnt;
} ngs[512];

int ngcnt = 0, unknownngcnt = 0;
int cmprdrs = -1;

#define rdr_du(a) ((a).rdrs ? (a).du / (a).rdrs : (a).du)

ngcmp(a, b)
struct NG *a, *b; {

    if ((cmprdrs && a->rdrs == b->rdrs) ||
       (!cmprdrs && rdr_du(*a) == rdr_du(*b)))
       return(strcmp(a->name, b->name));
    else if (cmprdrs)
       return(a->rdrs < b->rdrs ? 1 : -1);
    else
       return(rdr_du(*a) < rdr_du(*b) ? 1 : -1);
}

main (argc, argv)
int argc;
char **argv; {
    struct passwd *pp, *getpwent();
    FILE *fp;
    int newsrccnt = 0, i, noreadcnt = 0;
    int notreadcnt = 0, norccnt = 0;
    int accounts = 0, dusum = 0;
    char buf[100], c;
    long last_read_time;

    /* set up some time junk */
    time(&last_read_time);
    last_read_time -= READ_DELTA;

    /* read in active file */
    if ((fp = fopen(ACTIVE, "r")) == NULL) {
       perror(ACTIVE);
       exit(1);
    }
    while (!feof(fp)) {
       fscanf(fp, "%s %d %d %c\n",
                  ngs[ngcnt].name, &ngs[ngcnt].last_art,
                  &ngs[ngcnt].first_art, &c);
       ngs[ngcnt].rdrs = 0;
       ngs[ngcnt++].du = 0;
    }
    fclose(fp);

    /* Compute disk usages (hack hack hack) */
    for (i=0; i <ngcnt; i++) {
       ngs[i].artcnt = 0;
       dusum += ngdu(&ngs[i]);
    }

    /* Now read .newsrc's */
    while (pp = getpwent()) {
       accounts++;
       sprintf(buf, "%s/.newsrc", pp->pw_dir);
       if (access(buf, 0))
           norccnt++;
       else {
           struct stat stbuf;

           if (!stat(buf, &stbuf) && stbuf.st_mtime < last_read_time)
               notreadcnt++;
           else if ((fp = fopen(buf, "r")) == NULL)
               noreadcnt++;
           else {
               newsrccnt++;
               readnewsrc(fp, buf);
               fclose(fp);
           }
       }
    }

    /* sort the stats */
    qsort(ngs, ngcnt, sizeof(struct NG), ngcmp);

    /* Now, print the statistics */
    printf("\nFor %d accounts on system %s:\n", accounts, sysname());
    printf("\t%d have no .newsrc\n", norccnt);
    printf("\t%d have an unreadable .newsrc\n", noreadcnt);
    printf("\t%d have an out of date .newsrc\n", notreadcnt);
    printf("\t%d .newsrc's were read\n", newsrccnt);
    printf("\nFor %d newsgroups received:\n", ngcnt);
    printf("\t%d unknown newsgroups referenced in .newsrc's read.\n", unknownngcnt);
    printf("\t%d blocks total disk usage\n\n", dusum);
    puts("Statistics sorted by decreasing number of readers per newsgroup:\n");

    puts("Readers\t  # Articles\tDisk Space\tBlocks/Rdr\tNewsgroup");
    for (i=0; i < ngcnt; i++)
       printf("%5d\t   %5d\t  %6d\t  %5d\t\t%s\n",
           ngs[i].rdrs, ngs[i].artcnt,
           ngs[i].du, rdr_du(ngs[i]),
           ngs[i].name);


    /* resort the stats */
    cmprdrs = 0;
    qsort(ngs, ngcnt, sizeof(struct NG), ngcmp);

    /* Do it again sorted differently */
    puts("\nSorted by decreasing number of blocks per newsgroup reader:\n");
    puts("Readers\t  # Articles\tDisk Space\tBlocks/Rdr\tNewsgroup");
    for (i=0; i < ngcnt; i++)
       printf("%5d\t   %5d\t  %6d\t  %5d\t\t%s\n",
           ngs[i].rdrs, ngs[i].artcnt,
           ngs[i].du, rdr_du(ngs[i]),
           ngs[i].name);

}

readnewsrc(rcp, filename)
char *filename;
FILE *rcp; {
    char buf[100], *cp, *strchr();
    int i;

#ifdef RAPIDOFLEX
    return;
#endif RAPIDOFLEX

    while (fgets(buf, 100, rcp)) {

       if ((cp = strchr(buf, ':')) == NULL)
           continue;

       *cp = '\0';
       for (i = 0; i < ngcnt; i++)
           if (!strcmp(buf, ngs[i].name)) {
               ngs[i].rdrs++;
               break;
           }
       if (i == ngcnt)
           unknownngcnt++;
    }
}

ngdu(ng)
struct NG *ng; {
    char ngdir[256], ngart[256], *cp;
    struct stat stbuf;
    DIR *dir;
    struct direct *dirent;
    int pad = 0;    /* hack hack hack */
    int i;

#ifdef RAPIDOFLEX
    return(0);
#endif RAPIDOFLEX

    /* create directory name */
    sprintf(ngdir, "%s/%s", SPOOLDIR, ng->name);
    cp = ngdir;
    while (cp = strchr(cp, '.'))
       *cp = '/';

    /* calculate usage of all articles combined */
    if ((dir = opendir(ngdir)) == NULL) {
       perror(ngdir);
       return(0);
    }
    seekdir(dir, 2);
    while (dirent = readdir(dir)) {
       /* skip deleted files */
       if (!dirent->d_ino)
           continue;

       /* skip directories */
       sprintf(ngart, "%s/%s", ngdir, dirent->d_name);
       if (stat(ngart, &stbuf) || (stbuf.st_mode & S_IFMT) == S_IFDIR)
           continue;

       /* increment article count and disk usage */
       ng->artcnt++;
       if ((stbuf.st_mode & S_IFMT) == S_IFREG)
           ng->du += (stbuf.st_size + 511) / 512;
    }
    closedir(dir);

    /* compare article counts */
    if (ng->last_art == 0 && ng->first_art == 1)
       i = 0;
    else
       i = ng->last_art - ng->first_art;
    if (ng->artcnt != i  && ng->artcnt != i+1) {
          printf("Article count mismatch for newsgroup %s: ", ng->name);
          printf("active claims %d, actual is %d.\n", i, ng->artcnt);
    }
    return(ng->du);
}
