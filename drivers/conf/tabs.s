|
|	@(#) conf/oemsup.s 1.1 84/05/07 Copyright Microsoft Corporation, 1983
|
|	This Module contains Proprietary Information of Microsoft
|	Corporation and AT&T, and should be treated as Confidential.

|
|
| THIS FILE CONTAINS CODE WHICH IS SPECIFIC TO THE
| INTEL 286/310 COMPUTER AND MAY REQUIRE MODIFICATION
| WHEN ADAPTING XENIX TO NEW HARDWARE.
|
|	Modification History:
|		dilip	I000 - fixes in the idt and brkpt/ss trap. Unmarked.
|		llk	I001 - moved idt out of kernel data space.
|		rajiv	I002 - added selectors needed for the new line 
|			       discipline ring buffers.
|



#include "../h/param.h"
#include "../h/mmu.h"
#include "../h/i286instr.h"
#include "../h/machdep.h"
#include "../h/dtab.h"

dca_text=DSA_VALID+DSA_PRESENT+DSA_EXEC+DSA_TREAD
dca_data=DSA_VALID+DSA_PRESENT+DSA_DWRITE
dca_tss=DST_ATASK+DSA_PRESENT
dca_table=DST_TABLE+DSA_PRESENT
dca_tgate=DST_TRGATE+DSA_PRESENT
dca_utgate=DST_TRGATE+DSA_PRESENT+DSA_RING3
dca_igate=DST_IGATE+DSA_PRESENT
dca_uigate=DST_IGATE+DSA_PRESENT+DSA_RING3
dca_cgate=DST_CGATE+DSA_PRESENT+DSA_RING3	| for kernel entry

|
| This file defines the global descriptor table, interrupt(and trap)
| descriptor table, tss, and the kernel area reserved for per process
| u area.
| LINK this module in FIRST to assure its in the right place
|
| The following is the reserved area in kernel data space for per process'
| u page. Save and resume (mdep/mch.s) copy processes upages to and from
| this area on a context switch. 
| WARNING - the space reserved here for stack + u must be >= usegsz (see mmu.h)
|	  - this area must be at location 0 in kernel data space.
|
.data
.globl	_u
_u:			| The _u structure must be located
			| at OFFUSRPG, as defined in param.h
.blkb	KSSIZE		| 1Kb stack
.blkb	/A00		| 2.5Kb : user structure + ldt
.globl	div_trap, _ss_trap, nmi_int, _bkpt_trap, oflo_trap, array_trap
.globl	inv_op_trap, ndp_np_trap, df_trap, ndp_ex_trap, inv_tss_trap
.globl	not_pres_trap, stack_trap, _gp_fault, int_bad, math_err
|
| Interrupt Descriptor Table -- defines what happens on each of
|	the interrupts. 
|	The idt has entries for one master 8259 interrupt controller
|	 and 8 slaves. All entries should remain in the table irregardless
|	 of how many slaves you have (if any). 
|
|	WARNING: the Single Step and Breakpoint idt entries
|	have a type of 0. This is so the monitor will fill in the
|	details. Kernel initialization will copy the kernel entries
|	out and fill in its own. When a SS or BKPT trap comes in,
|	if its in the kernel, it gets 'forwarded' to the monitor.
|	For ports without this monitor, the type should be changed
|	to dca_tgate for these and all interrupts.
|
|	Otherwise, this table should not have to be changed for a new 286 port.
|
|	I001 changed name of idt to indicate temporary nature of 
|	this usage of this space
|
.globl _idt_tmp
_idt_tmp:		| I001  
DT_TAB_ENTRY(div_trap,KCS_SEL,0,dca_tgate)	| IDT 0 -- divide error
#ifdef MONITOR
  DT_TAB_ENTRY(_mss_trap,KCS_SEL,0,0)		| IDT 1 -- single step
#else
  DT_TAB_ENTRY(_ss_trap,KCS_SEL,0,dca_utgate)	| IDT 1 -- single step
#endif
DT_TAB_ENTRY(nmi_int,KCS_SEL,0,dca_tgate)	| IDT 2 -- non maskable int
#ifdef MONITOR
  DT_TAB_ENTRY(_mbkpt_trap,KCS_SEL,0,0)		| IDT 3 -- breakpoint int
#else
  DT_TAB_ENTRY(_bkpt_trap,KCS_SEL,0,dca_utgate)	| IDT 3 -- breakpoint int
#endif
DT_TAB_ENTRY(oflo_trap,KCS_SEL,0,dca_tgate)	| IDT 4 -- overflow int
DT_TAB_ENTRY(array_trap,KCS_SEL,0,dca_tgate)	| IDT 5 -- array bounds check
DT_TAB_ENTRY(inv_op_trap,KCS_SEL,0,dca_tgate)	| IDT 6 -- invalid op code
DT_TAB_ENTRY(ndp_np_trap,KCS_SEL,0,dca_tgate)	| IDT 7 -- ndp not present
DT_TAB_ENTRY(df_trap,KCS_SEL,0,dca_tgate)	| IDT 8 -- double fault int
DT_TAB_ENTRY(ndp_ex_trap,KCS_SEL,0,dca_tgate)	| IDT 9 -- ndp exception int
DT_TAB_ENTRY(inv_tss_trap,KCS_SEL,0,dca_tgate)	| IDT 10 - invalid TSS int
DT_TAB_ENTRY(not_pres_trap,KCS_SEL,0,dca_tgate)	| IDT 11 - descr not present int
DT_TAB_ENTRY(stack_trap,KCS_SEL,0,dca_tgate)	| IDT 12 - stack (ov/und)erflow
DT_TAB_ENTRY(_gp_fault,KCS_SEL,0,dca_tgate)	| IDT 13 - general protection
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 14 - reserved
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 15 - reserved
DT_TAB_ENTRY(math_err,KCS_SEL,0,dca_tgate)	| IDT 16 - math error
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 17
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 18
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 19
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 20
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 21
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 22
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 23
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 24
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 25
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 26
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 27
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 28
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 29
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 30
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 31
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 32
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 33
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 34
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 35
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 36
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 37
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 38
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 39
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 40
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 41
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 42
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 43
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 44
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 45
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 46
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 47
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 48
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 49
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 50
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 51
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 52
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 53
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 54
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 55
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 56
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 57
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 58
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 59
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 60
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 61
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 62
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_tgate)	| IDT 63
DT_TAB_ENTRY(int_0,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_1,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_2,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_3,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_4,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_5,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_6,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_7,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s0m0,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s1m0,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s2m0,KCS_SEL,0,dca_igate)     
DT_TAB_ENTRY(int_s3m0,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s4m0,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s5m0,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s6m0,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s7m0,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s0m1,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s1m1,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s2m1,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s3m1,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s4m1,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s5m1,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s6m1,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s7m1,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s0m2,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s1m2,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s2m2,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s3m2,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s4m2,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s5m2,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s6m2,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s7m2,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s0m3,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s1m3,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s2m3,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s3m3,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s4m3,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s5m3,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s6m3,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s7m3,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s0m4,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s1m4,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s2m4,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s3m4,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s4m4,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s5m4,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s6m4,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s7m4,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s0m5,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s1m5,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s2m5,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s3m5,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s4m5,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s5m5,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s6m5,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s7m5,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s0m6,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s1m6,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s2m6,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s3m6,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s4m6,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s5m6,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s6m6,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s7m6,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s0m7,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s1m7,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s2m7,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s3m7,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s4m7,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s5m7,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s6m7,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_s7m7,KCS_SEL,0,dca_igate)
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 136
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 137
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 138
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 139
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 140
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 141
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 142
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 143
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 144
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 145
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 146
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 147
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 148
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 149
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 150
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 151
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 152
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 153
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 154
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 155
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 156
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 157
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 158
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 159
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 160
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 161
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 162
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 163
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 164
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 165
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 166
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 167
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 168
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 169
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 170
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 171
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 172
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 173
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 174
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 175
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 176
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 177
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 178
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 179
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 180
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 181
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 182
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 183
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 184
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 185
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 186
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 187
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 188
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 189
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 190
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 191
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 192
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 193
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 194
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 195
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 196
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 197
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 198
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 199
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 200
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 201
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 202
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 203
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 204
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 205
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 206
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 207
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 208
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 209
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 210
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 211
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 212
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 213
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 214
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 215
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 216
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 217
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 218
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 219
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 220
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 221
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 222
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 223
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 224
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 225
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 226
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 227
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 228
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 229
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 230
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 231
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 232
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 233
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 234
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 235
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 236
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 237
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 238
DT_TAB_ENTRY(int_bad,KCS_SEL,0,dca_igate)	| INT 239
|
DT_TAB_ENTRY(_sw_240,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_241,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_242,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_243,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_244,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_245,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_246,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_247,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_248,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_249,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_250,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_251,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_252,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_253,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_254,KCS_SEL,0,dca_utgate)
DT_TAB_ENTRY(_sw_255,KCS_SEL,0,dca_utgate)
|
| The task state segment is defined below. It is used in interrupt
| and trap handling and should not change for a new port
|
.globl	_tss,_ktssip,_ktssfl,_ktssax,_ktsscx,_ktssdx,_ktssbx,_ktsssp
.globl	_ktssbp,_ktsssi,_ktssdi,_ktsses,_ktsscs,_ktssss,_ktssds,_tssldt
_tss:
_ktsslink:	.word	#NULL_SEL		| Link
		.word	#_u+KSSIZE		| sp0  kernel stack is the first
		.word	#KSS_SEL		| ss0  element of the u struct
		.word	#0 			| sp1
		.word	#NULL_SEL		| ss1
		.word	#0			| sp2
		.word	#NULL_SEL		| ss2
_ktssip:	.word	[pstart]		| ip
_ktssfl:	.word	0			| flags
_ktssax:	.word	0			| ax
_ktsscx:	.word	0			| cx
_ktssdx:	.word	0			| dx
_ktssbx:	.word	0			| bx
_ktsssp:	.word	#_u+KSSIZE		| sp	
_ktssbp:	.word	0			| bp
_ktsssi:	.word	0			| si
_ktssdi:	.word	0			| di
_ktsses:	.word	#KDS_SEL		| es
_ktsscs:	.word	#KCS_SEL		| cs
_ktssss:	.word	#KSS_SEL		| ss
_ktssds:	.word	#KDS_SEL		| ds
_tssldt:	.word	#CLDT_SEL		| ldt selector
.blkb	4			| pad to mod 16 boundary
|
| Global Descriptor Table --- statically initializes the GDT to
| various entries. 
|	WARNING:  Entries 1,2,4 and 11 are initialized for the
| SBC 286/10 monitor so that we can use it in protected mode
| even though we turn on PM.
| Descriptors 0 - 138 and 1B0 and 1B8 are for the monitor and should be 
| removed on ports with no monitor. 
| Remember to change mmu.h to match this table.
|
| NOTES-the gdt, tss, stack and data live in the same segment
|	hence the funny decl for stack & data seg
|    -  The memory for the kernel starts at KMEMSTART; The following
|       assumptions are in force:
|		- Kernel data starts in the first 64K (ie 0x0dddd)
|		- Kernel text starts in the second 64K (ie 0x1dddd)
|    -  The selector values defined in the include file mmu.h MUST 
|	agree with the gdt table entries in order for this xenix to work.
|    -  Descriptors 140 through 1E8 (the beginning of special io descriptors
|       for this port) should not have to change for a new port.
|    -  Remember to paragraph align the last entry (LAST_SEL)
|    -  For middle-model kernel, the initialization code at start: will
|	fill in the limits and base addresses of the kernel text and
|	data segments. This information is kept at a location pointed
|	to by dx and bx; the bootstrap sets up these two registers
|	immediately before jumping to the kernel.
|
|
.globl	gdt_addr, idt_addr
.globl _gdt
_gdt:
D_TAB_ENTRY(0,0,0,0)				| 0   empty first slot
D_TAB_ENTRY(LAST_SEL+7,_gdt+KMEMSTART,0,dca_data)	| 8 GDT alias
idt_alias:					| 		I001
D_TAB_ENTRY(0x07FF,0,0,dca_data)		| 10  idt alias I001 
D_TAB_ENTRY(0,0,0,0)				| 18  for monitor
D_TAB_ENTRY(0x05B60,0x08240,0x0FF,dca_text)	| 20  mon code seg
D_TAB_ENTRY(0,0,0,0)				| 28  for monitor
D_TAB_ENTRY(0,0,0,0)				| 30  for monitor
D_TAB_ENTRY(0,0,0,0)				| 38  for monitor
D_TAB_ENTRY(0,0,0,0)				| 40  for monitor
D_TAB_ENTRY(0,0,0,0)				| 48  for monitor
D_TAB_ENTRY(0,0,0,0)				| 50  for monitor
D_TAB_ENTRY(/07F,/0FF80,/0FF,dca_text)		| 58  mon restart seg
D_TAB_ENTRY(0,0,0,0)				| 60  for monitor
D_TAB_ENTRY(0,0,0,0)				| 68  for monitor
D_TAB_ENTRY(0,0,0,0)				| 70  for monitor
D_TAB_ENTRY(0,0,0,0)				| 78  for monitor
D_TAB_ENTRY(0,0,0,0)				| 80  for monitor
D_TAB_ENTRY(0,0,0,0)				| 88  for monitor
D_TAB_ENTRY(0,0,0,0)				| 90  for monitor
D_TAB_ENTRY(0,0,0,0)				| 98  for monitor
D_TAB_ENTRY(0,0,0,0)				| A0  for monitor
D_TAB_ENTRY(0,0,0,0)				| A8  for monitor
D_TAB_ENTRY(0,0,0,0)				| B0  for monitor
D_TAB_ENTRY(0,0,0,0)				| B8  for monitor
D_TAB_ENTRY(0,0,0,0)				| C0  for monitor
D_TAB_ENTRY(0,0,0,0)				| C8  for monitor
D_TAB_ENTRY(0,0,0,0)				| D0  for monitor
D_TAB_ENTRY(0,0,0,0)				| D8  for monitor
D_TAB_ENTRY(0,0,0,0)				| E0  for monitor
D_TAB_ENTRY(0,0,0,0)				| E8  for monitor
D_TAB_ENTRY(0,0,0,0)				| F0  for monitor
D_TAB_ENTRY(0,0,0,0)				| F8  for monitor
D_TAB_ENTRY(0,0,0,0)				| 100 for monitor
D_TAB_ENTRY(0,0,0,0)				| 108 for monitor
D_TAB_ENTRY(0,0,0,0)				| 110 for monitor
D_TAB_ENTRY(0,0,0,0)				| 118 for monitor
D_TAB_ENTRY(0,0,0,0)				| 120 for monitor
D_TAB_ENTRY(0,0,0,0)				| 128 for monitor
D_TAB_ENTRY(0,0,0,0)				| 130 for monitor
D_TAB_ENTRY(0,0,0,0)				| 138 for monitor
gdt_addr:
D_TAB_ENTRY(LAST_SEL+7,_gdt+KMEMSTART,0,dca_data)	| 140 GDT alias
idt_addr:
D_TAB_ENTRY(0x7FF,0,0,dca_data)			| 148 IDT alias I001
.globl	kdata_dscr
kdata_dscr:
D_TAB_ENTRY(0,0,0,0)				| 150 kernel ds
D_TAB_ENTRY(0,0,0,0)				| 158 kernel cs SEG 1
D_TAB_ENTRY(0,0,0,0)				| 160 kernel cs SEG 2
D_TAB_ENTRY(0,0,0,0)				| 168 kernel cs SEG 3
D_TAB_ENTRY(0,0,0,0)				| 170 kernel cs SEG 4
D_TAB_ENTRY(0,0,0,0)				| 178 kernel cs SEG 5
kss_dscr:
D_TAB_ENTRY(0,0,0,0)				| 180 kernel ss
D_TAB_ENTRY(0x2F,_tss+KMEMSTART,0,dca_tss)	| 188 tss
D_TAB_ENTRY(0x2F,_tss+KMEMSTART,0,dca_data)	| 190 tss alias
D_TAB_ENTRY(0,0,0,0)				| 198 Kernel work
D_TAB_ENTRY(0,0,0,0)				| 1A0 Kernel work
D_TAB_ENTRY(0,0,0,0)				| 1A8 Kernel work
D_TAB_ENTRY(0,0,0,0)				| 1B0 Kernel work - swap
D_TAB_ENTRY(0,0,0,0)				| 1B8 Kernel work - swap
D_TAB_ENTRY(0,0,0,0)				| 1C0 CLDT_SEL
DT_TAB_ENTRY(_kentry,KCS_SEL,1,dca_cgate)	| 1C8 Kernel Entry
D_TAB_ENTRY(0,0,0,0)				| 1D0 Map Work fuss
.globl	_ss_descr, _bkpt_descr
_ss_descr:
#ifdef MONITOR
  DT_TAB_ENTRY(_mss_trap,KCS_SEL,0,dca_cgate)	| 1D8 Mon SS gate for monitor
#else
  DT_TAB_ENTRY(_ss_trap,KCS_SEL,0,dca_cgate)	| 1D8 Mon SS gate for monitor
#endif
_bkpt_descr:
#ifdef MONITOR
  DT_TAB_ENTRY(_mbkpt_trap,KCS_SEL,0,dca_cgate)	| 1E0 Mon Bkpt gate for monitor
#else
  DT_TAB_ENTRY(_bkpt_trap,KCS_SEL,0,dca_cgate)	| 1E0 Mon Bkpt gate for monitor
#endif
D_TAB_ENTRY(0,0,0,0)				| 1E8 COPY_SEL
D_TAB_ENTRY(0,0,0,0)				| 1F0 BUF_SEL
D_TAB_ENTRY(0,0,0,0)				| 1F8 BUF2_SEL
D_TAB_ENTRY(0,0,0,0)				| 200 BUF3_SEL
D_TAB_ENTRY(0,0,0,0)				| 208 I215_SEL
D_TAB_ENTRY(0,0,0,0)				| 210 tty selector TTY0_SEL
D_TAB_ENTRY(0,0,0,0)				| 218 Driver seg/DEV_DSEL
D_TAB_ENTRY(0,0,0,0)				| 220 Driver seg
D_TAB_ENTRY(0,0,0,0)				| 228 Driver seg
D_TAB_ENTRY(0,0,0,0)				| 230 Driver seg
D_TAB_ENTRY(0,0,0,0)				| 238 Driver seg 
D_TAB_ENTRY(0,0,0,0)				| 240 Driver seg 
D_TAB_ENTRY(0,0,0,0)				| 248 Driver seg 
D_TAB_ENTRY(0,0,0,0)				| 250 Driver 0eg
D_TAB_ENTRY(0,0,0,0)				| 258 Driver seg/KHISTO_SEL
D_TAB_ENTRY(0,0,0,0)				| 260 Driver seg/DHISTO_SEL
D_TAB_ENTRY(0,0,0,0)				| 268 LAST_SEL para aligned

	.globl	_fupage
_fupage: 	.word	0	| first user page of memory, set below
	.globl	_kmemstart
_kmemstart:	.word	#KMEMSTART

	.globl	picmask, picbit
picmask:
	.byte	/FF, /02, /04, /08, /10, /20, /40, /80
picbit:
	.byte	/01, /02, /04, /08, /10, /20, /40, /80

.text
| this is the compatibility mode start up code
MSW_PROTO=1	| turn on protected mode 

	.globl  strt
strt:
	| non protected start-up (PORTABLE)
	| assumptions:
	|	- Boot has left a nsegs count word, a data selector, and 
	|	  several code selectors for the kernel at the end of the 
	|	  text it loaded in. 
	|	  dx & bx contain the long address of the selector data.
	|	  They also happen to point to the base of the first free
	|	  page after kernel text.
	| WARNING: dx and bx are RESERVED for the duration of this module.
	|	  (or until fpage is initialized)
	|

	|mov	ax,#/4
	|out	/30			| Enable NMI
	|mov	ax,#/f9
	|out	/31			| Enable NMI
	|mov	ax,#/24
	|out	/3c			| Enable NMI

	mov	ax,#KMEMSTART		| ax <-- address of data
	SHRI(Rax,4)			| convert to base
	mov	ds,ax			| establish addressability
	mov	ss,ax			| stack also
	mov	sp,#KSSIZE		| 1K stack

	|
	| Nextfree is a long passed to us in dx & bx by boot, high word in dx.
	| Nextfree also happens to be the address of the base of
	| Initialize the kernel's ds,ss, and cs gdt entries from information
	|
	| copy count bytes from seg:off in real memory to ds:gdt.
	|
	| copy (nextfree) bytes from (nextfree + 2) to gdt at ds:kdata_dscr.
	| NOTE: the call to moninit below should be removed for ports
	|	without the monitor.

					| destination is the gdt (es:di)
	mov	ax,ds
	mov	es,ax
	mov	di,#kdata_dscr		| address of kernel data descriptor
					| source is pointed to by dx:bx	
	mov	si,bx			| bx = offset of address of info
	add	si,#2			| skip over number of segments count
	
	mov	ax,dx			| dx = high word of address
	movb	cl,#12			| convert to segment value
	shl	ax,cl
	mov	ds,ax			| save computed ds

				| get count from nextfree, using NEW ds
	mov	ax,(bx)			| bx points to count of # of descriptors
	movb	cl,#3			| multiply by # bytes per descriptor (8)
	shl	ax,cl
	mov 	cx,ax			| count	

	cld				| clear direction
	rep
	movsb				| copy (cx) bytes from ds:si to es:di
	|
	| kernel stack is the same as kernel data, so initialize the stack 
	| descriptor with a  copy of the data one	
	|
	mov	di,#kss_dscr		| address of kernel stack descriptor
	mov	si,bx			| bx = offset of address of info
	add	si,#2			| skip over number of segments count
	mov	cx,#8			| load 1 8 byte descriptor
	cld
	rep
	movsb				| copy (cx) bytes from ds:si to es:di

	| I001 begin
	| Here we copy the idt out of the kernel's ds to save space.
	mov	ax, ds			| swap ds and es
	mov	cx, es
	mov	es, ax
	mov	ds, cx

	| Before we establish gdt, must fill out the 2 idt alias descriptors
	| (the base address of the idt is no longer statically defined).
	movb	idt_addr+4, dl		| high byte of base addr
	mov	idt_addr+2, bx		| low byte of base addr
	movb	idt_alias+4, dl		| high
	mov	idt_alias+2, bx		| low

	| copy out idt
	mov	di,bx			| first free memory address
	mov	si,#_idt_tmp		| address of idt
	mov	cx,#128 * SELSZ		| idt = 256 entries = 128*SELSZ words
	cld
	rep
	movs				| copy (cx) bytes from ds:si to es:di
	| end I001

	mov	ax,ss
	mov	ds,ax			| restore kernel ds

	mov	kss_dscr,#KSSIZE-1	| force ss limit to sizeof kernel stack
	
	LGDT(gdt_addr)			| establish gdt
	LIDT(idt_addr)			| establish idt

	mov	ax,#MSW_PROTO		| ax <-- protection bit
	LMSWR(Rax)			| turn protection on
	jmp	flush			| flush prefetch queue
flush:
	mov	ax,#0			| set ldt reg to 0
	LLDTR(Rax)
	jmpi	cl_buf,#KCS_SEL		| load CS
cl_buf:
	mov	ax,#KDS_SEL		| load DS,SS,ES with selector
	mov	ds,ax
	mov	es,ax
	mov	ax,#KSS_SEL
	mov	ss,ax
	mov	sp,#KSSIZE		| 1K stack

	mov	ax,#KTSS		| load TR with Kernel TSS
	LTRR(Rax)

	add	bx, #256*SELSZ		| I001 add idt size to dx:bx
	adc	dx, #0			| I001
	mov	cx,#LMMPGSZ		| convert value in dx:bx (address of 
sloop:
	sar	dx,*1			| first free page of memory after
	rcr	bx,*1			| kernel space) to page number
	loop	sloop			| i.e. btoms(dx:bx)
	mov	_fupage,bx		| store in fupage
#ifdef MONITOR
	calli   mon_init                | initialize monitor
#endif
	jmpi	pstart			| jump to protected start up


#ifdef MONITOR

| WARNING: single step and breakpoint are defined specially to be used
| with the monitor. If no monitor is being used, these entries should
| be defined in the same way all other entries are.
|
| traps 10-13 (invalid tss --- GP fault) cause an exception
| to be pushed onto the stack. This is cleared to allow a
| simple return mechanism
|
.globl _mss_trap
_mss_trap:
| 		jmpi	0,#MSS_GATE		| call monitor
| 		iret
|
 		push	bx			| bx is scratch pad
		push	bp
 		mov	bp,sp			| bp points to stack
 		mov	bx,#6(bp)		| bx <-- cs
 		test	bx,#SEL_LDT		| in LDT or GDT?
 		jne	ss_ok
|		cmp	sp,#/3f6
|		jz	ss_ok
		pop	bp
 		pop	bx			| restore ax
		| not ldt based cs, must be in kernel
		jmpi	0,#MSS_GATE		| call monitor
		iret

ss_ok:		| ldt based cs, must be a user
		pop	bp
 		pop	bx			| restore ax
		jmpi	_ss_trap

.globl _mbkpt_trap
_mbkpt_trap:
| 		jmpi	0,#MBRK_GATE		| call monitor
|
 		push	bx			| bx is scratch pad
		push	bp
 		mov	bp,sp			| bp points to stack
 		mov	bx,#6(bp)		| bx <-- cs
 		test	bx,#SEL_LDT		| in LDT or GDT?
 		jne	bkpt_ok
|		cmp	sp,#/3f6
|		jz 	bkpt_ok
		pop	bp
 		pop	bx
		| not ldt based cs, must be in kernel
		jmpi	0,#MBRK_GATE		| call monitor
bkpt_ok:	| ldt based cs, must be a user
		pop	bp
 		pop	bx			| restore ax
		jmpi	_bkpt_trap
#endif MONITOR


|********************************************************************
|
|	WARNING: the following 2 routines should be removed if 
|	there is no monitor
| mon_init:
| 	initialize monitor for protected mode
|
|********************************************************************


#ifdef MONITOR

GDT_ALIAS=/8
RSTRT_SEG=/58
RSTRT_LIM=/7F
RSTRT_LBASE=/0FF80
RSTRT_HBASE=/0FF
RSTRT_ACC=/9B		| present, DPL=0,seg, exec, read, accessed

CODE_SEG=/20
CODE_LIMIT=/5000
CODE_LBASE=/8240
CODE_HBASE=/0FF
CODE_ACC=/9B		| present, DPL=0,seg, exec, read, accessed


.globl	mon_init
mon_init:
	|
	| must make this procedure look like it was called long
	| ip already on stack
	|
	pushf		| monitor expects this
	cli
	| all the gdt entries were initialized statically
	| in conf/tabs, jump to monitors restart
	jmpi	0,#RSTRT_SEG

|***
|monitor -- trap to monitor
|	use the breakpoint gate to gain entry
|	(simulate SW interrupt)
|***
	.globl	_monitor
_monitor:
	pushf
	calli	0,#MBRK_GATE
	reti
#endif MONITOR

|***                                    ***
|***    spl?(), splx(p)                 ***
|***                                    ***
|*
|*	spl0()	Enables all levels (Everything ON)
|*	spl5()	Disables PIC 2-7 
|*	spl6()	Disables PIC 1-7 (Everything but clock OFF)
|*	spl7()	Disables PIC 0-7 (Everything OFF)
|*
|* Spl?() return old priority; splx(pri) restores saved level.
|*
|*
|* Note: interrupts must be off to put new value in PIC_MASK, in case more
|*	 levels are being masked (have race with PIC if ints on).


|***    tasktime() - Put system in task state.
|*
|*      tasktime(i) puts the system in 'task' mode.  Any pending interrupts
|*      are dismissed, further interrupts are allowed.
|*
|*      i = 0   - initial enabling of interrupts during bootup
|*      i = 1   - going task-time during lightning bolt
|*      i = 2   - going task-time during system call processing
|*
|*      Note that we may already be setup tasktime when tasktime() is
|*      called.  For example, clock() may call twice...


	.globl   _spl0,_spl2,_spl5,_spl6,_spl7,_splx,_splcli,_splbuf
	.globl  _tasktime

_tasktime:                              | same as spl0 for here
	| 
	| enable ints, to allow further interrupts during
	| lightening bolts
	|
_spl0:					| Everything ON
	movb	ah, #SPL0MASK		| PIC mask for all enabled
	j	spl			| finish.
_spl2:					| Disable 6-7
	movb	ah, #/C0		| PIC mask for 6-7 disabled
	j	spl			| finish
_splcli:
_spl5:					| Disable 2-7
	movb	ah, #/FC		| PIC mask for 2-7 disabled
	j	spl			| finish.
_splbuf:
_spl6:					| Disable 1-7
	movb	ah, #/FE		| PIC mask for 1-7 disabled
	j	spl			| finish.
_spl7:					| ALL OFF!
	movb	ah, #/FF		| Disable 0-7
spl:					| Common finish.
	cli				| zap ints while doing this
	mov	dx,_picimr
	in				| get old value
	xchgb	ah, al			| swap old/new mask

	orb	al,_maxmask	 	| mask out unused int levels
	out				| output new mask
	sti				| turn ON
	reti                            | ah = old mask, al = new mask

_splx:					| Restore old "state"
	mov	bx, sp			| frame pointer
	movb    al, #5(bx)              | al = old mask
	mov	dx,_picimr
	out				| set in PIC
	reti

.globl	_en_int, _dis_int
_en_int:
	sti
	reti
_dis_int:
	cli
	reti

| __acrtused
| The compiler puts out references to this variable to force crt0 to be 
| pulled in from /lib for normal C programs.  We resolve it here when 
| making a kernel since the C startup routines are not included in the 
| making of the kernel.
	.globl	__acrtused

__acrtused:			| Just define the symbol.

	.end
