
#include "../h/pty.h"
#include "../h/param.h"
#include "../h/dir.h"
#include "../h/a.out.h"
#include "../h/user.h"
#include "../h/systm.h"
#include "../h/tty.h"
#include "../h/conf.h"
#include "../h/buf.h"
#include "../h/file.h"
#include "../h/ioctl.h"

#define BUFSIZ 100
#define ALLDELAYS (NLDLY|TABDLY|CRDLY|VTDLY)

struct tty pt_tty[NPTY];

int    ve_pid[NPTY];
int    ve_oflag[NPTY];
struct ptydata ve_data[NPTY];
int    ptyopen[NPTY] = {0};
