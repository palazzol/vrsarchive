/* time accounting */
long int tvec0, tvec;

timebeg(){
        (void) time(&tvec0);
}

timedif(){
        (void) time(&tvec);
        return(tvec - tvec0);
}
