(**************************************************************************)
(* This program simulates a graph environment for LaTeX. Using a list of  *)
(* parameters, it generates LaTeX commands to produce a complete graph.   *)
(* see the latexgraph.doc file for detailed documentation.                *)
(* It is meant only for POSITIVE GRAPHS. 				  *)
(* author: Sunil Podar (podar@sbcs.csnet)				  *)
(* last update: Apr 14, 1985						  *)
(**************************************************************************)

program main (input,output);
type    wholeline =  packed array[1..79] of char;
        tenchar   =  packed array[1..10] of char;
	twochar   =  packed array[1..2] of char;
var
	error1 :boolean;
	piclength,picheight,position,Xdeltabar,
	   Ydeltabar,i,Xlegloc,Ylegloc,Xdeltanum,Ydeltanum : integer;
	Xaxisstrg,Yaxisstrg,commandstr,capstring,blank79,
	   labelname,fontname : wholeline;
	plotchar1,plotchar2,plotchar3,plotchar4,plotchar5 : wholeline;
  	legendloc,wantXY,captiontwo,prepost : twochar;
	legendXcord,legendYcord : integer;
	Xscalegraph,Yscalegraph,Xtimes,Ytimes : integer;
	string,blank10,captiontype,fignumber : tenchar;
	unitlngth,textwidth : real;
	Xorig,Yorig : integer;
	Xscalereal,Yscalereal,xreality, yreality, xgraph, ygraph,
	Xdeln, Ydeln, Xnum, Ynum : real;
	plotcharnum : integer;
	itsreal : boolean;

procedure strreadline;
var charac:char;
    i : integer;
    endoffile, endofline:boolean;
begin
   i:=1;
   endoffile :=false;
   endofline:=false;
   commandstr:=blank79;
   repeat
   begin
   read(charac);write(charac);
   commandstr[i] := charac;
   i := i+1;
   if eof then endoffile :=true
   		  else if eoln then endofline :=true;
   end
   until (endoffile or endofline);
   if not endoffile then begin
                         readln;writeln;
			 end;
   
end; (*strreadline*)

procedure strreadword;
var charac:char;
    i : integer;
begin
   i:=1;
   string:=blank10;
   read(charac);write(charac);
   repeat
   begin
   string[i] := charac;
   read(charac);write(charac);
   i := i+1
   end
   until ((charac = '/') or (charac = ' ') or (i > 10) or eoln);
   (* so I'm reading the / without assigning it to string, neat *)
   if (eoln) then string[i]:= charac
   (* a kluge, to capture the last charac when using this procedure to read *)
   (* the argument. normally I use it only for parameters. *)
end; (*strreadword*)

procedure legendread;

begin
   legendloc := '  ';
   read(legendloc[1]);write(legendloc[1]);
   read(legendloc[2]);write(legendloc[2]);
   if legendloc = 'ex' then begin
                            readln(legendXcord,legendYcord);
			    writeln(' ',legendXcord:1,' ',legendYcord:1)
			    end
                       else begin
                            readln;writeln
			    end
   (* take care of the 'no', 'tr' & 'tl' options later *)
end; (* legendread *)
   

begin(* main *)
   for i := 1 to 10 do blank10[i] := ' ';
   for i := 1 to 79 do blank79[i] := ' ';
   captiontype := blank10;
   fignumber := blank10;
   fontname  := blank79;
   labelname := blank79;
 
   strreadword;
   while string <> '%%%%%%%%%%' do
   begin
    if string =  '%pre&post?'  then begin
				    readln(prepost[1],prepost[2]);
				    writeln(prepost[1],prepost[2])
				    end
    else if string =  '%unitlngth'  then  begin
					  readln(unitlngth);
				   	  writeln(' ',unitlngth:3:2)
				   	  end
    else if string =  '%font-name'  then  begin
    					    strreadline;
					    fontname:=commandstr
					  end
    else if string =  '%piclength'  then  begin
					  readln(piclength);
				   	  writeln(' ',piclength:1)
				   	  end
    else if string =  '%picheight'  then  begin
                                          readln(picheight);
					  writeln(' ',picheight:1)
					  end
    else if string =  '%??caption' 
                           then begin
			   	strreadword;readln;
				captiontype:=string;
			        writeln
			        end
    else if string =  '%fignumber'  then  begin
					  strreadword;readln;
					  fignumber:=string;
					  writeln
					  end
    else if string =  '%Xaxisstrg'  then  begin
    					    strreadline;
					    Xaxisstrg:=commandstr
					  end
    else if string =  '%Yaxisstrg'  then  begin
					    strreadline;
					    Yaxisstrg:=commandstr
					  end
    else if string =  '%capstring'  then  begin
					    strreadline;
					    capstring:=commandstr
					  end
    else if string =  '%labelname'  then  begin
					    strreadline;
					    labelname:=commandstr
					  end
    else if string =  '%legendloc'  then  legendread
    else if string =  '%Xdeltabar'  then  begin
					  readln(Xdeltabar);
					  writeln(' ',Xdeltabar:1)
					  end
    else if string =  '%Xdeltanum'  then  begin
					  readln(Xdeltanum);
					  writeln(' ',Xdeltanum:1)
					  end
    else if string =  '%Ydeltabar'  then  begin
					  readln(Ydeltabar);
					  writeln(' ',Ydeltabar:1)
					  end
    else if string =  '%Ydeltanum'  then  begin
					  readln(Ydeltanum);
					  writeln(' ',Ydeltanum:1)
					  end
    else if string =  '%Xratiog:r'  
                           then begin
		                readln(Xscalegraph,Xscalereal);
	       	                writeln(' ',Xscalegraph:1,' ',Xscalereal:7:2)
			        end
    else if string =  '%Yratiog:r'  
                           then begin
			        readln(Yscalegraph,Yscalereal);
			        writeln(' ',Yscalegraph:1,' ',Yscalereal:7:2)
			        end
    else if string =  '%want-X^Y^' 
                           then begin
			        readln(wantXY[1],wantXY[2]);
			        writeln(wantXY[1],wantXY[2])
			        end
    else if string =  '%plotchar1'  then  begin
					    strreadline;
					    plotchar1:=commandstr
					  end
    else if string =  '%plotchar2'  then  begin
					    strreadline;
					    plotchar2:=commandstr
					  end
    else if string =  '%plotchar3'  then  begin
					    strreadline;
					    plotchar3:=commandstr
					  end
    else if string =  '%plotchar4'  then  begin
					    strreadline;
					    plotchar4:=commandstr
					  end
    else if string =  '%plotchar5'  then  begin
					    strreadline;
					    plotchar5:=commandstr
					  end
    else begin
	  readln;writeln;
          writeln('**********************************************');
          writeln('error1: unknown string :\',string,'\');
	  writeln('**********************************************');
	  error1:=true
	 end;
    string := blank10;
    strreadword
   end; (* while *)
   readln;  (* this readln is to finish reading the %%%%%.. line*)
   writeln;

   (* THE FUN BEGINS HERE *)
   if not error1 then
   begin
   textwidth := piclength*unitlngth + 15.0;  (* in mm *)
   if textwidth >240
     then begin
	  writeln('% *****************************************************');
          writeln('% max-possible-piclength is about 240mm which will have ');
	  writeln('% to be in Landscape. You''ll have to reduce scales.');
	  writeln('% *****************************************************')
	  end
     else if textwidth > 170
             then begin
		  writeln
		  ('% *****************************************************');
	          writeln
		  ('% THIS TEXT IS A BIT TOO WIDE FOR VERTICAL PAPER MODE.');
                  writeln
		  ('% YOU WILL HAVE TO USE "dvi-imagen -L  file" TO PRINT.');
		  writeln
		  ('% *****************************************************')
		  end;

   if prepost = 'ye' then
   begin
   writeln('\documentstyle{article}');
   writeln('\setlength{\topmargin}{0cm}');
   writeln('\setlength{\headheight}{1cm}');
   writeln('\setlength{\footskip}{3cm}');
   writeln('\setlength{\oddsidemargin}{0cm}');
   writeln('\setlength{\textwidth}{',textwidth:6:2,'mm}');
   writeln;
   writeln('\renewcommand{\thepage}{}     % => no page number ');
   writeln('\begin{document}');
   writeln;
   end; (* end prepost *)
   
   writeln('\newcommand{\xaxis}{',Xaxisstrg,
                                      '} % the literal for X-axis');
   writeln('\newcommand{\yaxis}{',Yaxisstrg,
                                       '} % the literal for Y-axis');
   if plotchar1[1] <> 'n' then
	writeln('\newcommand{\plotcharone}',plotchar1);
   if plotchar2[1] <> 'n' then
	writeln('\newcommand{\plotchartwo}',plotchar2);
   if plotchar3[1] <> 'n' then
	writeln('\newcommand{\plotcharthree}',plotchar3);
   if plotchar4[1] <> 'n' then
	writeln('\newcommand{\plotcharfour}',plotchar4);
   if plotchar5[1] <> 'n' then
	writeln('\newcommand{\plotcharfive}',plotchar5);
   writeln;
   writeln('\begin',fontname); (* fontname contains the {} *)
   writeln('\begin{figure}[p]     %you might want different options here');

   (* Xorig & Yorig refer to the actual origin of the graph and not to the *)
   (* origin of the pic box. 			                           *)
   Xorig:= 0;
   Yorig:= 0;
   writeln('\unitlength = ',unitlngth:3:2,'mm');
   writeln('\begin{picture}(',(piclength+10):1,',',(picheight+10):1,')(',
			(Xorig-10):1,',',(Yorig-10):1,')');
   (* need a box of +10 on both axes to account for -10 origins & to *)
   (* get proper centering *)
   writeln;
   
   writeln('\thicklines');
   writeln('\put(',Xorig:1,',0){\vector(1,0){',piclength:1,'}}');
   writeln('\put(',Xorig:1,',0){\vector(0,1){',picheight:1,'}}');
   writeln('\thinlines');
   
   (*********************************************************************)
   (* Following statements make the arrows and the X & Y 		*)
   (*********************************************************************)
   
   if wantXY = 'ye' then   (* I only read first two char of 'yes' *)
   begin
   writeln('\put(',Xorig:1,',-9){\makebox(',piclength:1,
	',2){X \hspace{0.6em} {\raisebox{0.6ex}{\vector(1,0){8}}}}}');
   writeln('\put(',(Xorig-11):1,',',((picheight div 2) - 7):1,
                      '){\makebox(2,2){Y}}');
   writeln('\put(',(Xorig-10):1,',',(picheight div 2 - 1):1,
                      '){\vector(0,1){8}}');
   end; (* wantXY *)
   
   (*********************************************************************)
   (*the following writes statements for writing numbers & bars along X-axis*)
   (*********************************************************************)
   
   Xtimes := piclength div Xdeltabar;
   writeln('\multiput(',Xorig:1,',-1)(',Xdeltabar:1,',0){',
	     Xtimes:1,'}{\line(0,1){2}}');
   (* Xdelnum is in graph units *)
   Xdeln  := (Xscalereal*Xdeltanum) / Xscalegraph;   
   position := Xdeltanum; Xnum:=Xdeln;
   if (Xnum - trunc(Xnum)) > 0 then itsreal := true;
   writeln
     ('% Add a line similar to next one if 0 for X-axis at origin desired.');
   while position < piclength  do
     begin
     if itsreal
     then writeln('\put(',position:1,',-5){\makebox(0,2){',Xnum:7:2,'}}')
     else writeln('\put(',position:1,',-5){\makebox(0,2){',trunc(Xnum):1,'}}');
     
     position := position + Xdeltanum;
     Xnum     := Xnum + Xdeln  (* first number is always 0 *)
     end;

   (*********************************************************************)
   (*the following writes statements for writing numbers & bars along Y-axis*)
   (*********************************************************************)
   
   Ytimes := picheight div Ydeltabar;
   writeln('\multiput(',(Xorig-1):1,',0)(0,',Ydeltabar:1,'){',
	     Ytimes:1,'}{\line(1,0){2}}');
   (* Ydelnum is in graph units *)
   Ydeln  := ( Yscalereal*Ydeltanum) / Yscalegraph;
   position := Ydeltanum; Ynum:=Ydeln;
   if (Ynum - trunc(Ynum)) > 0 then itsreal := true;
   writeln
     ('% Add a line similar to next one if 0 for Y-axis at origin desired.');
   while position < picheight do
      begin
      if itsreal
      then writeln('\put(',(Xorig-7):1,',',
		position:1,'){\makebox(5,0)[r]{',Ynum:7:2,'}}')
      else writeln('\put(',(Xorig-7):1,',',
		position:1,'){\makebox(5,0)[r]{',trunc(Ynum):1,'}}');
      position := position + Ydeltanum;
      Ynum     := Ynum + Ydeln
      end;

   if legendloc <>  'no' then
   begin   (* and it is then obviously do nothing *)
   if legendloc = 'tr' then begin
			    Xlegloc:= piclength - 10 - 30;
			    Ylegloc:= picheight - 10 - 12
			    end
      else if legendloc = 'tl' then begin
			            Xlegloc:= Xorig + 10;
				    Ylegloc:= picheight - 10 -12
				    end
      else begin
           Xlegloc := legendXcord;
	   Ylegloc := legendYcord
	   end;

   (* the - 10 in above commands is to account for the shifted origin which*)
   (* as of now is fixed at (-10,-10) *)
   
   writeln('% you might have to fix the box below to get the circles right.');
   writeln('% also you''ll have to fiddle with the X & Y coord of the box to');
   writeln('% place it right. I have made an attempt at placement.');
   writeln('% Change the AA,BB,....,EE in the box below to whatever you want');
   writeln('% and try to keep it short; I don''t expect other changes.');
   
   writeln('\put(',Xlegloc:1,',',Ylegloc:1,
                     '){\fbox{\shortstack[l]{');
   if plotchar1[1] <> 'n' then
	writeln(' {\makebox(4,2)[lb]{\put(2,1){\plotcharone}}}: AA\\');
   if plotchar2[1] <> 'n' then
	writeln(' {\makebox(4,3)[lb]{\put(2,1){\plotchartwo}}}: BB\\');
   if plotchar3[1] <> 'n' then
	writeln(' {\makebox(4,3)[lb]{\put(2,1){\plotcharthree}}}: CC\\');
   if plotchar4[1] <> 'n' then
	writeln(' {\makebox(4,3)[lb]{\put(2,1){\plotcharfour}}}: DD\\');
   if plotchar5[1] <> 'n' then
	writeln(' {\makebox(4,3)[lb]{\put(2,1){\plotcharfive}}}: EE\\');
   writeln(' {\makebox(4,4)[b]{X}}: \xaxis \\');
   writeln(' {\makebox(4,2)[b]{Y}}: \yaxis');
   writeln(' }}}');
   
   end; (* not 'no' *)
   
   
   
   captiontwo[1]:=capstring[1];
   captiontwo[2]:=capstring[2];
   if captiontype[1] = 'e' then    (* "e"xplicit  , anything else other than *)
				   (* "L" for LaTeX => no   *)
   if captiontwo = 'YX' then
   begin
   writeln('\put(',Xorig:1,',-18){\makebox(',piclength:1,',0)[t]{');
   writeln('Figure ',fignumber,'$\!$:\hspace{0.5em}');
   writeln('\yaxis \hspace{0.5em} vs. \hspace{0.5em} \xaxis}}');
   writeln('% if the caption line is longer than the graphwidth, comment out');
   writeln('% the previous stmt and use next one. you might have to fiddle');
   writeln('% with the width of parbox.');
   writeln('%\put(',Xorig:1,',-18){\makebox(',piclength:1,',0)[tl]{');
   writeln('%Figure ',fignumber,'$\!$:\hspace{0.5em}\parbox[t]{',
             (textwidth-32):6:2,'mm}{');
   writeln('%\yaxis \hspace{0.5em} vs. \hspace{0.5em} \xaxis}}}')
   end
   else  (* means explicit string is specified *)
   begin
   writeln('\put(',Xorig:1,',-18){\makebox(',piclength:1,',0)[t]{');
   writeln('Figure ',fignumber,'$\!$:\hspace{0.5em}');
   writeln(capstring);
   writeln('}}');
   writeln('% if the caption line is longer than the graphwidth, comment out');
   writeln('% the previous stmt and use next one. you might have to fiddle');
   writeln('% with the width of parbox.');
   writeln('%\put(',Xorig:1,',-18){\makebox(',piclength:1,',0)[tl]{');
   writeln('%Figure ',fignumber,'$\!$:\hspace{0.5em}\parbox[t]{',
             (textwidth-32):6:2,'mm}{');
   writeln('%',capstring);
   writeln('%}}}')
   end;
   writeln('% beginning of data');

   (* the following statements reads the stdin and simply *)
   (* copies them to stdout *)
   (*       while not eof do *)
   (*       begin *)
   (*       commandstr:=blank79; *)
   (*       strreadline; *) (* strreadline also does the writing *)
   (*       end; *)
       
   while not eof do
   begin
   readln(plotcharnum,xreality,yreality);
   xgraph := (xreality*Xscalegraph)/Xscalereal;
   ygraph := (yreality*Yscalegraph)/Yscalereal;
   if plotcharnum = 1 then
       writeln('\put(',xgraph:10:5,',',ygraph:10:5,'){\plotcharone}')
   else if plotcharnum = 2 then
       writeln('\put(',xgraph:10:5,',',ygraph:10:5,'){\plotchartwo}')
   else if plotcharnum = 3 then
       writeln('\put(',xgraph:10:5,',',ygraph:10:5,'){\plotcharthree}')
   else if plotcharnum = 4 then
       writeln('\put(',xgraph:10:5,',',ygraph:10:5,'){\plotcharfour}')
   else if plotcharnum = 5 then
       writeln('\put(',xgraph:10:5,',',ygraph:10:5,'){\plotcharfive}')
   end;


   writeln('% end of data');
   writeln('\end{picture}');
   
   if captiontype[1] = 'L' then     (* "L"aTeX . if it is not = 'L' or 'e'*)
				    (* then interpreted as 'no'  *)
   begin
   writeln('% if the caption line is longer than the graphwidth,use a');
   writeln('% \parbox[t]{...mm}{.........} like statement for the argument');
   writeln('% with suitable args for parbox to get the whole thing centered.');
   if captiontwo = 'YX' 
	   then begin
		writeln('% You might want to do something to the [] in',
			   'following caption statement');
		writeln('\caption[]{\normalsize \yaxis vs. \xaxis }')
		end
	   else begin
		writeln('\caption{\normalsize ');
		writeln(capstring);
		writeln('}')
		end;
   if labelname[1] = '{' then    (* anything else => 'no' *)
	writeln('\label',labelname)   (* labelname contains the {} *)
   end;
   
   writeln('\end{figure}');
   writeln;  (* a blank line is supposedly needed before \end{fontname} *)
   		      (* so says the latex manual *)
   writeln('\end',fontname);  (* fontname contains the {} *)
   writeln;
   
   if prepost = 'ye' then
   	writeln('\end{document}');
   end (* not error1 *)
end.
