/*
	newsrc states
*/
#define NEWS_ON ':'
#define NEWS_OFF '!'

#define SFLG_SCAN 1
#define SFLG_SPEC 2

#define FPFIX "Re: "
#define FPFLEN 4

#define FIL_AUTHOR 'w'
#define FIL_TITLE 't'

/*
	header lines and associated lengths.  Strings should
	actually be used only once.
*/ 
#define RHEAD "References: "
#define RHDLEN 12
#define MHEAD "Message-ID: "
#define MHDLEN 12
#define PHEAD "Path: "
#define PHDLEN 6
#define DHEAD "Date: "
#define DHDLEN 6
#define RTHEAD "Reply-To: "
#define RTHDLEN 10
#define TOHEAD "To: "
#define TOHDLEN 4
#define FHEAD "From: "
#define FHDLEN 6
#define FTHEAD "Followup-To: "
#define FTHDLEN 13
#define DISHEAD "Distribution: "
#define DISHDLEN 14
#define THEAD "Subject: "
#define THDLEN 9
#define LHEAD "Lines: "
#define LHDLEN 7
#define NHEAD "Newsgroups: "
#define NHDLEN 12

#define CHFIRST "FSL"	/* first char's of those used in page display */
