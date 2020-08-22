/*********************************************
 *             GIFtoGS Converter             *
 *                                           *
 *      May 16, 1988  by Scott Hemphill      *
 *      May 16, 1988  by Vincent Slyngstad   *
 *                                           *
 * I wrote this program, and hereby place it *
 * in the public domain, i.e. there are no   *
 * copying restrictions of any kind.         *
 *********************************************/

#include <stdio.h>
char *malloc();
int strncmp();

#define min(x,y) ((x) < (y) ? (x) : (y))
#define FALSE 0
#define TRUE 1

typedef int bool;
typedef struct codestruct {
            struct codestruct *prefix;
            unsigned char first,suffix;
        } codetype;

FILE *infile;
unsigned int screenwidth;           /* The dimensions of the screen */
unsigned int screenheight;          /*   (not those of the image)   */
bool global;                        /* Is there a global color map? */
int globalbits;                     /* Number of bits of global colors */
unsigned char globalmap[256][3];    /* RGB values for global color map */
char colortable[256];            /* Hex intensity strings for an image */
unsigned char *raster;              /* Decoded image data */
codetype *codetable;                /* LZW compression code data */
int datasize,codesize,codemask;     /* Decoder working variables */
int clear,eoi;                      /* Special code values */

void usage()
{
        fprintf(stderr,"usage: giftogs input-file > output-file\n");
        exit(-1);
}

void fatal(s)
char *s;
{
        fprintf(stderr,"giftogs: %s\n",s);
        exit(-1);
}

void checksignature()
{
        char buf[6];

        fread(buf,1,6,infile);
        if (strncmp(buf,"GIF",3)) fatal("file is not a GIF file");
        if (strncmp(&buf[3],"87a",3)) fatal("unknown GIF version number");
}

/* Get information which is global to all the images stored in the file */

void readscreen()
{
        unsigned char buf[7];

        fread(buf,1,7,infile);
        screenwidth = buf[0] + (buf[1] << 8);
        screenheight = buf[2] + (buf[3] << 8);
        global = buf[4] & 0x80;
        if (global) {
            globalbits = (buf[4] & 0x07) + 1;
            fread(globalmap,3,1<<globalbits,infile);
        }
}

/* Convert a color map (local or global) to an array of two character
   hexadecimal strings, stored in colortable.  RGB is converted to
   8-bit grayscale using integer arithmetic. */

void initcolors(colortable,colormap,ncolors)
char colortable[256];
unsigned char colormap[256][3];
int ncolors;
{
        register unsigned color;
        register i;

        for (i = 0; i < ncolors; i++) {
            color = 77*colormap[i][0] + 150*colormap[i][1] + 29*colormap[i][2];
            color /= (77+150+29);
            colortable[i] = color;
        }
}

/* Output the bytes associated with a code to the raster array */
void outcode(p,fill)
register codetype *p;
register unsigned char **fill;
{
        if (p->prefix) outcode(p->prefix,fill);
        *(*fill)++ = p->suffix;
}

/* Process a compression code.  "clear" resets the code table.  Otherwise
   make a new code table entry, and output the bytes associated with the
   code. */

void process(code,fill)
register code;
unsigned char **fill;
{
        static avail,oldcode;
        register codetype *p;

        if (code == clear) {
            codesize = datasize + 1;
            codemask = (1 << codesize) - 1;
            avail = clear + 2;
            oldcode = -1;
        } else if (code < avail) {
            outcode(&codetable[code],fill);
            if (oldcode != -1) {
                p = &codetable[avail++];
                p->prefix = &codetable[oldcode];
                p->first = p->prefix->first;
                p->suffix = codetable[code].first;
                if ((avail & codemask) == 0 && avail < 4096) {
                    codesize++;
                    codemask += avail;
                }
            }
            oldcode = code;
        } else if (code == avail && oldcode != -1) {
            p = &codetable[avail++];
            p->prefix = &codetable[oldcode];
            p->first = p->prefix->first;
            p->suffix = p->first;
            outcode(p,fill);
            if ((avail & codemask) == 0 && avail < 4096) {
                codesize++;
                codemask += avail;
            }
            oldcode = code;
        } else {
            fatal("illegal code in raster data");
        }
}

/* Decode a raster image */

void readraster(width,height)
unsigned width,height;
{
        unsigned char *fill = raster;
        unsigned char buf[255];
        register bits=0;
        register unsigned count,datum=0;
        register unsigned char *ch;
        register int code;

        datasize = getc(infile);
        clear = 1 << datasize;
        eoi = clear+1;
        codesize = datasize + 1;
        codemask = (1 << codesize) - 1;
        codetable = (codetype*)malloc(4096*sizeof(codetype));
        if (!codetable) fatal("not enough memory for code table");
        for (code = 0; code < clear; code++) {
            codetable[code].prefix = (codetype*)0;
            codetable[code].first = code;
            codetable[code].suffix = code;
        }
        for (count = getc(infile); count > 0; count = getc(infile)) {
            fread(buf,1,count,infile);
            for (ch=buf; count-- > 0; ch++) {
                datum += *ch << bits;
                bits += 8;
                while (bits >= codesize) {
                    code = datum & codemask;
                    datum >>= codesize;
                    bits -= codesize;
                    if (code == eoi) goto exitloop;  /* This kludge put in
                                                        because some GIF files
                                                        aren't standard */
                    process(code,&fill);
                }
            }
        }
exitloop:
        if (fill != raster + width*height)
			fprintf(stderr,"giftogs: raster has the wrong size\n");
        free(codetable);
}

/* Read a row out of the raster image and write it to the output file */

void rasterize(row,width)
int row,width;
{
        register unsigned char *scanline;
        register i;

        scanline = raster + row*width;
        for (i = 0; i < width; i++) {
            putchar(colortable[*scanline++]);
        }
}


/* Read image information (position, size, local color map, etc.) and convert
   to greyscale. */

void readimage()
{
        unsigned char buf[9];
        unsigned left,top,width,height;
        bool local,interleaved;
        char localmap[256][3];
        int localbits;
        int *interleavetable;
        register row;
        register i;

        fread(buf,1,9,infile);
        left = buf[0] + (buf[1] << 8);
        top = buf[2] + (buf[3] << 8);
		left = top = 0;	/* Don't worry about the screen offset */
        width = buf[4] + (buf[5] << 8);
        height = buf[6] + (buf[7] << 8);
        local = buf[8] & 0x80;
        interleaved = buf[8] & 0x40;
        if (local) {
            localbits = (buf[8] & 0x7) + 1;
            fread(localmap,3,1<<localbits,infile);
            initcolors(colortable,localmap,1<<localbits);
        } else if (global) {
            initcolors(colortable,globalmap,1<<globalbits);
        } else {
            fatal("no colormap present for image");
        }
        printf("%d %d\n", width, height);
        raster = (unsigned char*)malloc(width*height);
        if (!raster) fatal("not enough memory for image");
        readraster(width,height);
        if (interleaved) {
            interleavetable = (int*)malloc(height*sizeof(int));
            if (!interleavetable) fatal("not enough memory for interleave table");
            row = 0;
            for (i = top; i < top+height; i += 8) interleavetable[i] = row++;
            for (i = top+4; i < top+height; i += 8) interleavetable[i] = row++;
            for (i = top+2; i < top+height; i += 4) interleavetable[i] = row++;
            for (i = top+1; i < top+height; i += 2) interleavetable[i] = row++;
            for (row = top; row < top+height; row++) rasterize(interleavetable[row],width);
            free(interleavetable);
        } else {
            for (row = top; row < top+height; row++) rasterize(row,width);
        }
        free(raster);
}

/* Read a GIF extension block (and do nothing with it). */

void readextension()
{
        unsigned char code,count;
        char buf[255];

        code = getc(infile);
        while (count = getc(infile)) fread(buf,1,count,infile);
}

main(argc,argv)
int argc;
char *argv[];
{
        int quit = FALSE;
        char ch;

        if (argc != 2) usage();
        infile = fopen(argv[1],"r");
        if (!infile) {
            perror("giftogs");
            exit(-1);
        }
        checksignature();
        readscreen();
        do {
            ch = getc(infile);
            switch (ch) {
                case '\0':  break;  /* this kludge for non-standard files */
                case ',':   readimage();
                            break;
                case ';':   quit = TRUE;
                            break;
                case '!':   readextension();
                            break;
                default:    fatal("illegal GIF block type");
                            break;
            }
        } while (!quit);
		return(0);
}
