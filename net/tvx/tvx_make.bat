REM This is the batch file needed to compile and line TVX using
REM Computer Innovations C-86 compiler.  If you have a different
REM compiler, you will have to make your own batch file.
REM	This batch file assumes stdio.h is in a directory called \c\.
REM
cc1 tvx_1 -hc:\c\
cc2 tvx_1
cc3 tvx_1
cc4 tvx_1
cc1 tvx_2 -hc:\c\
cc2 tvx_2
cc3 tvx_2
cc4 tvx_2
cc1 tvx_lex -hc:\c\
cc2 tvx_lex
cc3 tvx_lex
cc4 tvx_lex
cc1 tvx_io -hc:\c\
cc2 tvx_io
cc3 tvx_io
cc4 tvx_io
cc1 tvx_lib -hc:\c\
cc2 tvx_lib
cc3 tvx_lib
cc4 tvx_lib
cc1 tvx_ibm -hc:\c\
cc2 tvx_ibm
cc3 tvx_ibm
cc4 tvx_ibm
REM
REM	Now, link the thing into a file called TVX.EXE
REM	It uses the CII small, DOS 2 library, which is called /c/cslib2
REM	on my system.  You may need to replace the library with whatever
REM	you call the corresponding library (like c86s2s.lib).
REM
link tvx_1+tvx_2+tvx_lex+tvx_io+tvx_lib+tvx_ibm,tvx,nul:,/c/cslib2
REM
REM	Build the patch / configuration utilities
REM
cc1 tvx_cfg -hc:\c\
cc2 tvx_cfg
cc3 tvx_cfg
cc4 tvx_cfg
link tvx_cfg,tvx_cfg,nul:,/c/cslib2
cc1 tvx_ptch -hc:\c\
cc2 tvx_ptch
cc3 tvx_ptch
cc4 tvx_ptch
link tvx_ptch,tvx_ptch,nul:,/c/cslib2
REM
REM	Build of tvx finished
REM
