/* information about dam */
prinfo(){
int f;
char c;
        f = open("prinfo.x",0);
        if(f < 0) f = open("/usr/games/lib/dam/prinfo.x",0);
        if(f < 0){
                printf("sorry, cannot find it\n");
                return;
        }
        while(read(f,&c,1) == 1) putchar(c);
        (void) close(f);
}
