/***                    curses.h                        ***/

     /*** Include file for seperate compilation.  ***/

#define OFF             0
#define ON              1

int  InitScreen(),      /* This must be called before anything else!! */

     ClearScreen(),      CleartoEOLN(),

     MoveCursor(),
     CursorUp(),         CursorDown(),
     CursorLeft(),       CursorRight(),

     StartBold(),        EndBold(),
     StartUnderline(),   EndUnderline(),
     StartHalfbright(),  EndHalfbright(),
     StartInverse(),     EndInverse(),
     
     transmit_functions(),

     Raw(),              ReadCh();
#ifdef __STDC__
void PutLine(int x, int y, char *line, ...);
#else
int  PutLine();
#endif

char *return_value_of();
