/*-----------------------------------------------------------------------*/
/* Casino blackjack.  To invoke:                                         */
/*      bj [-dN] [-pM] [-bO] [-mP] [-fQ] [-u]                            */
/* where N is replaced by the number of decks to be used, M is replaced  */
/* by the number of players, O is replaced by the starting bankroll, P   */
/* is replaced by the minimum bet, Q is replaced by the name of a saved  */
/* game file, and u tells the system to maintain the Uston Simple Plus/  */
/* Minus count                                                           */
/* Defaults are: decks=8, players=3 (including the user but not house),  */
/* bankroll=100 (for everyone except the house, which has an infinite    */
/* bankroll, and minimum bet=2.                                          */
/*                                                                       */
/* To compile:  cc cbj.c -o cbj                                          */
/*                                                                       */
/*                          Miles Murdocca                               */
/*                          January, 1985                                */
/*                                                                       */
/*-----------------------------------------------------------------------*/

#include <stdio.h>
#include <curses.h>     /* For definitions of TRUE = 1 and FALSE = 0 */
#include <signal.h>

#define VERSION 1.0
#define MAXDECKS        16 /* The maximum number of decks allowed */
#define MAXPLAYERS      8  /* Max number of players allowed (includ house) */
#define MAXSIZEOFHAND 12        /* in cards */
#define MAXSPLITS 2     /* The max number of possible hands due to splits */
#define MAXDEALERSHAND 6 /* Some casinos limit the size of the dealer's hand*/
#define HIDEDOWNCARDS 0
#define SHOWALLCARDS 1
#define ENDOFDECK -1
#define NEWLINE '\n'
#define ENDOFSTRING 0
#define HOUSE   0
#define USER    1

int rand();
int no_decks = 8;       /* The default number of decks */
int no_players = 4;     /* The default number of players (incl house) */
int bank_roll = 100;    /* The default bankroll for the players */
int deck_ptr = 0;
int min_bet = 2;        /* The minimum bet */
int insurance_played;
int no_hands_played;    /* The number of hands played so far */
int uston_countp = FALSE; /* TRUE/FALSE maintain the Uston count */
int uston_count = 0;    /* The uston count */

char *pack[52] = {      /* What a pack of cards looks like */
"A-spades", "2-spades", "3-spades", "4-spades", "5-spades", "6-spades",
"7-spades", "8-spades", "9-spades", "10-spades", "J-spades", "Q-spades",
"K-spades",
"A-hearts", "2-hearts", "3-hearts", "4-hearts", "5-hearts", "6-hearts",
"7-hearts", "8-hearts", "9-hearts", "10-hearts", "J-hearts", "Q-hearts",
"K-hearts",
"A-clubs", "2-clubs", "3-clubs", "4-clubs", "5-clubs", "6-clubs",
"7-clubs", "8-clubs", "9-clubs", "10-clubs", "J-clubs", "Q-clubs",
"K-clubs",
"A-diamonds", "2-diamonds", "3-diamonds", "4-diamonds", "5-diamonds",
"6-diamonds", "7-diamonds", "8-diamonds", "9-diamonds", "10-diamonds",
"J-diamonds", "Q-diamonds", "K-diamonds"
    };

struct playertemplate {
    float bank_roll;    /* How much money a player has */
    int no_hands;       /* The number of hands a player has */
    int hand[MAXSPLITS][MAXSIZEOFHAND]; /* His hand(s) */
    long bet[MAXSPLITS]; /* The size of the current bets for this player */
    int broke;          /* Player has no more money */
    int bust[MAXSPLITS]; /* Player's hand(s) are over 21 */
    int insurance;      /* TRUE/FALSE */
    int surrender;      /* TRUE/FALSE */
    int stand[MAXSPLITS]; /* TRUE/FALSE for each hand */
    int credit;         /* TRUE/FALSE is the player betting on credit? */
    long original_bet;   /* For insurance() */
    int double_down[MAXSPLITS]; /* TRUE/FALSE is the user's hand double downed? */
    int dd_took_3rd_card[MAXSPLITS]; /* T/F Has player taken 3rd card on double down? */
    } players[MAXPLAYERS];

float amount_user_won = 1.0;  /* These two variables are initialized to 1 */
float amount_user_lost = 1.0; /* instead of 0 to prevent zerodivide in update() */
int deck[52*MAXDECKS];
SIG_T leave();
char *cptr;
char user_name[100];
char *getlogin();
main(argc, argv)
int argc;
char *argv[];
{
    int i, j, tmp;
    char str[100];

    signal(SIGINT, leave);        /* Set to restore tty stats */
    srand(getpid());  /* Seed the random number generator */
    cptr = getlogin();
    strcpy(str, cptr);
    strcpy(user_name, "*** ");
    strcat(user_name, str);
    if (process_options(argc, argv))    { /* Not a restored game */
	printf("\nCASINO BLACKJACK version %1.1f\n", VERSION);
	if (no_decks > 1 && no_players > 2)
	    printf("%d decks, %d players\n", no_decks, no_players-1);
	else if (no_decks == 1 && no_players > 2)
	    printf("%d deck, %d players\n", no_decks, no_players-1);
	else if (no_decks > 1 && no_players == 2)
	    printf("%d decks, %d player\n", no_decks, no_players-1);
	else printf("%d deck, %d player\n", no_decks, no_players-1);
        initialize();       /* Initialize the players bankrolls */
	printf("Beginning bankrolls -\t%s: $%d.00\n", user_name, bank_roll);
	for (i = 2; i < no_players; i++)
	    printf("\t\t\tPlayer #%d: $%d.00\n", i, bank_roll);
	printf("Minimum bet: $%d.00\n\n", min_bet);
	}
    else {
	update();
	goto RESTART;
	}

    while(i == i) {
	if ((deck_ptr == 0) || (deck_ptr >= (52*no_decks)/2))
	    shuffle(deck);
	placebets();
	deal(deck);
	print_hands(HIDEDOWNCARDS);
	/* Play ends immediately if house has a 'natural' blackjack */
	if (blackjack(players[HOUSE].hand[j])) {
	    printf("HOUSE has blackjack.");
	    goto SETTLEBETS;
	    }
	for (i = USER; i < no_players; i++)
	    for (j = 0; j < players[i].no_hands; j++)
	    /* House pays player at 3/2 if player has blackjack on the */
	    /* first 2 cards */
		if (blackjack(players[i].hand[j])) {
		    if (i == USER) {
			printf("%s has blackjack.  ", user_name);
			printf("House pays %s at 3 to 2 against original\n", user_name);
			amount_user_won += 1.5 * players[i].bet[j];
			}
			else {
			printf("Player #%d has blackjack.  ", i);
			printf("House pays player #%d at 3 to 2 against original\n", i);
			}
		    printf("bet.  Play continues for the other players.\n");
		    players[i].bank_roll += 2.5 * players[i].bet[j];
		    players[i].bet[j] = 0;
		    players[i].stand[j] = TRUE;
		    }
RESTART: for (i = USER; i < no_players; i++)   /* User plays first */
	    while (play_hand(i));
	while (play_hand(HOUSE)); /* The house plays last */
SETTLEBETS: print_hands(SHOWALLCARDS);
	payoff();
	no_hands_played++;
	re_initialize();
	}
    }


/* Shuffle the deck of cards */
shuffle(deck)
int deck[];
{
    int i, tmp;

    printf("Shuffling deck...\n");
    for (i = 0; i < 52*no_decks; i++) deck[i] = ENDOFDECK;
    for (i = 0; i < 52*no_decks; i++) {
	while(((tmp = rand()&0x3f) >= 52) || member(tmp, deck));
	deck[i] = tmp;
	}
    deck_ptr = 0;
    uston_count = 0;
    }

/* Return TRUE if the maximum number of card are in the deck */
member(card, deck)
int deck[];
{
    int i, count;

    count = 0;
    for (i = 0; i < 52; i++) {
	if (deck[i] == card) count++;
	if (count >= no_decks) return(TRUE);
	}
    return(FALSE);
    }


/* Process the command line options */
process_options(argc, argv)
int argc;
char *argv[];
{
    int i;
    char filename[100];

    for (i = 1; i < argc; i++) {
	if (argv[i][0] != '-') error(0);
	switch(argv[i][1]) {
	    case 'b': sscanf(argv[i]+2, "%d", &bank_roll); break;
	    case 'd': sscanf(argv[i]+2, "%d", &no_decks); break;
	    case 'f': sscanf(argv[i]+2, "%s", filename);
		cont_game(filename);
		return(FALSE);
		break;
	    case 'p': sscanf(argv[i]+2, "%d", &no_players);
	if (++no_players > MAXPLAYERS) error(3);  /* increment to include house */
		break;
	    case 'm': sscanf(argv[i]+2, "%ld", &min_bet); break;
	    case 'u': uston_countp = TRUE; break;
	    default: printf("Bad option: %s\n", argv[i]); error(1); break;
	    }
	}
    if (no_decks > MAXDECKS) error(2);
    if (no_players < 2) error(4);
    return(TRUE);
    }


/* Print an error message and die. */
error(index)
int index;
{
    switch(index) {
	case 0: printf("Command line arguments must be preceded by '-'\n");
	    break;
	case 1: break;
	case 2: printf("Too many decks.  Max is: %d\n", MAXDECKS); break;
	case 3: printf("Too many players.  Max is: %d\n", MAXPLAYERS); break;
	case 4:
	    printf("Too few players.  There must be at least 2:\n");
	    printf("one for the house and one for the user.\n");
	    break;
	case 5: printf("Ran out of cards.\n"); update(); break;
	default: printf("Bad argument to error(%d)\n", index); break;
	}
    exit(-1);
    }

/* Initialize the players bankrolls and hands */
initialize()
{
    int i, j;

    insurance_played = FALSE;
    deck_ptr = 0;
    for (i = 0; i < no_players; i++) {
	players[i].bank_roll = bank_roll;
	players[i].no_hands = 1;
	players[i].broke = FALSE;
	for (j = 0; j < MAXSPLITS; j++) {
	    players[i].bust[j] = FALSE;
	    players[i].hand[j][0] = ENDOFDECK;
	    players[i].stand[j] = FALSE;
	    players[i].double_down[j] = FALSE;
	    }
	players[i].insurance = FALSE;
	players[i].surrender = FALSE;
	players[i].credit = FALSE;
	}
    }

/* Initialize for a new hand, after the first hand has already been played */
re_initialize()
{
    int i, j;

    insurance_played = FALSE;
    for (i = 0; i < no_players; i++) {
	if (players[i].broke == FALSE) {
	    players[i].no_hands = 1;
	    for (j = 0; j < MAXSPLITS; j++) {
		players[i].bust[j] = FALSE;
		players[i].hand[j][0] = ENDOFDECK;
		players[i].stand[j] = FALSE;
		players[i].double_down[j] = FALSE;
		}
	    players[i].insurance = FALSE;
	    players[i].surrender = FALSE;
	    }
	}
    }

/* Deal from the deck[] to the players[] */
deal(deck)
int deck[];
{
    int i, j;

    /* Initially, each player gets 2 cards.  The first card is dealt */
    /* down, and the others are dealt face up.  The "down" card is */
    /* always players[i].hand[j][0].  The dealer is always player 0, */
    /* and the user is always player 1. */
    for (j = 0; j < 2; j++) {
	for (i = 0; i < no_players; i++) {
	    if (players[i].broke == FALSE) {
		uston(deck[deck_ptr]);
		players[i].hand[0][j] = deck[deck_ptr++];
		}
	    }
	}

    for (i = 0; i < no_players; i++) {
	players[i].no_hands = 1;
	players[i].hand[0][2] = ENDOFDECK;
	}
    }


/* The players place their bets */
placebets()
{
    int i;
    char line[100], response[100];

    /* The house, of course, does not bet */
    BET: printf("\nYour bankroll is $%.2f.  Minimum bet is $%d.00.\nBet? $",
	players[USER].bank_roll, min_bet);
    getline(line);
    sscanf(line,"%s", response);
    if (not(is_a_number(response))) {
	printf("Must enter a dollar amount (in numbers).\n");
	goto BET;
	}
    sscanf(response, "%ld", &players[USER].bet[0]);
    /* In case the user bet fractional parts of a dollar*/
    if (!check_bet(players[USER].bet[0])) goto BET;
    players[USER].bank_roll -= players[USER].bet[0];
    players[USER].original_bet = players[USER].bet[0];
    if (players[USER].bank_roll < 0 && players[USER].credit == FALSE) {
	printf("You are broke.  Play on credit? ");
	getline(line); tolower(line);
	sscanf(line, "%s", response);
	if (response[0] == 'n') {
	    printf("Wise choice.\n");
	    leave();
	    }
	if (response[0] != 'y') printf("I assume that means \"yes\".\n");
	printf ("OK, you can play on credit.  Play sensibly.\n");
	players[USER].credit = TRUE;
	}

    for (i = 2; i < no_players; i++) { /* Players 0, 1 are house and user */
	if (players[i].broke == FALSE) {
	    players[i].bank_roll -= min_bet;
	    if (players[i].bank_roll < 0) {
		printf("Player %d is broke and leaves game.\n", i);
		players[i].broke = TRUE;
		}
	    else {
		players[i].bet[0] = min_bet;
		printf("Player #%d bets $%ld.00\n", i, players[i].bet[0]);
		players[i].original_bet = players[i].bet[0];
		}
	    }
	}
    }

/* Read until newline */
getline(line)
char line[];
{
    int i;

    for (i = 0; (line[i] = getchar()) != NEWLINE; i++);
    line[++i] = ENDOFSTRING;
    }


/* Convert lowercase characters to uppercase characters */
toupper(str)
char str[];
{
    int i;

    for (i = 0; str[i] != ENDOFSTRING; i++) {
	if ((str[i] >= 'a') && (str[i] <= 'z'))
	    str[i] = str[i] + ('A' - 'a');
	}
    }

/* Convert uppercase characters to lowercase characters */
tolower(str)
char str[];
{
    int i;

    for (i = 0; str[i] != ENDOFSTRING; i++) {
	if ((str[i] >= 'A') && (str[i] <= 'Z'))
	    str[i] = str[i] - ('A' - 'a');
	}
    }

/* Print statistics and exit the game */
SIG_T
leave()
{
    int i;
    char line[100], response[100];
    float tmp;

    signal(SIGINT, SIG_IGN);            /* Ignore rubouts */
    printf("Save game? ");
    getline(line); tolower(line);
    sscanf(line, "%s", response);
    if (response[0] == 'y') {
	while (savegame());
	exit(1);
	}
    tmp = 0;
    for (i = 0; i < no_players; i++) {
	if (i != USER)
	    if (players[i].broke == FALSE) tmp++;
	}
    printf("%1.0f players left (besides the house).\n", tmp-1);
    tmp = players[USER].bank_roll - bank_roll;
    if (tmp > 0) printf("Good game.  You are ahead $%.2f.\n", tmp);
    else if (tmp == 0) printf("You broke even.\n");
    else printf("You lost $%.2f.  Maybe you shouldn't gamble.\n", -tmp);
    exit(1);
    }

/* The house takes bets from or pays bets to the players */
payoff()
{
    int i, j, player, house;

    for (i = USER; i < no_players; i++) {
	if (players[i].broke == FALSE) {
	    for (j = 0; j < players[i].no_hands; j++) {
		player = card_count(players[i].hand[j]);
		house = card_count(players[HOUSE].hand[0]);
		/* Player pays house */
		if (player > 21 || (player < house && house <= 21) ||
			(house == 21 && cards(players[HOUSE].hand[0]) == 2)) {
		    if (i == USER) {
		    if (players[i].no_hands == 1)
			printf("%s pays house $%ld.00\n", user_name,
			    players[i].bet[j]);
			else printf("%s pays house $%ld.00 for hand #%d\n",
			    user_name, players[i].bet[j], j+1);
			amount_user_lost += players[i].bet[j];
			}
		    else {
		    if (players[i].no_hands == 1)
			printf("Player %d pays house $%ld.00\n", i,
			    players[i].bet[j]);
			else printf("Player %d pays house $%ld.00 for hand #%d\n",
			    i, players[i].bet[j], j+1);
			}
		    }

		/* House pays player */
		else if (house > 21 || player > house) {
		    if (i == USER) {
			if (players[i].no_hands == 1)
			    printf("House pays %s $%ld.00\n", user_name,
				players[USER].bet[j]);
			    else printf("House pays %s $%ld.00 for hand #%d\n",
				user_name, players[USER].bet[j], j+1);
			players[i].bank_roll += 2*players[i].bet[j];
			amount_user_won += players[i].bet[j];
			}
		    else {
			if (players[i].no_hands == 1)
			    printf("House pays player #%d $%ld.00\n",
				i, players[i].bet[j]);
			    else printf("House pays player #%d $%ld.00 for hand %d\n",
				i, players[i].bet[j], j+1);
			players[i].bank_roll += 2*players[i].bet[j];
			}
		    }

		/* Both player and house have blackjack - no payoffs */
		else {
		    if (i == USER) {
			if (players[i].no_hands == 1)
			    printf("No payoffs to or from %s\n", user_name,
				players[USER].bet[j]);
			    else printf("No payoffs to or from %s for hand #%d\n",
				user_name, players[USER].bet[j], j+1);
			players[i].bank_roll += players[i].bet[j];
			amount_user_won += players[i].bet[j];
			amount_user_lost += players[i].bet[j];
			}
		    else {
			if (players[i].no_hands == 1)
			    printf("No payoffs to or from player #%d $%ld.00\n",
				i, players[i].bet[j]);
			    else printf("No payoffs to or from #%d $%ld.00 for hand %d\n",
				i, players[i].bet[j], j+1);
			players[i].bank_roll += players[i].bet[j];
			}
		    }

		} /* End "j" loop */
	    } /* End "if" */
	} /* End "i" loop */
    } /* End payoff() */


/* Return the point value of the deck of cards, assign aces values of */
/* either 1 or 11 to get the optimal total count (closest to 21) */
card_count(deck)
int deck[];
{
    int i, count, aces;

    count = 0;  aces = 0;
    for (i = 0; deck[i] != ENDOFDECK; i++) {
	switch(pack[deck[i]][0]) {
	    case 'A': aces++; break;
	    case '2': count += 2; break;
	    case '3': count += 3; break;
	    case '4': count += 4; break;
	    case '5': count += 5; break;
	    case '6': count += 6; break;
	    case '7': count += 7; break;
	    case '8': count += 8; break;
	    case '9': count += 9; break;
	    case '1':
	    case 'J':
	    case 'Q':
	    case 'K': count += 10; break;
	    default:
		printf("Bad card in the pack: %s\n", pack[deck[i]]);
		break;
	    }
	}

    /* Find the optimal values for acess */
    switch(aces) {
	case 0: break;
	case 1:
	    if (count+11 > 21) count++;  else count+= 11;
	    break;
	default: (count += aces - 2);
	case 2:
	    if (count+22 <= 21) count += 22;
	    else if (count+12 <= 21) count += 12;
	    else count += 2;
	    break;
	}

    return(count);
    }


/* Deal another card to a player */
take_card(who, which_hand)
int who, which_hand;
{
    int i;

    if (deck_ptr >= 52*no_decks) error(5);
    if (cards(players[who].hand[0]) >= 3 && players[who].double_down[which_hand]) {
	printf("Can't take more than 3 cards when double downed.\n");
	return;
	}
    if (players[who].double_down[which_hand] && players[who].dd_took_3rd_card[which_hand] == FALSE)
	players[who].dd_took_3rd_card[which_hand] = TRUE;
    for (i = 0; players[who].hand[which_hand][i] != ENDOFDECK; i++);
    uston(deck[deck_ptr]);
    players[who].hand[which_hand][i++] = deck[deck_ptr++];
    players[who].hand[which_hand][i] = ENDOFDECK;
    switch(who) {
	case USER: printf("%s draws ", user_name); break;
	case HOUSE: printf("House draws "); break;
	default: printf("Player #%d draws ", who); break;
	}
    printf("%s\n", pack[players[who].hand[which_hand][i-1]]);
    }


/* What to do when a player stands */
stand(who, which_hand)
int who, which_hand;
{
    if (players[who].double_down[which_hand] && cards(players[who].hand[which_hand]) < 3) {
      printf("You cannot stand when double downed and you don't have 3 cards.\n");
	return;
	}
    players[who].stand[which_hand] = TRUE;
    switch(who) {
	case HOUSE: printf("House stands.\n"); break;
	case USER: printf("%s stands.\n", user_name); break;
	default: printf("Player #%d stands.\n", who); break;
	}
    }


/* Print out the cards */
print_hands(whattodo)
int whattodo;
{
    int i, j, k;

    printf("\n");
    for (i = 0; i < no_players; i++) {
	if (players[i].broke == FALSE) {

	    for (j = 0; j < players[i].no_hands; j++) {
		switch(i) {
		    case USER:
			if (players[i].no_hands == 1)
			    printf("%s: ", user_name);
			else printf("%s, hand #%d: ", user_name, j+1);
			break;
		    case HOUSE:
			if (players[i].no_hands == 1)
			    printf("House: ");
			else printf("House, hand #%d: ", j+1);
			break;
		    default:
			if (players[i].no_hands == 1)
			    printf("Player #%d: ", i);
			else printf("Player #%d, hand #%d: ", i, j+1);
			break;
		    } /* End switch */
		switch(whattodo) {
		    case HIDEDOWNCARDS:
			switch(i) {
			    case USER:
			for (k = 0; players[i].hand[j][k] != ENDOFDECK; k++) {
			    printf("%s, ", pack[players[i].hand[j][k]]);
			    }
			    printf("\010\010 \n");
			    break;
			    case HOUSE:
				printf("%s, ", pack[players[i].hand[j][0]]);
				if (insurance_played)
				  printf("%s, ", pack[players[i].hand[j][1]]);
		       if (cards(players[i].hand[j]) - 1 > 1 ||
			   cards(players[i].hand[j]) - 1 == 0)
		       printf("%d cards down\n", cards(players[i].hand[j])-1);
		       else printf("%d card down\n", cards(players[i].hand[j])-1);
			    break;
			    default:
			for (k = 0; players[i].hand[j][k] != ENDOFDECK; k++) {
			    printf("%s, ", pack[players[i].hand[j][k]]);
			    }
			    printf("\010\010 \n");
			    break;
			    } /* end switch */
			break;
		    case SHOWALLCARDS:
			for (k = 0; players[i].hand[j][k] != ENDOFDECK; k++) {
			    printf("%s, ", pack[players[i].hand[j][k]]);
			    }
			printf("\010\010 \n");
			break;
		    }
		}
	    }
	}
    }


/* Return TRUE if the bet is OK (at least the minimum) */
check_bet(bet)
long bet;
{
    printf("%s's bet is $%ld.00\n", user_name, bet);
    if (bet >= min_bet) return(TRUE);
    printf("Bet is too small.\n");
    return(0);
    }


/* A player plays his hand */
play_hand(who)
int who;
{
    int i, tmp, flag;
    char str[100];

    if (players[who].broke == TRUE) return(FALSE);

    switch(who) {
	case USER:
	    printf("\n[?]\t[h]it\t[q]uit\t[sp]lit\t[st]and\t[d]ouble\t[u]pdate\n");
	    printf("[sa]ve game\t[c]ontinue\t[i]nsurance\t[su]rrender\n");
	    if (uston_countp)
		printf("Uston count: %d\n", uston_count);
	    scanf("%s", str); while (getchar() != NEWLINE);
	    tolower(str);
	    break;
	case HOUSE:
	    if (players[HOUSE].stand[0] == TRUE) return(FALSE);
	    /* House can draw no more than MAXDEALERSHAND cards */
	    if (cards(players[HOUSE].hand[0]) >= MAXDEALERSHAND)
		return(FALSE);
	    str[0] = 'h'; str[1] = ENDOFSTRING;
	    break;
	default:
	    flag = TRUE;
	    for (i = 0; i < players[who].no_hands; i++)
		if (players[who].stand[i] == FALSE) flag = FALSE;
	    if (flag) return(FALSE);
	    str[0] = 'h'; str[1] = ENDOFSTRING;
	    break;
	}

    switch (str[0]) {
	case 'c':
	for (i = 0; i < players[who].no_hands; i++) {
	if (players[who].double_down[i] && players[who].dd_took_3rd_card[i] == FALSE) {
	    printf("You must take another card (double down on hand #%d).\n", i+1);
	    printf("Drawing now...\n");
	    hit(who);
	    return(TRUE); break;
	    }
	    }
	    return(FALSE); break;
	case 'd': double_down(who); return(TRUE); break;
	case 'h': hit(who); return(TRUE); break;
	case 'i': insurance(who); return(TRUE); break;
	case 'q': leave(); return(TRUE); break;
	case 's': switch(str[1]) {
		case 'a': savegame(); exit(1); break;
		case 'p': split(who); return(TRUE); break;
		case 't':
		    if ((tmp = players[who].no_hands) > 1) {
			printf("Which hand? (1 - %d): ", tmp);
			scanf("%d", &tmp);
			tmp--;
			while (getchar() != NEWLINE);
	if (players[who].double_down[tmp] && players[who].dd_took_3rd_card[tmp] == FALSE) {
	printf("You must take another card (double down on hand #%d).\n", tmp+1);
	    printf("Drawing now...\n");
	    hit(who);
	    }
			stand(who, tmp);
			}
			else stand(who, 0);
		    return(TRUE);
		    break;
		case 'u': surrender(who); return(TRUE); break;
		default: printf("%s - bad choice\n", str); return(TRUE);break;
		}
	    break;
	case 'u': update(); return(TRUE); break;
	case '?': help(); return(TRUE); break;
	default: printf("%s - bad choice\n", str); return(TRUE); break;
	} /* End switch */
    }

/* The player takes "insurance" */
insurance(who) {
    int i, flag, deck[52];

    if (insurance_played) {
	printf("Insurance has already been taken for this round.\n");
	return;
	}
    flag = FALSE;
    for (i = 1; players[HOUSE].hand[0][i] != ENDOFDECK; i++)
	if (pack[players[HOUSE].hand[0][i]][0] == 'A')
	    flag = TRUE;
    if (flag == FALSE) {
	printf("Dealer does not show an ace.\n");
	return;
	}
    deck[0] = players[HOUSE].hand[0][0]; deck[1] = ENDOFDECK;
    printf("Dealer's down card was %s\n", pack[deck[0]]);
    if (card_count(deck) == 10) {
	if (who == USER) {
	    printf("%s is paid $%ld.00\n", user_name, players[USER].original_bet / 2);
	    amount_user_won += players[USER].original_bet;
	    }
	    else printf("Player #%d is paid $%ld.00\n", players[who].original_bet/2);
	players[who].bank_roll += players[who].original_bet / 2.0;
	}
    else {
	if (who == USER) {
	    printf("%s pays house $%ld.00\n", user_name, players[USER].original_bet/2);
	    amount_user_lost += players[USER].original_bet;
	    }
  else printf("Player #%d pays house $%ld.00\n", players[who].original_bet/2);
	players[who].bank_roll -= players[who].original_bet / 2.0;
	}
    insurance_played = TRUE;
    }

/* The player takes another card */
hit(who)
{
    int i, j, k, tmp;
    char line[100], response[100];

    switch(who) {
	case USER:
	    if (players[USER].no_hands > 1) {
		printf("Which hand? (1 - %d)? ", players[USER].no_hands);
		getline(line); tolower(line);
		sscanf(line, "%d", &i);
		i--;
		}
	    else i = 0;
	    tmp = card_count(players[USER].hand[i]);
	    if (players[USER].stand[i] == FALSE) take_card(USER, i);
		else printf("You cannot take a card - you are standing.\n");
	    if (players[USER].no_hands == 1) printf("%s: ", user_name);
		else printf("%s, hand #%d: ", user_name, i+1);
	    for (k = 0; players[USER].hand[i][k] != ENDOFDECK; k++) {
		printf("%s, ", pack[players[USER].hand[i][k]]);
		}
	    printf("\010\010 \n");
	    break;
	case HOUSE:
	    tmp = card_count(players[HOUSE].hand[0]);
	    if (tmp < 17) {
		take_card(HOUSE, 0);
		}
	    else {
		if (players[HOUSE].stand[0] == FALSE) stand(HOUSE, 0);
		}
	    break;
	default:  /* Other players */
	    for (i = 0; i < players[who].no_hands; i++) {
		tmp = card_count(players[who].hand[i]);
		if (tmp < 17) {
		    take_card(who, i);
		    }
		else {
		    if (players[who].stand[i] == FALSE) stand(who, i);
		    }
		}
	    break;
	} /* End switch */
    }

/* The player does "double down" */
double_down(who) {
    int i;
    char line[100];

    if (players[USER].no_hands > 1) {
	printf("Which hand? (1 - %d)? ", players[USER].no_hands);
	getline(line); tolower(line);
	sscanf(line, "%d", &i);
	i--;
	}
    else i = 0;
    if (players[who].double_down[i]) {
	printf("Can only double down once per hand.\n");
	return;
	}
    if (cards(players[who].hand[i]) != 2) {
	printf("Player can only double down on the first 2 cards.\n");
	return;
	}
#ifdef NODOUBLESPLIT
    if (players[who].no_hands > 1) {
	printf("Can't double down after splitting.\n");
	return;
	}
#endif
    players[who].dd_took_3rd_card[i] = FALSE;
    players[who].bank_roll -= players[who].bet[0];
    players[who].bet[0] += players[who].bet[0];
    if (who == USER) printf("%s's ", user_name);
	else printf("Player #%d's ", who);
    printf("bet is doubled to $%ld.00\n", players[who].bet[0]);
    players[who].double_down[i] = TRUE;
    }

/* Give the user a summary of the game */
update() {
    int i;

    printf("%d decks, %d players, %2.0f%% of the deck remains.\n",
	no_decks, no_players-1, 100*(52.0*no_decks - deck_ptr)/(52.0*no_decks));
    printf("Bankrolls -\t%s: $%.2f\n", user_name, players[USER].bank_roll);
    for (i = 2; i < no_players; i++)
	printf("\t\tPlayer #%d: $%.2f\n", i, players[i].bank_roll);
    printf("%s's beginning bankroll was: $%d.00\n", user_name, bank_roll);
    if (amount_user_won > amount_user_lost) {
    printf("%s's betting average: %2.2f%%\n", user_name, ((amount_user_won
	- amount_user_lost) / (amount_user_lost + amount_user_won)) * 100.0);
	}
    else {
    printf("%s's betting average: %2.2f%%\n", user_name, ((amount_user_lost -
	amount_user_won) / (amount_user_won + amount_user_lost)) * 100.0);
	}
    printf("Minimum bet: $%d.00\n", min_bet);
    if (players[USER].no_hands == 1)
	printf("%s's current bet: ", user_name);
	else printf("%s's current bets: ", user_name);
    for (i = 0; i < players[USER].no_hands; i++)
	printf("$%ld.00  ", players[USER].bet[i]);
    printf("\n");
    print_hands(HIDEDOWNCARDS);
    printf("%d hands have been played.\n", no_hands_played);
    }

/* Give some helpful information to the confused */
help() {
    printf("\tCASINO BLACKJACK DESCRIPTION OF OPTIONS\n");
    printf("For each of the commands, type in the characters enclosed in\n");
    printf("parentheses.\n\n");
    printf("[?]\t\t- prints this message.\n");
    printf("[h]it\t\t- draw another card.\n");
    printf("[q]uit\t\t- exit game (with save).\n");
    printf("[sp]lit\t\t- split a pair into 2 hands.\n");
    printf("[st]and\t\t- when finished drawing cards.\n");
    printf("[d]ouble\t- double your bet on the first 2 cards of a hand.\n");
    printf("[u]pdate\t- give an update of the games's progress.\n");
    printf("[sa]ve game\t- same as quit.\n");
    printf("[c]ontinue\t- when player is satisfied with the hand(s) and bet(s).\n");
    printf("[i]nsurance\t- bet half the original bet that dealer has blackjack\n");
    printf("\t\t  on the first 2 cards.\n");
    printf("[su]rrender\t- discontinue hand and forfeit half your bet.\n");
    printf("del\t\t- (delete key) immediate exit (with save).\n");
    printf("\n");
    }

/* The player surrenders half of his bet */
surrender(who) {
    int i;

    switch(who) {
	case USER:
	    printf("%s surrenders $%ld.00\n", user_name, players[who].bet[0]/2);
	    break;
	default:
	    printf("Player #%d surrenders $%ld.00\n", who, players[who].bet[0]/2);
	    break;
	}
    for (i = 2; i <= cards(players[who].hand[0]); i++)
	players[who].hand[0][i-2] = players[who].hand[0][i];
    players[who].bank_roll += players[who].bet[0] / 2.0;
    if (who == USER)
	amount_user_lost += players[USER].bet[0] / 2;
    players[who].bet[0] = 0;
    players[who].stand[0] = TRUE;
    }

/* The player splits 2 like cards into two hands */
split(who) {
    int i, k, tmp;
    char line[100];

    if (players[USER].no_hands > 1) {
	printf("Which hand? (1 - %d)? ", players[USER].no_hands);
	getline(line); tolower(line);
	sscanf(line, "%d", &k);
	k--;
	}
    else k = 0;

    if (players[who].double_down[k]) {
	printf("Can't split when double downed.\n");
	return;
	}

    if (players[who].no_hands >= MAXSPLITS) {
	printf("Only %d splits allowed per hand.\n", MAXSPLITS);
	return;
	}

    for (i = 0; i < players[who].no_hands; i++) {
	if (cards(players[who].hand[i]) == 2 && pack[players[who].hand[i][0]][0] ==
	    pack[players[who].hand[i][1]][0]) {
	    tmp = players[who].no_hands;
	    players[who].hand[tmp][0] = players[who].hand[tmp-1][1];
	    players[who].hand[tmp-1][1] = ENDOFDECK;
	    players[who].hand[tmp][1] = ENDOFDECK;
	    players[who].bet[tmp] = players[who].original_bet;
	    players[who].bank_roll -= players[who].original_bet;
	    players[who].no_hands++;
	    if (who == USER) {
		printf("You now have %d hands.\n", players[who].no_hands);
		}
	    return;
	    }
	}
    if (players[who].no_hands == 1)
	printf("Your hand cannot be split.\n");
    else if (players[who].no_hands == 2)
	printf("Neither of your hands can be split.\n");
    else printf("None of your hands can be split.\n");
    }

/* Return the number of cards in the hand */
cards(hand)
int hand[];
{
    int i;

    for (i = 0; hand[i] != ENDOFDECK; i++);
    return(i);
    }


/* Save the game */
savegame()
{
    int i, j, k, fp;
    char filename[100], response[100];

    printf("File? ");
    scanf("%s", filename);
    while (getchar() != NEWLINE);
    if ((fp = creat(filename, 0644)) < 0) {
	printf("Cannot creat file: %s - Try again? ", filename);
	scanf("%s", response); tolower(response);
	while (getchar() != NEWLINE);
	if (response[0] == 'y') return(TRUE);
	else return(FALSE);
	}

    write(fp, &no_decks, sizeof no_decks);
    write(fp, &no_players, sizeof no_players);
    write(fp, &bank_roll, sizeof bank_roll);
    write(fp, &deck_ptr, sizeof deck_ptr);
    write(fp, &min_bet, sizeof min_bet);
    write(fp, &insurance_played, sizeof insurance_played);
    write(fp, &no_hands_played, sizeof no_hands_played);
    write(fp, &amount_user_won, sizeof amount_user_won);
    write(fp, &amount_user_lost, sizeof amount_user_lost);
    write(fp, &uston_count, sizeof uston_count);
    write(fp, &uston_countp, sizeof uston_countp);

    for (i = 0; i < no_players; i++) {
	write(fp, &players[i].bank_roll, sizeof players[i].bank_roll);
	write(fp, &players[i].no_hands, sizeof players[i].no_hands);
	for (j = 0; j < MAXSPLITS; j++)
	    for (k = 0; k < MAXSIZEOFHAND; k++)
		write(fp, &players[i].hand[j][k], sizeof players[i].hand[j][k]);
	for (j = 0; j < MAXSPLITS; j++)
	    write(fp, &players[i].bet[j], sizeof players[i].bet[j]);
	write(fp, &players[i].broke, sizeof players[i].broke);
	for (j = 0; j < MAXSPLITS; j++)
	    write(fp, &players[i].bust[j], sizeof players[i].bust[j]);
	write(fp, &players[i].insurance, sizeof players[i].insurance);
	write(fp, &players[i].surrender, sizeof players[i].surrender);
	for (j = 0; j < MAXSPLITS; j++)
	    write(fp, &players[i].stand[j], sizeof players[i].stand[j]);
	write(fp, &players[i].credit, sizeof players[i].credit);
	write(fp, &players[i].original_bet, sizeof players[i].original_bet);
	for (j = 0; j < MAXSPLITS; j++)
	    write(fp, &players[i].double_down[j], sizeof players[i].double_down[j]);
	for (j = 0; j < MAXSPLITS; j++)
	    write(fp, &players[i].dd_took_3rd_card[j], sizeof players[i].dd_took_3rd_card[j]);
	} /* End "i" loop */
    for (i = 0; i < 52*MAXDECKS; i++)
	write(fp, &deck[i], sizeof deck[i]);

    close(fp);
    printf("Game saved in file: %s\n", filename);
    return(FALSE);
    } /* End savegame() */


/* Restore a saved game */
cont_game(filename)
char filename[];
{
    int i, j, k, fp;
    char response[100];

    if ((fp = open(filename, 0)) < 0) {
	printf("Cannot open file: %s\n", filename);
	error(1);
	}

    read(fp, &no_decks, sizeof no_decks);
    read(fp, &no_players, sizeof no_players);
    read(fp, &bank_roll, sizeof bank_roll);
    read(fp, &deck_ptr, sizeof deck_ptr);
    read(fp, &min_bet, sizeof min_bet);
    read(fp, &insurance_played, sizeof insurance_played);
    read(fp, &no_hands_played, sizeof no_hands_played);
    read(fp, &amount_user_won, sizeof amount_user_won);
    read(fp, &amount_user_lost, sizeof amount_user_lost);
    read(fp, &uston_count, sizeof uston_count);
    read(fp, &uston_countp, sizeof uston_countp);

    for (i = 0; i < no_players; i++) {
	read(fp, &players[i].bank_roll, sizeof players[i].bank_roll);
	read(fp, &players[i].no_hands, sizeof players[i].no_hands);
	for (j = 0; j < MAXSPLITS; j++)
	    for (k = 0; k < MAXSIZEOFHAND; k++)
		read(fp, &players[i].hand[j][k], sizeof players[i].hand[j][k]);
	for (j = 0; j < MAXSPLITS; j++)
	    read(fp, &players[i].bet[j], sizeof players[i].bet[j]);
	read(fp, &players[i].broke, sizeof players[i].broke);
	for (j = 0; j < MAXSPLITS; j++)
	    read(fp, &players[i].bust[j], sizeof players[i].bust[j]);
	read(fp, &players[i].insurance, sizeof players[i].insurance);
	read(fp, &players[i].surrender, sizeof players[i].surrender);
	for (j = 0; j < MAXSPLITS; j++)
	    read(fp, &players[i].stand[j], sizeof players[i].stand[j]);
	read(fp, &players[i].credit, sizeof players[i].credit);
	read(fp, &players[i].original_bet, sizeof players[i].original_bet);
	for (j = 0; j < MAXSPLITS; j++)
	    read(fp, &players[i].double_down[j], sizeof players[i].double_down[j]);
	for (j = 0; j < MAXSPLITS; j++)
	    read(fp, &players[i].dd_took_3rd_card[j], sizeof players[i].dd_took_3rd_card[j]);
	} /* End "i" loop */
    for (i = 0; i < 52*MAXDECKS; i++)
	read(fp, &deck[i], sizeof deck[i]);

    close(fp);
    printf("Game restored from file: %s\n", filename);
    } /* End cont_game() */


/* Return TRUE if the hand is a blackjack (21 points). ow return FALSE */
blackjack(hand)
int hand[];
{
    if (card_count(hand) == 21) return(TRUE);
	else return(FALSE);
    }


not(value)
{
    if (value) return(FALSE);
	else return(TRUE);
    }

is_a_number(str)
char str[];
{
    int i;

    if (sscanf(str, "%d", &i) > 0) return(TRUE);
	else return(FALSE);
    }

uston(card)
int card;
{
    switch(pack[card][0]) {
	case '1':
	case 'A': uston_count--; break;
	case '3':
	case '4':
	case '5':
	case '6':
	case '7': uston_count++; break;
	default: break;
	}
    }
