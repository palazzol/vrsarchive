BIN = /usr/local

install:	$(BIN)/index $(BIN)/iindex ;

clean:
		rm -f lex.yy.o

$(BIN)/index:	index.l
		lex index.l
		cc -O -o $@ lex.yy.c -ll
		chmod 711 $@
		rm lex.yy.c

$(BIN)/iindex:	index.l
		lex index.l
		cc -O -o $@ lex.yy.c -ll -DPR=\"imprint\"
		chmod 711 $@
		rm lex.yy.c
