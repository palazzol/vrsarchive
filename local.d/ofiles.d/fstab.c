#ifndef lint
static char sccsid[] = "%W% %G%";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <mnttab.h>
#include "fstab.h"
#ifndef PNMNTTAB
#define PNMNTTAB "/etc/mnttab"
#endif

static struct fstab fs;
static struct mnttab mnt;
static FILE *fd = 0;

setfsent()
{ if (fd)
    endfsent();
  if ((fd = fopen(PNMNTTAB, "r")) == NULL) {
    fd = 0;
    return (0);
  }
  return (1);
}

endfsent()
{ if (fd) {
    fclose(fd);
    fd = 0;
  }
  return (1);
}

struct fstab *
getfsent()
{ int nbytes;

  if ((fd == 0) && (setfsent() == 0))
    return ((struct fstab *)0);
  while (1) {
    nbytes = fread((char *)&mnt, sizeof mnt, 1, fd);
    if (nbytes != 1)
      return ((struct fstab *)0);
    if (mnt.mt_dev[0] == 0)
      continue;
    fs.fs_spec = mnt.mt_filsys;
    fs.fs_file = mnt.mt_dev;
    fs.fs_type = mnt.mt_ro_flg? "r" : "rw";
    fs.fs_freq = 1;
    fs.fs_passno = 1;
    return (&fs);
  }
}

struct fstab *
getfsspec(name)
char *name;
{ register struct fstab *fsp;

  if (setfsent() == 0)    /* start from the beginning */
    return ((struct fstab *)0);
  while((fsp = getfsent()) != 0)
    if (strcmp(fsp->fs_spec, name) == 0)
      return (fsp);
  return ((struct fstab *)0);
}

struct fstab *
getfsfile(name)
char *name;
{ register struct fstab *fsp;

  if (setfsent() == 0)    /* start from the beginning */
    return ((struct fstab *)0);
  while ((fsp = getfsent()) != 0)
    if (strcmp(fsp->fs_file, name) == 0)
      return (fsp);
  return ((struct fstab *)0);
}

struct fstab *
getfstype(type)
char *type;
{ register struct fstab *fs;

  if (setfsent() == 0)
    return ((struct fstab *)0);
  while ((fs = getfsent()) != 0)
    if (strcmp(fs->fs_type, type) == 0)
      return (fs);
  return ((struct fstab *)0);
}
