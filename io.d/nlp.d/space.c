/*
 *	Parallel port configuration
*/
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/systm.h"
#include "sys/conf.h"
#include "sys/termio.h"
#include "sys/tty.h"
#include "sys/file.h"
#include "sys/nlp.h"

ushort nlp_base[] = {
	0x03BC,		/* Parallel port on monochrome adapter */
	0x0378,		/* This one comes built in on Intel SYP301 */
	0x0278,		/* Add-on parrallel port */
};
#define NLP_PORTS (sizeof(nlp_base)/sizeof(nlp_base[0]))

int nlp_ports = NLP_PORTS;
struct nlpstate nlp_state[NLP_PORTS];
