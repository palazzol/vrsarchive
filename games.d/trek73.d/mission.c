/*
 * TREK73: mission.c
 *
 * Mission Assignment Routines
 *
 * mission, alert, missionlog
 */

#ifdef BSD
#include <sys/time.h>
#endif
#ifdef SYSV
#include <time.h>
#endif

#include "externs.h"


mission()
{
	int onef;
	char temp[3];

	if (terse)
		return;
	onef = (shipnum == 1);
	puts("\n\n\nSpace, the final frontier.");
	printf("These are the voyages of the starship %s.\n", shiplist[0]->name);
	puts("Its five year mission: to explore strange new worlds,");
	puts("to seek out new life and new civilizations,");
	puts("to boldly go where no man has gone before!");
	puts("\n                    S T A R    T R E K\n");
	missionlog();
	if (onef)
		strcpy(temp, "a");
	else
		sprintf(temp,"%d", shipnum);
	printf("%s:  %s, I'm picking up %s vessel%s on an interception\n",
	    helmsman, title, temp, plural(shipnum));
	printf("   course with the %s.\n", shiplist[0]->name);
	printf("%s:  Sensors identify %s as ", science, onef ? "it" : "them");
	if (onef)
		printf("a%s ", vowelstr(foerace));
	printf("%s %s%s,\n", foerace, foestype, plural(shipnum));
	printf("   probably under the command of Captain %s.\n", foename);
	printf("%s:  Sound general quarters, Lieutenant!\n", captain);
	printf("%s:  Aye, %s!\n", com,  title);
}

alert()
{
	register int i;

	printf("Computer: The %ss are attacking the %s with the ",
	    foerace, shiplist[0]->name);
	if (shipnum == 1) {
		printf("%s", shiplist[1]->name);
	} else {
		for (i = 1; i <= shipnum; i++) {
			if (i == shipnum)
				printf("and the ");
			printf("%s", shiplist[i]->name);
			if (i == shipnum)
				continue;
			printf(", ");
			if (i == 1 || i == 6)
				printf("\n   ");
		}
	}
	printf(".\n");
}

missionlog()
{
	static char *missiontab[] = {

	/* "The Trouble with Tribbles" */
	"   We are acting in response to a Priority 1 distress call from",
	"space station K7.",

	/* "The Gamesters of Triskelion" */
	"   We are orbiting Gamma 2 to make a routine check of automatic",
	"communications and astrogation stations.",

	/* "Metamorphosis" */
	"   We are on course for Epsilon Canares 3 to treat Commissioner",
	"Headford for Sukaro's disease.",

	/* "Journey to Babel" */
	"   We have been assigned to transport ambassadors to a diplomatic",
	"conference on the planet code named Babel.",

	/* ?? */
	"   Our mission is to investigate a find of tritanium on Beta 7.",
	0,

	/* "Shore Leave" */
	"   We are orbiting Rigel 4 for therapeutic shore leave.",
	0,

	/* "A Piece of the Action" */
	"   We are orbiting Sigma Iota 2 to study the effects of",
	"contamination upon a developing culture.",

	/* "The Immunity Syndrome" */
	"   We have altered course for a resue mission on the Gamma 7A",
	"system.",

	/* "Amok Time" */
	"   We are presently on course for Altair 6 to attend inauguration",
	"cermonies on the planet.",

	/* "Who Mourns for Adonis?" */
	"   We are on a cartographic mission to Pollux 9.",
	0,

	/* "The Changeling" */
	"   We are headed for Malurian in response to a distress call",
	"from that system.",

	/* "Mirror, Mirror" */
	"   We are to negotiate a treaty to mine dilithium crystals from",
	"the Halkans.",

	/* "The Apple" */
	"   We are to investigate strange sensor readings reported by a",
	"scoutship investigating Gamma Triangula 6.",

	/* "The Doomsday Machine" */
	"   We are headed for planets L370 and L374 to investigate the",
	"disappearance of the starship Constellation in that vincinity.",

	/* "The Ultimate Computer" */
	"   We are ordered, with a skeleton crew, to proceed to Space",
	"Station K2 to test Dr. Richard Daystrom's computer M5.",

	/* "Bread and Circuses" */
	"   We have encountered debris from the SS Beagle and are",
	"proceeding to investigate.",

	/* "Patterns of Force" */
	"   We are on course for Ekos to locate John Gill.",
	0,

	/* "The Paradise Syndrome" */
	"   We are to divert an asteroid from destroying an inhabited",
	"planet.",

	/* "And The Children Shall Lead" */
	"   We are responding to a distresss call form the scientific",
	"expedition on Triacus.",

	/* "Is There in Truth No Beauty?" */
	"   We have been assigned to transport the Medusan Ambassador to",
	"to his home planet.",

	/* "Star Trek II -- The Wrath of Khan" */
	"   We are within the Neutral Zone on a mission to rescue the",
	"Kobayashi Maru.",

	};
	long t1;
	struct tm *localtime(), *date;

	t1 = time(0);
	date = localtime(&t1);
	printf("%s:  Captain's log, stardate %02d%02d.%02d\n",
	    captain, date->tm_year, date->tm_mon+1, date->tm_mday);
	t1 = (randm(sizeof missiontab / sizeof (char *)) - 1) & ~01;
	puts(missiontab[t1]);
	if (missiontab[++t1])
		printf("   %s\n", missiontab[t1]);
}
