#define USG
/* #define BSD4_x */

/*				NOTE
   Need to define the system being used.  Currently defined for Bell (USG).  To
   use on BSD remove the "#define USG" and the comment delimiters for 
   "define BSD4_x".
*/



/* In order to perform random page output the input file must be read
   sequentially.  Various text characteristics change and are not refreshed
   with each new page.  To guarantee the correct text qualities all input
   needs to be processed and certain qualities transmitted to the display
   device.
*/

/*
output language from ditroff:
all numbers are character strings

sn      size in points
fn      font as number from 1-n
cx      ascii character x
Cxyz    funny char xyz. terminated by white space
Hn      go to absolute horizontal position n
Vn      go to absolute vertical position n (down is positive)
hn      go n units horizontally (relative)
vn      ditto vertically
nnc     move right nn, then print c (exactly 2 digits!)
                (this wart is an optimization that shrinks output file size
                 about 35% and run-time about 15% while preserving ascii-ness)
Dt ...\n        draw operation 't':
        Dl x y          line from here by x,y
        Dc d            circle of diameter d with left side here
        De x y          ellipse of axes x,y with left side here
        Da x y r        arc counter-clockwise by x,y of radius r
        D~ x y x y ...  wiggly line by x,y then x,y ...
nb a    end of line (information only -- no action needed)
        b = space before line, a = after
w       paddable word space (information only -- no action needed)
p       new page begins -- set v to 0
#...\n  comment
x ...\n device control functions:
        x i     init
        x T s   name of device is s
        x r n h v       resolution is n/inch
                h = min horizontal motion, v = min vert
        x p     pause (can restart)
        x s     stop -- done for ever
        x t     generate trailer
        x f n s font position n contains font s
        x H n   set character height to n
        x S n   set slant to N

        Subcommands like "i" are often spelled out like "init".
*/

#define TOP 3132
#define ESC 27
#define NULL 0
#define ROUNDING 0
#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>

#ifdef BSD4_x
#include <sgtty.h>
struct sgttyb ottyb, nttyb;
#endif

#ifdef USG
#include <sys/termio.h>
#ifndef TCGETA
#include <sys/ioctl.h>
#endif /*!TCGETA*/
struct  termio old;
struct  termio noret;
#endif

FILE *fp;                /* input file pointer */
int  c;
char *s;
char *filename[30];
char page_runner;       /* true if to not display characters; need to get to
                           another page different than the current page */
char first_page;
char quit;
char ignore_cr;
int i;
int currentx, currenty;
int absx, absy;
int point_size, raster_size, raster_height;
int char_width;
int char_space;
int resolution;
int current_page, next_page;
float rasters_per_point;
jmp_buf env;
int visibility[8];

SIG_T
catchsig (dummy)
{
        signal (SIGINT, SIG_IGN);
        quit = TRUE;
        longjmp (env, 1);
}

main(argc,argv)
int argc;
char *argv[];
{
        /* initialize */
        signal (SIGHUP, catchsig);
        signal (SIGINT, catchsig);
        signal (SIGQUIT, catchsig);
        signal (SIGTERM, catchsig);
        next_page = 1;
        currentx = 0;
        currenty = 0;
        rasters_per_point = .6591;  /* set initially for a phototypesetter */
        page_runner = FALSE;
        first_page = TRUE;

        if (argc < 2) {
                printf("dithds [-p#] <ditroff.out>\n");
                exit(1);
        }

        /* read all the options */
        while (argc > 1 && argv[1][0] == '-'){
                switch (argv[1][1]) {
                case 'p':
                        next_page = atoi(&argv[1][2]);
                        page_runner = TRUE;
                        break;
                default:
                        printf("Illegal option %c\n",*s);
                        break;
                }
                argc--;
                argv++;
        }

        if ((fp = fopen(*++argv,"r")) == NULL)
        {
                printf("Can't open %s\n", *argv);
                exit(1);
        }
        *filename = *argv;

        /* initialize the terminal */
        /* put in tek mode */
        select_code(0);
        /* turn off the dialog area */
        dialog_visib(0);
        fixup_level(2);  /* set the fixup level to 2.  This allows for quick
                                deletion of segments.  */
        /* set coordinate mode to 12 bit format on 4115's 
           gives an error on other terminals.  Error level of 0 */
        coor_mode(0,0);
        /* set the window */
        set_window(0,0,4095,TOP);
        page();
        /* set the graphtext precision to stroke */
        tx_prec(2);

#ifdef BSD4_x
        ioctl(0,TIOCGETP,(char *) &ottyb);
        nttyb = ottyb;
        nttyb.sg_flags |= CBREAK;
        ioctl(0,TIOCSETP,(char *) &nttyb);
#endif

#ifdef USG
        ioctl(0, TCGETA, &old);      /* set for reading characters from tty */
        ioctl(0, TCGETA, &noret);
        noret.c_cc[VMIN] = 1;
        noret.c_cc[VTIME] = 0;
        noret.c_lflag &= ~(ICANON|ECHO);
        ioctl(0, TCSETA, &noret);
#endif

        quit = FALSE;
        c = getc(fp);
        if (c == EOF) quit = TRUE;
        setjmp (env);
        while (quit == FALSE) {
                switch(c) {

                case 'p':
                        current_page = ctoi();
                        ignore_cr = FALSE;
                        if (current_page != next_page)
                                break;
                        if (page_runner == FALSE) {
                                /* close off all open segments for next page */
                                if (first_page == TRUE) {
                                        next_page = next_page + 1;
                                        del_segment(-1);
                                        page();
                                        first_page = FALSE;
                                }
                                else {
                                        end_segment();
                                        printf("%c",7); /* signal end of page */
                                        fflush(stdout);
                                        /*readch();*/
                                        readresponse();
                                        del_segment(-1);
                                        page();
                                        if (next_page < current_page) {
                                                fclose(fp);
                                                fp = fopen(*argv,"r");
                                                page_runner = TRUE;
                                                break;
                                        }
                                        else if (next_page > current_page) {
                                                page_runner = TRUE;
                                                break;
                                        }
                                        else next_page = current_page +1;
                                        if (c == EOF) {
                                                next_page = 1;
                                                fclose(fp);
                                                fp = fopen(*argv,"r");
                                                del_segment(-1);
                                                page();
                                                page_runner = TRUE;
                                                break;
                                        }
                                }
                        }
                        else {   /* page_runner == TRUE */
                                first_page = FALSE;
                                page_runner = FALSE;
                                next_page = next_page + 1;
                        }
                        /* don't clear screen: leave dialog area alone */
                        /* open retained segment 1 */
                        begin_seg(1);
                        /* draw a line down the right margin */
                        move(2420,0);
                        draw(2420,TOP);
                        move(currentx,currenty);
                        break;

                case 's':
                        /* point size */

                        /*  The font size is determined only by the terminal
                        being used.  The device formatted for does not enter
                        in.   A simple conversion is all that is needed.
                        72(points/inch) * 11(inches/page) = 792(points/page)
                        3132(rasters/page) * 1/792(page/points) =
                                                        3.95(rasters/point)
                                point size 1 = raster size 3.95
                        */

                        point_size = ctoi();
                        raster_size = point_size * 3.95 + ROUNDING;
                        char_width = raster_size * .4 + ROUNDING;  /* it looks good */
                        raster_height = .75 * raster_size;
                        char_space = 1.2 * raster_size + ROUNDING; /* close to default */
                        tx_size(char_width,raster_height,char_space);
                        break;

                case 'f':       /* font change */
                        ignore_cr = FALSE;
                        tx_font(ctoi());
                        break;

                case 'h':       /* relative horizontal move */
                        absx = absx + ctoi();
                        if (page_runner == TRUE)
                                break;
                        currentx = absx * rasters_per_point + ROUNDING;
                        move(currentx,currenty);
                        break;

                case 'v':       /* relative move vertical */
                        absy = absy + ctoi();
                        if (page_runner == TRUE)
                                break;
                        currenty = TOP - absy * rasters_per_point + ROUNDING;
                        move(currentx,currenty);
                        break;

                case 'H':
                        /* absolute horizontal move */
                        ignore_cr = FALSE;
                        absx = ctoi();
                        if (page_runner == TRUE)
                                break;
                        currentx = absx * rasters_per_point + ROUNDING;
                        move (currentx,currenty);
                        break;

                case 'V':
                        /* absolute vertical */
                        ignore_cr = FALSE;
                        absy = ctoi();
                        if (page_runner == TRUE)
                                break;
                        currenty = TOP - absy * rasters_per_point + ROUNDING;
                        move(currentx,currenty);
                        break;

                case 'c':
                        /* character */
                        /* display next character */
                        c = getc(fp);
                        if (page_runner == TRUE)
                                break;
                        printf("\037%c",c);
                        break;

                case 'C':
                        /* special character */
                        if (page_runner == TRUE){
                                c = getc(fp);
                                c = getc(fp);
                                break;
                        }
                        c = getc(fp);
                        if (c == '\n') c = getc(fp);  /* remove newlines */
                        switch(c){
                        case 'r':
                                c = getc(fp);
                                if (c == '\n') c = getc(fp);  /* remove newlines */
                                if (c == 'u')  /* underline */
                                        printf("\037_");
                                break;
                        case 'F':       /* fancy ff's */
                                printf("\037f");
                                currentx = currentx + char_space * .2 + ROUNDING;
                                move(currentx,currenty);
                                printf("\037f");
                                c = getc(fp);
                                if (c == '\n') c = getc(fp);  /* remove newlines */
                                currentx = currentx + char_space * .2 + ROUNDING;
                                move(currentx,currenty);
                                printf("\037%c",c);
                                break;
                        case 'f':       /* fancy f's */
                                printf("\037f");
                                currentx = currentx + char_space * .2 + ROUNDING;
                                move(currentx,currenty);
                                c = getc(fp);
                                if (c == '\n') c = getc(fp);  /* remove newlines */
                                printf("\037%c",c);
                                break;
                        case 'h':       /* hypen */
                                c = getc(fp);
                                if (c == '\n') c = getc(fp);  /* remove newlines */
                                c = '-';
                                printf("\037%c",c);
                                break;
                        case 'd':       /* Cde is a degree sign */
                                currenty = currenty + raster_height * .4;
                                currentx = currentx + char_width * .5;
                                move(currentx,currenty);
                                c = getc(fp);
                                if (c == '\n') c = getc(fp);
                                    /* remove newlines */
                                c = 'o';
                                printf("\037%c",c);
                                currenty = currenty - raster_height * .4;
                                currentx = currentx - char_width * .5;
                                move(currentx,currenty);
                                break;
                                
                        default:
                                while ((c = getc(fp)) != '\n' );
                                break;
                        }
                        break;

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                        /* horizontal relative displacement */
                        ignore_cr = TRUE;
                        while ((c >= '0') && (c <= '9')){
                                absx = absx + (c - '0') * 10;
                                c = getc(fp);
                                if (c == '\n') c = getc(fp);
                                   /* remove any newlines */
                                absx = absx + (c - '0');
                                c = getc(fp);
                                if (c == '\n') c = getc(fp);
                                   /* remove any newlines */
                                if (page_runner != TRUE) {
                                        currentx = absx * rasters_per_point + ROUNDING;
                                        move(currentx,currenty);
                                        printf("\037%c",c);
                                }
                        c = getc(fp);
                        if (c == 'w') c = getc(fp);  /* ignore 'w' */
                        } /* while */
                        ungetc(c,fp);
                        break;

                case 'x':
                        c=getc(fp); /* this should be a blank */
                        switch(getc(fp)){
                        case 'r':  /* set the resolution translation
                                                                                factor */
                                /* there are 3 numbers on this line.  they
                                   are (1) resolution in points per inch
                                       (2) minimum horizontal motion
                                       (3) minimum vertical motion
                                   Only concerned with the resolution in points
                                   per inch.  Call the term "resolution".

                                   How to determine the mapping from ANY device
                                   to the Tek terminal.

                resolution(points/inch) * 11(inches/page) = (points/page)

                3132(rasters/page) * (page/points) = (rasters/point)

                motion of 1 point equals X rasters on the Tek.

                                   */

                                while(isalpha(c=getc(fp)));
                                resolution = ctoi();
                                rasters_per_point = 3132.0 / (11.0 * resolution);
                                while ((c = getc(fp)) != '\n' );
                                /* ignore the rest of line */
                                break;

                        default:
                                /* ignore all other device control for now */
                                while ((c = getc(fp)) != '\n' );
                                break;
                        } /* switch */
                        break;

                case 'D':
                        if (page_runner == TRUE)
                                break;
                        switch(getc(fp)){
                        case 'l':
                                /* draw line from current to x,y */
                                move(currentx,currenty);
                                absx = absx + ctoi();
                                currentx = absx * rasters_per_point + ROUNDING;
                                absy = absy + ctoi();
                                currenty = TOP - absy * rasters_per_point;
                                draw(currentx,currenty) /* I001 + ROUNDING*/;
                                while ((c = getc(fp)) != '\n');
                                break;

                        default:
                                break;
                        }       /* switch */

                        break;

                case 'n':
                        ignore_cr = FALSE;
                        ctoi();         /* remove b */
                        ctoi();         /* remove a */
                        /* while (( c = getc(fp)) != '\n');  
                                                /* ignore rest of line */
                        break;

                default:
                        break;

                } /* switch */
                c = getc(fp);
                if ((c == EOF) && (page_runner == TRUE)) {
                        next_page = 1;
                        fclose(fp);
                        fp = fopen(*argv,"r");
                        del_segment(-1);
                        page();
                        page_runner = TRUE;
                }
                if ((c == EOF) && (page_runner == FALSE)) {
                        end_segment();  /* close out segments */
                        printf("%c",7); /* signal at the end with a bell */
                        fflush(stdout);
                        quit = TRUE;
                        readresponse();
                        if (quit == FALSE) {
                                fclose(fp);
                                fp = fopen(*argv,"r");
                                del_segment(-1);
                                page_runner = TRUE;
                                page();
                                c = getc(fp);
                        }
                }
        }

        /* set terminal back */
        visibility[0] = 1;
        visibility[1] = 1;      /* turn graphics plane on to gauranttee not
                                        killed in help area */
        surf_visib(2,visibility);
        del_segment(-1);        /* erase retained segments */
        page();
        fixup_level(6);         /* set fixup level back to default */
        dialog_visib(1);        /* turn the dialog area on*/
        clear_dialog();
        select_code(1);         /* put in ansi mode */
#ifdef BSD4_x
        ioctl(0,TIOCSETP,(char *) &ottyb);
#endif
#ifdef USG
        ioctl(0, TCSETAW, &old);         /* set tty back */
#endif
} /* tk */


ctoi()
{
        int n , sign;
        char s;

        s = ' ';
        while (s == ' ' || s == '\n' || s == '\t')
                s = getc(fp);   /* skip white space */
        sign = 1;
        if (s == '+' || s == '-') {     /* sign */
                sign = (s == '+') ? 1 : -1;
                s = getc(fp);
        }
        n = 0;
        while (s >= '0' && s <= '9'){
                n = 10 * n + s - '0';
                s = getc(fp);
                if ((s == '\n') && (ignore_cr == TRUE)) s = getc(fp);
        }
        ungetc(s,fp);
        return(sign * n);
}


readresponse()
{
        int delta_page;
        char s;

        read(0, &s, 1);
        while(s != '\n')
                switch(s){
                case '0': 
                case '1': 
                case '2': 
                case '3': 
                case '4':
                case '5': 
                case '6': 
                case '7': 
                case '8': 
                case '9':
                        next_page = 0;
                        while (s >= '0' && s <= '9'){
                                next_page = 10 * next_page + s - '0';
                                read(0, &s, 1);
                        }
                        s = '\n';
                        quit = FALSE;
                        break;
                case '+':
                        delta_page = 0;
                        s = getchar();
                        while (s >= '0' && s <= '9'){
                                delta_page = 10 * delta_page + s - '0';
                                read(0, &s, 1);
                        }
                        if (delta_page == 0) delta_page = 1;
                        next_page = next_page + delta_page - 1;
                        s = '\n';
                        quit = FALSE;
                        break;
                case '-':
                        delta_page = 0;
                        read(0, &s, 1);
                        while (s >= '0' && s <= '9'){
                                delta_page = 10 * delta_page + s - '0';
                                read(0, &s, 1);
                        }
                        if (delta_page == 0) delta_page = 1;
                        next_page = next_page - delta_page - 1;
                        s = '\n';
                        quit = FALSE;
                        break;
                case 'H':       /* provide help */
                case 'h':
                case '?':
                        visibility[0] = 1;
                        visibility[1] = 0;      /* turn graphics plane off */
                        surf_visib(2,visibility);
                        dialog_visib(1);        /* turn on the dialog area */
                        clear_dialog();
                        print_help();
                        read(0, &s, 1);
                        /* turn off the dialog area */
                        dialog_visib(0);        /* turn off the dialog area */
                        visibility[0] = 1;
                        visibility[1] = 1;      /* turn graphics plane on */
                        surf_visib(2,visibility);
                        read(0, &s, 1);
                        break;
                case 'k':
                case 'K':       /* who uses k to quit? */
                case 'q': 
                case 'Q':
                        quit = TRUE;
                        s = '\n';
                        break;
                case ' ':
                case '\n':
                        s = '\n';
                        break;
                default:
                        read(0, &s, 1);
                        break;
                } /* switch */
} /* readresponse */

print_help(){
        printf("\n\n\n\n\n\n    The options available are:\n\n\n");
        printf("\tcarriage return,\n");
        printf("\tspace         to go to next page\n");
        printf("\t#CR           absolute page to go to, # an integer, CR - carriage return\n");
        printf("\t+#CR,-#CR     relative page to go to, # an integer, CR - carriage return\n");
        printf("\th,H,?         for this page\n");
        printf("\tq,Q,k,K       to quit\n");
        printf("\n\tCarriage Return and space perform the same function\n");
        printf("\tBackspacing does not work.\n");
        printf("\n\n\tAny key to continue: ");
        }

readch(){
        char c;
        if (read(2,&c,1)<1) c = 0;
        return(c);
}
