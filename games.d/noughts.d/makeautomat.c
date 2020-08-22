# include "stdio.h"
# define FOUND 1
# define NOT_FOUND 0
# define DUMMY 'd'

empty(){
	while(getchar() != '\n');
}

fempty(fd) FILE *fd;{
	while(getc(fd) != '\n');
}

main(){
	char text[128], uttext[128], c;
	char textarray[512][16];
	int automat[512][3]; /* .=0 O=1 X=2 */
	char pattern[4];
	int offs, offset[3], point[3], threat[3];
	int p[10], t[10], count, length;
	FILE *fd_in, *fd_out;
	int i, j, k;
	int flag;

/* ********** Pass 1 *********************************************************
L{ser in en fil fr}n standard input med formatet:
---------
|
|
| Block vilket inneh}ller po{ngdefenitioner.
|
|
---------
|
|
| Block vilket inneh}ller m|nster vilka ger po{ng.
|
|
---------
Utdata skrivs p} filen make_first och inneh}ller m|nster plus po{ng.
*************************************************************************** */

	printf("Pass 1\n");
	for(i = 0; i < 10; i++){
		scanf("%*c%d%d", &t[i], &p[i]); empty();
	}

	fd_out = fopen("make_first", "w");
	while(scanf("%s", text) != EOF){
		if(text[0] == 0)
			continue;
		j = 0;
		for(i = 0; i < 3; i++){
			offset[i] = 0;
			point[i] = 0;
			threat[i] = 0;
		}
		for(count = 0; text[count] != 0; count++);
		for(i = 0; text[i] !=0; i++){
			if((text[i] == '.') || (text[i] == 'X'))
				uttext[i] = text[i];
			else{
				offset[j] = count - i - 1;
				threat[j] = t[text[i] - 'a'];
				point[j] = p[text[i] - 'a'];
				uttext[i] = '.';
				j++;
			}
		}
		uttext[i] = 0;
		fprintf(fd_out, "%-10s%3d", uttext, j);
		for(i = 0; i < 3; i++)
			fprintf(fd_out,"%5d%3d%3d", offset[i], threat[i], point[i]);
		fprintf(fd_out,"\n");
	}
	fclose(fd_out);

/* ********** Pass 2 *********************************************************
L{ser filen make_first samt l{gger till motsvarande m|nster med
O i st}llet f|r X.
*************************************************************************** */

	printf("Pass 2\n");
	fd_in = fopen("make_first", "r");
	fd_out = fopen("make_temp1", "w");
	while((fscanf(fd_in, "%c", &c)) != EOF)
		if(c == 'X')
			fprintf(fd_out, "O");
		else
			fprintf(fd_out, "%c", c);
	fclose(fd_in);
	fclose(fd_out);
	system("cat make_first make_temp1 > make_temp2");
	system("mv make_temp2 make_first");
	system("rm make_temp1");

/* ********** Pass 3 *********************************************************
L{ser filen make_first samt l{gger till delm|nster vilka leder till m|nstren.
Om delm|nstret redan finns skall det ej l{ggas till. D.v.s inga kopior
skall uppst}. Delm|nstren ges po{ng noll. Filen sorteras.
*************************************************************************** */

	printf("Pass 3\n");
	fd_in = fopen("make_first", "r");
	length = 0;
	while((fscanf(fd_in, "%s", textarray[length])) != EOF){
		length++;
		fempty(fd_in);
	}
	count = length;
	fclose(fd_in);
	fd_out = fopen("make_first", "a");
	for(i = 0; i < count; i++){
		strcpy(text, textarray[i]);
		for(; text[0] != 0; text[strlen(text) - 1] = 0){
			flag = NOT_FOUND;
			for(j = 0; j < length; j++)
				if(!strcmp(text, textarray[j])){
					flag = FOUND;
					break;
				}
			if(flag == NOT_FOUND){
				strcpy(textarray[length++], text);
				fprintf(fd_out, "%-10s%3d", text, 0);
				for(k = 0; k < 3; k++)
					fprintf(fd_out, "%5d%3d%3d", 0, 0, 0);
				fprintf(fd_out, "\n");
				length++;
			}
		}
	}
	fclose(fd_out);
	system("sort make_first -o make_sort");

/* ********** Pass 4 *********************************************************
Det sista passet vilket konstruerar automaten samt g|r om filen till en
c-fil med namnet automat.c.
*************************************************************************** */

	printf("Pass 4\n");
	fd_in = fopen("make_sort", "r");
	strcpy(textarray[0], "");
	length = 1;
	while((fscanf(fd_in, "%s", textarray[length])) != EOF){
		length++;
		fempty(fd_in);
	}
	strcpy(pattern, ".OX");
	for(i = 0; i < length; i++){
		strcpy(text, textarray[i]);
		for(j = strlen(text) + 1; j < 20; j++)
			text[j] = 0;
		text[strlen(text)] = DUMMY;
		for(j = 0; j < 3; j++){
			text[strlen(text) - 1] = pattern[j];
			flag = NOT_FOUND;
			for(offs = 0; offs < 11; offs++){
				for(k = 0; k < length; k++)
					if(!strcmp(text + offs, textarray[k])){
						flag = FOUND;
						automat[i][j] = k;
						break;
					}
				if(flag == FOUND)
					break;
			}
			if(flag == NOT_FOUND)
				printf("Cant find %s\n", text);
		}
	}
	fclose(fd_in);
	fd_in = fopen("make_sort", "r");
	fd_out = fopen("make_automat", "w");
	for(i = 0; i < 3; i++){
		offset[i] = 0;
		point[i] = 0;
		threat[i] = 0;
	}
	count = 0;
	for(i = 0; i < length; i++){
		fprintf(fd_out, "/*%3d %-10s*/", i, textarray[i]);
		fprintf(fd_out, "%3d,%3d,%3d,", automat[i][0], automat[i][1],
			automat[i][2]);
		fprintf(fd_out, "%5d,", count);
		for(j = 0; j < 3; j++)
			fprintf(fd_out, "%4d,%3d,%3d,", offset[j], threat[j],
				point[j]);
		fprintf(fd_out, "\n");
		fscanf(fd_in, "%*s%d", &count);
		for(j = 0; j < 3; j++)
			fscanf(fd_in, "%d%d%d\n", &offset[j], &threat[j],
				&point[j]);
	}
	fprintf(fd_out, "};\n");
	fclose(fd_in);
	fclose(fd_out);
	system("cat make_header make_automat | sed 's/ 0,/ __/g' > automat.c");
	system("rm make_first make_sort make_automat");
}
