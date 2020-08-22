IMPLEMENTATION MODULE InOutExtensions ;

(* written by 		   *)
(*    Jesse M. Heines	   *)
(*    University of Lowell *)

(* Version 1.2     1/28/87 *)



FROM CharCodes IMPORT
   EscapeCh ;

FROM InOut IMPORT
   termCH, Read, Write, WriteInt ;

FROM MathLib0 IMPORT
   log10 ;

FROM SCTermStream IMPORT
   CreateInOutput ;

FROM Streams IMPORT
   Delete, Stream ;

FROM SYSTEM IMPORT
   MAXINT, MININT ;

FROM TextIO IMPORT
   EndOfLine, ReadCHAR ;



PROCEDURE GetEscapeSequence
   (VAR c : ARRAY OF CHAR ) ;   (* characters read from the terminal *)

(* This procedure is designed to read as escape sequnce from the terminal   *)
(* without waiting for the user to press RETURN.  It begins by reading a    *)
(* single from the terminal.  If the character read is ESCape, a second     *)
(* character is read.  If the second characters is '[', a third character   *)
(* is read.  Any characters not read are set to 0C.                         *)

VAR 
   SingleCharStream : Stream ;   (* terminal stream in single character mode *)

BEGIN

      (* Initialization *)

   c[0] := 0C ;
   c[1] := 0C ;
   c[2] := 0C ;

      (* Open input stream *)

   SingleCharStream := CreateInOutput () ;

      (* Read characters *)

   c[0] := ReadCHAR (SingleCharStream) ;
   IF c[0] = EscapeCh THEN
      c[1] := ReadCHAR (SingleCharStream) ;
      IF c[1] = '[' THEN
	 c[2] := ReadCHAR (SingleCharStream) ;
      END ;
   END ;

      (* Close input stream *)

   Delete (SingleCharStream) ;

END GetEscapeSequence ;



PROCEDURE GetOneChar
   (VAR c : CHAR ) ;   (* character read from the terminal *)

(* This procedure gets a single character from the terminal without *)
(* waiting for the user to press RETURN.                            *)

VAR 
   SingleCharStream : Stream ;   (* terminal stream in single character mode *)

BEGIN

   SingleCharStream := CreateInOutput () ;
   c := ReadCHAR (SingleCharStream) ;
   Delete (SingleCharStream) ;

END GetOneChar ;



PROCEDURE ReadLn ;

(* This procedure reads data from the current input stream until an end  *)
(* of line character (EOL), a null character (0C), or a CTRL/D character *)
(* (4C) is read.  The data read is not saved.  The purpose of this       *)
(* procedure is to skip the rest of the current line to prepare for      *)
(* reading the next line of input.                                       *)

BEGIN

      (* Read as long as character read is not a terminating condition *)

   WHILE (NOT (EndOfLine (termCH))) AND (termCH # 0C) AND (termCH # 4C) DO
      Read (termCH) ;
   END ;

END ReadLn ;



PROCEDURE ReadLine
   (VAR line : ARRAY OF CHAR) ;   (* the line read *)

(* This procedure reads a line of data from the current input stream.     *)
(* Reading is terminated when the end of line character (EOL) is reached, *)
(* the array into which characters are being read becomes full, a null    *)
(* character is read, or a CTRL/D character (4C) is read.                 *)

VAR 
   k       : CARDINAL ;   (* loop index *)
   linelen : CARDINAL ;   (* number of characters read thus far *)

BEGIN

      (* Initialization *)

   Read (termCH) ;
   linelen := 0 ;

      (* Read as long as character read is not a terminating condition *)

   WHILE (NOT (EndOfLine (termCH))) AND (linelen <= HIGH(line)) AND 
	 (termCH # 0C) AND (termCH # 4C) DO
      line[linelen] := termCH ;
      Read (termCH) ;
      INC (linelen) ;
   END ;

      (* Zero out the rest of the line *)

   FOR k := linelen TO HIGH(line) DO
      line[k] := 0C ;
   END ;

END ReadLine ;



PROCEDURE WriteFormattedReal
   (r              : REAL ;         (* the real number to write *)
    width          : CARDINAL ;     (* size of output field *)
    ndecimalplaces : CARDINAL ) ;   (* number of decimal places to write *)

(* This procedure writes a real number to the output stream and places that *)
(* number in a field "width" spaces wide.  The field will be filled with    *)
(* blanks to pad it to the appropriate size.  (The number is right justi-   *)
(* fied in the field.)  WriteFormattedReal will output numbers in standard  *)
(* format with the specified number of decimal places.                      *)

VAR
   k          : CARDINAL ;   (* loop index *)
   length     : CARDINAL ;   (* minimum field width needed to write number *)
   placevalue : REAL ;       (* value of place currently being printed *)
   placedigit : [0..9] ;     (* digit in place currently being printed *)

BEGIN

      (* Compute minimum number of spaces needed to write number *)

   IF r > 0.0 THEN
      IF r >= 1.0 THEN
	 length := TRUNC (log10 (r)) + 2 + ndecimalplaces ;
      ELSE
	 length := 2 + ndecimalplaces ;
      END ;
   ELSE
      IF r <= -1.0 THEN
	 length := TRUNC (log10 (ABS(r))) + 3 + ndecimalplaces ;
      ELSE
	 length := 3 + ndecimalplaces ;
      END ;
   END ;

      (* Output padding spaces if necessary *)

   FOR k := length + 1 TO width DO
      Write (' ') ;
   END ;

      (* Write integer part *)
   
   IF (r >= FLOAT (MININT)) AND (r <= FLOAT (MAXINT)) THEN
      WriteInt (TRUNC(r), 0) ;

   ELSE
      IF r < 0.0 THEN
	 Write ('-') ;
	 r := ABS (r) ;
      END ;

      placevalue := Power (10.0,TRUNC(log10(r))) ;

      FOR k := TRUNC(log10(r)) TO 0 BY -1 DO
	 placedigit := TRUNC (r/placevalue) ;
	 Write (CHR (48 + placedigit)) ;
	 r := r - placevalue * FLOAT(placedigit) ;
	 placevalue := placevalue / 10.0 ;
      END ;

   END ;

      (* Write decimal point *)
   
   Write ('.') ;

      (* Write fractional part *)

   r := r - FLOAT(TRUNC(r)) ;

   FOR k := 1 TO ndecimalplaces DO
      r := 10.0 * r ;
      IF k < ndecimalplaces THEN
	 WriteInt (TRUNC(r), 0) ;
      ELSE
	 WriteInt (RoundDigit(r), 0) ;
      END ;
      r := r - FLOAT(TRUNC(r)) ;
   END ;

END WriteFormattedReal ;



PROCEDURE Power
   (n : REAL ;       (* number to compute the Power of *)
    x : CARDINAL )   (* Power to raise number to *)
   : REAL ;          (* result type *)

(* This procedure computes n to the x and returns the result as a real *)
(* number.  It is not exported.                                        *)

VAR
   k    : CARDINAL ;   (* loop index *)
   temp : REAL ;       (* intermediate result *)

BEGIN
   temp := 1.0 ;
   FOR k := 1 TO x DO
      temp := n * temp ;
   END ;
   RETURN temp ;
END Power ;



PROCEDURE RoundDigit
   (n : REAL )   (* real number to round *)
   : INTEGER ;   (* return type *)

(* This procedure rounds a real number to the nearest 1 and returns the *)
(* resultant value as an integer.  It is not exported.                  *)

BEGIN
   IF ABS (n - FLOAT(TRUNC(n))) < 0.5 THEN
      RETURN TRUNC(n) ;
   ELSE
      IF n > 0.0 THEN
	 RETURN TRUNC(n) + 1 ;
      ELSE
	 RETURN TRUNC(n) - 1 ;
      END ;
   END ;
END RoundDigit ;



END InOutExtensions.
