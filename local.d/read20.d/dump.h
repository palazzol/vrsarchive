#ifdef COMMENT

	F O R M A T   O F   D U M P E R   T A P E S
	===========================================


EACH PHYSICAL RECORD WRITTEN BY DUMPER CONTAINS ONE OR MORE
LOGICAL RECORDS, EACH OF WHICH IS 518 (1006 OCTAL) WORDS LONG.

EACH LOGICAL RECORD HAS THE FOLLOWING FORMAT:

	!=======================================================!
CHKSUM  !          CHECKSUM OF ENTIRE 518-WORD RECORD           !  +0
	!-------------------------------------------------------!
ACCESS  !         PAGE ACCESS BITS (CURRENTLY NOT USED)         !  +1
	!-------------------------------------------------------!
TAPNO   !SCD!    SAVESET NUMBER     !        TAPE NUMBER        !  +2
	!-------------------------------------------------------!
PAGNO   !F1!F2!    FILE # IN SET    !      PAGE # IN FILE       !  +3
	!-------------------------------------------------------!
TYP     !              RECORD TYPE CODE (NEGATED)               !  +4
	!-------------------------------------------------------!
SEQ     !        RECORD SEQUENCE NUMBER (INCREASES BY 1)        !  +5
	!=======================================================!
	!                                                       !
	!         CONTENTS OF FILE PAGE IF DATA RECORD          !
	!        OTHER TYPES HAVE OTHER INFORMATION HERE        !
	!                                                       !
	!=======================================================!


TYPE	VALUE	MEANING
----	-----	-------
DATA	  0	CONTENTS OF FILE PAGE
TPHD	  1	NON-CONTINUED SAVESET HEADER
FLHD	  2	FILE HEADER (CONTAINS FILESPEC, FDB)
FLTR	  3	FILE TRAILER
TPTR	  4	TAPE TRAILER (OCCURS ONLY AFTER LAST SAVESET)
USR	  5	USER DIRECTORY INFORMATION
CTPH	  6	CONTINUED SAVESET HEADER
FILL	  7	NO MEANING, USED FOR PADDING


SCD (3 BITS) - 0=NORMAL SAVE, 1=COLLECTION, 2=ARCHIVE, 3=MIGRATION

F1 F2	MEANING
-- --	-------
 0  0	OLD-FORMAT TAPE (NO FILE # IN PAGNO BITS 2-17)
 1  1	OLD-FORMAT TAPE, CONTINUED FILE
 0  1	NEW-FORMAT TAPE (FILE # IN PAGNO BITS 2-17)
 1  0	NEW-FORMAT TAPE, CONTINUED FILE

A DUMPER TAPE IS A COLLECTION OF RECORDS ORGANIZED IN THE
FOLLOWING FASHION:


!=======================================================!
!            HEADER FOR FIRST SAVESET (TPHD)            !
!-------------------------------------------------------!
!          USER INFO (USR) OR FILE (SEE BELOW)          !
!-------------------------------------------------------!
!                   USER INFO OR FILE                   !
!-------------------------------------------------------!
!                           .                           !
!                           .                           !
!                           .                           !
!=======================================================!
!            HEADER FOR SECOND SAVESET (TPHD)           !
!-------------------------------------------------------!
!          USER INFO (USR) OR FILE (SEE BELOW)          !
!-------------------------------------------------------!
!                   USER INFO OR FILE                   !
!-------------------------------------------------------!
!                           .                           !
!                           .                           !
!                           .                           !
!=======================================================!
!                                                       !
!                  SUBSEQUENT SAVESETS                  !
!                                                       !
!=======================================================!
!                                                       !
!                     LAST SAVESET                      !
!                                                       !
!=======================================================!
!                  TAPE TRAILER (TPTR)                  !
!=======================================================!


NOTES:

1.  ON LABELED TAPES, THE TPTR RECORD APPEARS ONLY IF
    THE SAVESET IS CONTINUED ON ANOTHER TAPE.

2.  SOLITARY TAPE MARKS (EOF'S) ARE IGNORED ON INPUT.
    TWO CONSECUTIVE TAPE MARKS ARE INTERPRETED AS TPTR.

3.  ON LABELED TAPES, EACH SAVESET OCCUPIES EXACTLY ONE FILE.

4.  THE FIRST RECORD OF A CONTINUED SAVESET IS CTPH
    INSTEAD OF TPHD.

A DISK FILE SAVED ON A DUMPER TAPE ALWAYS HAS THIS
SEQUENCE OF RECORDS:

!=======================================================!
!                  FILE HEADER (FLHD)                   !
!-------------------------------------------------------!
!          DATA RECORD: 1 PAGE OF FILE (DATA)           !
!-------------------------------------------------------!
!          DATA RECORD: 1 PAGE OF FILE (DATA)           !
!-------------------------------------------------------!
!                           .                           !
!                           .                           !
!                           .                           !
!-------------------------------------------------------!
!                  FILE TRAILER (FLTR)                  !
!=======================================================!

#endif


				/* 5 bytes per 36-bit word */
				/* 518 word logical blocks */
#define TAPEBLK 518*5

				/* Checksum is first word */
#define WdoffChecksum      0
#define BtoffChecksum      0
#define BtlenChecksum     36
				/* Page access bits is second word */
#define WdoffAccess        1
#define BtoffAccess        0
#define BtlenAccess       36
				/* SCD, first 3 bits in next word */
#define WdoffSCD           2
#define BtoffSCD           0
#define BtlenSCD           3
				/* Number of saveset on tape */
#define WdoffSaveSetNum    2
#define BtoffSaveSetNum    3
#define BtlenSaveSetNum   15
				/* Tape number of dump */
#define WdoffTapeNum       2
#define BtoffTapeNum      18
#define BtlenTapeNum      18
				/* F1, F2 Flag bits */
#define WdoffF1F2          3
#define BtoffF1F2          0
#define BtlenF1F2          2
				/* File Number in Set (new format only) */
#define WdoffFileNum       3
#define BtoffFileNum       2
#define BtlenFileNum      16
				/* Page Number in file */
#define WdoffPageNum       3
#define BtoffPageNum      18
#define BtlenPageNum      18
				/* Record type (2's complement) */
#define WdoffRectype       4
#define BtoffRectype       0
#define BtlenRectype      36
				/* Record sequence number */
#define WdoffRecseq        5
#define BtoffRecseq        0
#define BtlenRecseq       36


				/* SCD Values */
#define SCDNormal       0
#define SCDCollection   1
#define SCDArchive      2
#define SCDMigration    3

				/* F1, F2 Values */
#define F1F2Old            0
#define F1F2OldContinue    3
#define F1F2New            1
#define F1F2NewContinue    2

				/* Record type values */
#define RectypeData     0
#define RectypeTphd     1
#define RectypeFlhd     2
#define RectypeFltr     3
#define RectypeTptr     4
#define RectypeUsr      5
#define RectypeCtph     6
#define RectypeFill     7

#define WdoffSSDate        8            /* Saveset date offset (type 1, 6) */
#define WdoffSSName        9            /* Saveset name offset (type 1, 6) */
#define WdoffFLName        6            /* Filename offset (type 2) */
#define WdoffFDB         134            /* FDB offset (type 2) */

					/* Number of bits per byte */
#define WdoffFDB_BSZ     011+WdoffFDB
#define BtoffFDB_BSZ       6
#define BtlenFDB_BSZ       6

					/* Number of pages in the file */
#define WdoffFDB_PGC     011+WdoffFDB
#define BtoffFDB_PGC      18
#define BtlenFDB_PGC      18

					/* Number of bytes in the file */
#define WdoffFDB_Size    012+WdoffFDB
#define BtoffFDB_Size      0
#define BtlenFDB_Size     36

					/* Date of last write to file */
#define WdoffFDB_Wrt     014+WdoffFDB
