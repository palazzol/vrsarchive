        int comment;            /* > 0 iff in comment body      */
        char **arg;             /* Current file name            */
        char ident[12];         /* Current function name        */
        int inbody;             /* > 0 iff in fuction body      */
        int line;               /* Current Line                 */
        char *nullstr = "";     /* Dummy file name              */
        int page;               /* Current Page                 */
%%
\{                      inbody++;
\}                      inbody--;
\n                      if (++line > 56) { line = 0; page++; };
\f                      line = 0; page++;
.                       ;
\/\*                    comment = 1;
\*\/                    comment = 0;
\,.*\(                  ;
\+.*\(                  ;
\-.*\(                  ;
\(r[0-9]*\)             ;
\(fp\)                  ;
\(sp\)                  ;
\(pc\)                  ;
\(.*\%.*\)              ;
\([^)%]*\)              { if ((!comment) && (!inbody) && (ident[0]))
                            printf("%-10s %-14s %4d\n",ident,*arg,page);
                        }
\([^)]*\)[^-,;\n]*\+    ;
\([^)]*\)[^+,;\n]*\-    ;
\([^)]*\)[^-+;\n]*\,    yyless(yyleng-1);
\([^)]*\)[^-,+\n]*\;    ;
^#.*                    ;
[_A-Za-z][_A-Za-z0-9]*  { yytext[10] = '\0';
                          strcpy(ident,yytext);
                        }
%%

parseone()
{
  comment = 0;          /* Not in comment body          */
  inbody = 0;           /* Not in fuction body          */
  line = 1;             /* Current Line                 */
  page = 1;             /* Current Page                 */
  ident[0] = '\0';
  yylex();
}

main(argc,argv)
int argc;
char *argv[];
{ int piped[2];

#ifdef PR
  pipe(piped);
  if (fork() != 0) {
    close(0);
    dup(piped[0]);
    close(piped[0]);
    close(piped[1]);
    execlp(PR,PR,"-2","-h","Index",(char *)NULL);
  }
  close(1);
  dup(piped[1]);
  close(piped[0]);
  close(piped[1]);
#endif
  pipe(piped);
  if (fork() != 0) {
    close(0);
    dup(piped[0]);
    close(piped[0]);
    close(piped[1]);
    execlp("sort","sort",(char *)NULL);
  }
  close(1);
  dup(piped[1]);
  close(piped[0]);
  close(piped[1]);
  if (argc > 1) {
    arg = &argv[1];
    while (argc > 1) {
      freopen(*arg,"r",stdin);
      parseone();
      argc--;
      arg++;
    }
  } else {
    arg = &nullstr;
    parseone();
  }
}
