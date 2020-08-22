/*
 * addresses for the 82258 ADMA controller chip
 *
 */
#define ADMA_BASE     0x200

#define ADMA_GCR     ADMA_BASE+0x00
#define ADMA_SCR     ADMA_BASE+0x02
#define ADMA_GSR     ADMA_BASE+0x04
#define ADMA_GMR     ADMA_BASE+0x08
#define ADMA_GBR     ADMA_BASE+0x0A
#define ADMA_GDR     ADMA_BASE+0x0C

#define ADMA_CSR0      ADMA_BASE+0x10
#define ADMA_CPRL0     ADMA_BASE+0x20
#define ADMA_CPRH0     ADMA_BASE+0x22
#define ADMA_BCRL0     ADMA_BASE+0x38
#define ADMA_BCRH0     ADMA_BASE+0x3A

#define ADMA_CSR1      ADMA_BASE+0x50
#define ADMA_CPRL1     ADMA_BASE+0x60
#define ADMA_CPRH1     ADMA_BASE+0x62
#define ADMA_BCRL1     ADMA_BASE+0x68
#define ADMA_BCRH1     ADMA_BASE+0x6A

#define ADMA_CSR2      ADMA_BASE+0x90
#define ADMA_CPRL2     ADMA_BASE+0xA0
#define ADMA_CPRH2     ADMA_BASE+0xA2
#define ADMA_BCRL2     ADMA_BASE+0xA8
#define ADMA_BCRH2     ADMA_BASE+0xAA

#define ADMA_CSR3      ADMA_BASE+0xD0
#define ADMA_CPRL3     ADMA_BASE+0xE0
#define ADMA_CPRH3     ADMA_BASE+0xE2
#define ADMA_BCRL3     ADMA_BASE+0xE8
#define ADMA_BCRH3     ADMA_BASE+0xEA

#define ADMA_WRITE     0x8005
#define ADMA_READ      0x4050

#define DMA_CONTROL    0xD1
#define DMA_SCSI       0x03

#define CH0_MASK       0x0C
#define START_CH0      0x12
#define STOP_CH0       0x14
