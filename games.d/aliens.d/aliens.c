/*
 *	Aliens -- A satire of human conflict with the invaders
*/
#include <stdio.h>
#define	MAXCHAR 20

char s[MAXCHAR];	/*  character command string  */
long hkill, hwound, humans, aliens, humwnd;

prompt()		/*  command prompt  */
{
	printf("\nYour command, sir:\t");
	return;
}

unrec(n)		/*  unrecognized command  */
char n;
{
char c;
	if(n != '?')	{
	printf("That is indisputably an illegal command.\n");
	printf("Do you wish a list of commands?\t");
	(void) scanf("%s", &c);
	}
	else	c = 'y';
	if(c == 'y')	{
		printf("Presently implemented commands:\n\n");
		printf("d\tDynamite\t\tf\tFlak Guns\n");
		printf("h\tHandguns\t\tj\tJet Fighter Planes\n");
		printf("l\tLaser Cannon\t\tn\tNuclear Missiles\n");
		printf("o\tObscene Gestures\tp\tPunk Rock\n");
		printf("r\tRocks\t\t\tt\tTire Irons\n");
		printf("M\tMedic\t\t\ts\tSecret Weapon\n");
		printf("Trap\tTrap\t\t\tTreaty\tTreaty\n");
		printf("D\tDivine Intervention\n");
		printf("?\tList all commands\n\n");
		}
	return;
}

long rocks(n)
long n;
{
	extern long hkill, hwound, aliens, humans;	
	static long prob[5] ={35, 65, 77, 92, 100};
	long akill;
	long i, j;

	akill = 0;
	printf("Your men throw rocks at the aliens.\n");
	i = dice(100) + 10 * n + modd();
	for( j = 0; prob[j] < i; j++ );
	switch(j)	{
	
		case 0:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("The aliens are surprised by your unorthodox tactics.\n");
	printf("They laugh so hard they soil their pantaloons.\n");
	printf("This causes their suits' environmental ");
	printf("control to go haywire.\n");
	printf("%ld aliens are killed by poor toilet discipline.\n",akill);
		break;

		case 1:
	akill = 0;
	printf("The aliens pick up the rocks and wonder what they are.\n");
	printf("One of the aliens picks up a rock and chews ");
	printf("on it speculatively.\n");
	printf("He gabbles in alienese and soon all the aliens ");
	printf("are wolfing down rocks.\n");
	printf("It seems that your rocks are the best ");
	printf("things they've ever tasted.\n");
	printf("No aliens are killed by your attack, but a ");
	printf("few of them get tummyaches.\n");
		break;

		case 2:
	akill = 0;
	hwound = dice(range(humans));
	if(hwound > humans) hwound = humans;
	printf("The aliens have set up a defensive repulsifier ");
	printf("around themselves.\n");
	printf("When your rocks strike the repulsifier field, ");
	printf("they bounce right back.\n");
	printf("In fact, they bounce back at twice the speed ");
	printf("you threw them.\n");
	printf("%ld humans are wounded by thrown rocks.\n",hwound);
		break;

		case 3:
	akill = 0;
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("The aliens are angered by your thrown rocks.\n");
	printf("They use their riot-control troops and ");
	printf("tear gas on your men.\n");
	printf("The aliens' tear gas proves to be rather toxic to humans.\n");
	printf("%ld humans are killed by Philadelphia Police Dept. tactics.\n",
		hkill);
		break;
		
		default:
	akill = dice(range(humans - aliens)) + dice(range(humans));
	printf("The aliens are gratified by the reception you give them.\n");
	printf("To acknowledge your courtesy, they call in ");
	printf("a full parade band.\n");
	printf("The band is armed for full combat.\n");
	printf("%ld aliens have joined the expeditionary force.\n", akill);
	akill = - akill;
		break;
	}
	return(akill);
}
long medic()
{
	static long count = 0;
	extern long humwnd, humans, aliens;
	long i;
	printf("You call in the M*A*S*H and their crew of excellent,");
	printf(" humanitarian doctors.\n");
	printf("They do not like the idea of sending wounded men back");
	printf(" to the front.\n");
	printf("You order them to patch up the wounded and never mind");
	printf(" what they like.\n");
	if(count < 3)	{
		if(aliens > humans)	{
			i = - humwnd;
			printf("The M*A*S*H doctors patch up your");
			printf(" wounded and they return to the front.\n");
			count++;
			if(count == 3)	{
				printf("They also say that this is the");
				printf(" last time they'll help you.\n");
				printf("Next time, they'll make sure you");
				printf(" can\'t get any more men wounded.\n");
				}
		}
		else	{
			i = 0;
			printf("The M*A*S*H doctors know that able-bodied");
			printf(" humans outnumber the aliens.\n");
			printf("Therefore, you are not in desperate need");
			printf(" of men.\n");
			printf("The M*A*S*H doctors flatly refuse to send");
			printf(" the wounded back to the front.\n");
			printf("Your wounded remain in their beds.\n");
		}
	}
	else	{
		printf("The M*A*S*H doctors weren't kidding.\n");
		printf("They refuse to even consider sending the wounded");
		printf(" back to the front.\n");
		printf("Instead, they give you a psychiatric examination,");
		printf(" which you fail.\n");
		printf("You are committed to a mental hospital.\n");
		printf("Without your leadership, your army soon falls");
		printf(" apart...............\n");
		i = humans;
		}
	return(i);
}

long handguns(n)
long n;
{
	extern long hkill, hwound, aliens, humans;
	static long prob[5] ={45, 65, 90, 100};
	long akill;
	long i, j;
	
	akill = 0;
	printf("Your men shoot handguns at the aliens.\n");
	i = dice(100) + 10 * n + modd();
	for( j = 0; prob[j] < i; j++) ;
	switch(j)	{

		case 0:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("The aliens are surprised by your attack and retreat.\n");
	printf("Your men shoot several aliens during their ");
	printf("hasty withdrawal.\n");
	printf("%ld aliens are killed by your handguns.\n",akill);
		break;

		case 1:
	akill = 0;
	hwound = dice(range(humans / 2));
	if(hwound > humans) hwound = humans;
	printf("Your men greatly enjoy old western movies.\n");
	printf("Their handguns let them act out these movies realistically.\n");
	printf("In fact, they start having shootouts just like ");
	printf("Matt Dillon used to.\n");
	printf("%ld humans are wounded outside of Dodge City.\n",hwound);
		break;

		case 2:
	akill = 0;
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("Your men decide that shooting at aliens is boring.\n");
	printf("They drive down to the local liqour store and demand money.\n");
	printf("After stealing $20, they are arrested in a brothel outside ");
	printf("of town.\n");
	printf("%ld humans are locked up and lost to your army.\n",hkill);
		break;

		default:
	akill = dice(range(humans));
	printf("The aliens are alarmed that such simple ");
	printf("and common weapons work.\n");
	printf("They realize that the entire population of ");
	printf("Earth can be armed this way.\n");
	printf("The alien commander hurriedly calls back home ");
	printf("for reinforcements.\n");
	printf("%ld additional aliens arrive on Earth ",akill);
	printf("to counter your cheap handguns.\n");
	akill = - akill;
		break;
	}
	return(akill);
}
long tires(n)
long n;
{
	extern long hkill, hwound, aliens, humans;
	static long prob[4] ={50, 60, 85, 100};
	long akill;
	long i, j;

	akill = 0;
	printf("Your men attack the aliens with tire irons.\n");
	i = dice(100) + 10 * n + modd();
	for(j = 0; prob[j] < i; j++);
	switch(j)	{
	
		case 0:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("No aliens are killed by this, but their ");
	printf("helmets are cracked.\n");
	printf("The cracked helmets let air into the aliens' suits.\n");
	printf("Our air is poisonous to the aliens and the ");
	printf("aliens choke and die.\n");
	printf("%ld aliens are killed by two things ", akill);
	printf("needed to fix a flat tire.\n");
		break;

		case 1:
	akill = 0;
	hwound = dice(range(humans) / 2);
	if(hwound > humans) hwound = humans;
	printf("Your men are inexperienced in the use of hoodlum weapons.\n");
	printf("They end up trying to figure out how to use ");
	printf("them on each other.\n");
	printf("They become rather proficient in the use of tire irons.\n");
	printf("%ld humans are wounded by lack of streetwisdom.\n",hwound);
		break;

		case 2:
	akill = 0;
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("Your men cannot believe the idiotic weapons ");
	printf("that they now have.\n");
	printf("Most of your men refuse to advance into the attack.\n");
	printf("The remaining brave men are greatly outnumbered ");
	printf("and are overwhelmed.\n");
	printf("%ld humans are killed by unquestioning ",hkill);
	printf("obedience to dumb orders.\n");
		break;

		default:
	akill = dice(range(humans - aliens)) + dice(range(humans / 2));
	printf("Your tire iron attacks prove to be highly effective.\n");
	printf("The aliens' commander is alarmed and requests ");
	printf("special reinforcements.\n");
	printf("These reinforcements have been trained to fight dirty.\n");
	printf("%ld aliens join the expeditionary ", akill);
	printf("force ready to fight like men.\n");
	akill = - akill;
		break;
	}
	return(akill);
}

long lasers(n)
long n;
{
	extern long hkill,hwound;
	static long prob[5] ={30, 60, 70, 90, 100};
	long akill;
	long i, j;

	akill = 0;
	printf("Your men fire high-powered laser cannon at the aliens.\n");
	i = dice(100) + 10 * n + modd();
	for(j = 0; prob[j] < i; j++);
	switch(j)	{

		case 0:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("The laser cannons turn out to be extremely effective.\n");
	printf("You vaporize several of the aliens' landing craft.\n");
	printf("The aliens retreat temporarily to orbit ");
	printf("without counterattacking.\n");
	printf("%ld aliens are vaporized in their ships.\n", akill);
		break;

		case 1:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("Your laser cannons vaporize several alien ships.\n");
	printf("The aliens counterattack in rage using their ");
	printf("own laser cannons.\n");
	printf("Both sides take heavy casualties.\n");
	printf("%ld humans and %ld aliens are ", hkill, akill);
	printf("killed in the battle.\n");
		break;

		case 2:
	hwound = dice(range(humans / 3));
	if(hwound > humans) hwound = humans;
	printf("Your laser cannons do not work exactly as designed.\n");
	printf("They overheat considerably, inflicting serious ");
	printf("burns to your men.\n");
	printf("%ld humans are wounded by fundamental design errors.\n",hwound);
	akill = 0;
		break;

		case 3:
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("The aliens think you are trying to signal them ");
	printf("with searchlights.\n");
	printf("They activate their lights to send signals back.\n");
	printf("Their lights turn out to be even deadlier ");
	printf("than your laser cannon.\n");
	printf("%ld humans are vaporized by aliens saying \"Hi!\"\n",hkill);
	akill = 0;
		break;

		default:
	akill = dice(range(aliens));
	printf("The aliens notice your powerful light sources.\n");
	printf("Many of the aliens strip off their suits to ");
	printf("sunbathe in them.\n");
	printf("This precipitates an orgy amongst the nude ");
	printf("aliens, causing them to...\n");
	printf("\t..... REPRODUCE!!! .....\n");
	printf("%ld new aliens are hatched and join ", akill);
	printf("the expeditionary force.\n");
	akill = - akill;
		break;
	}
	return(akill);
}
long nukes(n)
long n;
{
	extern long hkill, aliens, humans;
	static long prob[5] ={25, 45, 65, 80, 100};
	long akill;
	long i, j;

	akill = 0;
	printf("Your men launch nuclear missiles at the aliens.\n");
	i = dice(100) + n * 10 + modd();
	for(j = 0; prob[j] < i; j++) ;
	switch(j)	{

		case 0:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("The nuclear attacks hit the aliens hard.\n");
	printf("Many aliens are killed in the attack.\n");
	printf("The aliens retreat to regroup and do not counterattack.\n");
	printf("%ld aliens are nuked out of existence.\n", akill);
		break;

		case 1:
	printf("Your nuclear weapons all date back to the 1970's.\n");
	printf("None of them work especially well.\n");
	printf("All of them are duds.\n");
	printf("1 alien is killed accidentally when an unexploded bomb ");
	printf("falls on him.\n");
	akill = 1;
		break;
		
		case 2:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("The aliens are hit hard by your nuclear missiles.\n");
	printf("They are forced to launch several nuclear weapons o");
	printf("f their own.\n");
	printf("Both humans and aliens suffer heavy casualties.\n");
	printf("%ld humans and %ld aliens get nuked ",hkill, akill);
	printf("out of existence.\n");
		break;

		case 3:
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("Some of the missiles have 1960's-vintage guidance systems.\n");
	printf("These missiles insist on hitting the targets they ");
	printf("were given in 1962.\n");
	printf("Many of your men are staying in these places.\n");
	printf("%ld humans are nuked by old political hatreds.\n",hkill);
		break;

		default:
	akill = dice(range(humans));
	printf("The intense radiation from the nuclear blasts affects ");
	printf("some humans.\n");
	printf("The radiation causes them to ............ MUTATE!!!!\n");
	printf("The mutants decide they like the aliens more than ");
	printf("they like you.\n");
	printf("%ld mutants enlist in the alien expeditionary force.\n",akill);
	hkill = akill;
	if(hkill > humans) hkill = humans;
	akill = - hkill;
		break;
	}
	return(akill);
}
long obscene(n)
long n;
{
	extern long hkill, hwound, humans, aliens;
	long akill;
	static long prob[6] ={20, 40, 60, 75, 90, 100};
	long i, j;

	akill = 0;
	printf("Your men make obscene gestures at the aliens.\n");
	i = dice(100) + n * 12 + modd();
	for(j = 0; prob[j] < i; j++) ;
	switch(j)	{

		case 0:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("The aliens are appalled at your disrespect for them.\n");
	printf("Many decide that they do not want to associate with ");
	printf("such vulgar beings.\n");
	printf("They get back into their ships and go back to where ");
	printf("they came from.\n");
	printf("%ld aliens are offended right off the face of the Earth.\n",
			akill);
		break;

		case 1:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("The aliens are confused by your peculiar contortions.\n");
	printf("Some of the more puzzled aliens try to make ");
	printf("the gestures themselves.\n");
	printf("The aliens' skeletons are not made to perform such motions.\n");
	printf("These aliens hurt themselves and have to be sent ");
	printf("home to recover.\n");
	printf("%ld aliens depart because of sprained middle tentacles.\n",
			akill);
		break;

		case 2:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("Your obscene gestures have deep religious significance ");
	printf("to the aliens.\n");
	printf("Many of them become convinced that humans are ");
	printf("sacred beings.\n");
	printf("These aliens apply for conscientious objector status.\n");
	printf("%ld aliens are shipped back home to be de-programmed.\n",akill);
		break;

		case 3:
	hwound = dice(range(humans / 2));
	if(hwound > humans) hwound = humans;
	printf("Some of your men also make obscene gestures at each other.\n");
	printf("This precipitates fighting among your men.\n");
	printf("The aliens are greatly amused by this show of human ");
	printf("solidarity.\n");
	printf("%ld humans are wounded by fighting amongst themselves.\n",
			hwound);
		break;

		case 4:
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("The aliens make obscene gestures back.\n");
	printf("The aliens' obscene gestures turn out to be highly comical.\n");
	printf("They are so comical that several of your men die laughing.\n");
	printf("%ld humans are killed by dirty-mindedness.\n",hkill);
		break;

		default:
	akill = dice(range(humans));
	printf("The aliens decide to take the advice of your men's obscene");
	printf(" gestures.\n");
	printf("This causes them to ..................... REPRODUCE!!!!!\n");
	printf("%ld more aliens are added to the expeditioary force.\n",akill);
	akill = - akill;
		break;
	}
	return(akill);
}

long flak(n)
long n;
{
	extern long hkill, hwound, humans, aliens;
	static long prob[5] ={40, 70, 78, 90, 100};
	long akill;
	long i, j;

	akill = 0;
	printf("Your men fire flak guns at the aliens.\n");
	i = dice(100) + 10 * n + modd();
	for(j = 0; prob[j] < i; j++) ;
	switch(j)	{

		case 0:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("The flak guns prove to be very effective against ");
	printf("the aliens.\nThe aliens suffer heavy losses and ");
	printf("retreat without counterattacking.\n");
	printf("%ld aliens are killed by your anti-aircraft fire.\n",akill);
		break;

		case 1:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("The aliens suffer some casualties from your flak guns.\n");
	printf("The aliens decide to counterattack.\n");
	printf("The aliens bomb your flak gun emplacements.\n");
	printf("%ld aliens and %ld humans are killed ",akill, hkill);
	printf("in the exchange.\n");
		break;

		case 2:
	akill = 0;
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("Your men accidentally load the wrong ammunition ");
	printf("into the flak guns.\nThey put regular shells in the ");
	printf("guns instead of proximity-fuzed shells.\n");
	printf("These shells sail harmlessly past the aliens\' ships ");
	printf("when fired.\nThey land much less harmlessly among ");
	printf("your other positions downrange.\n");
	printf("%ld humans are killed by gross ineptitude.\n", hkill);
		break;

		case 3:
	akill = 0;
	hwound = dice(range(humans / 3));
	if(hwound > humans) hwound = humans;
	hkill = dice(range(humans / 2));
	if(hkill > humans) hkill = humans;
	printf("Some of your flak guns are very old.\n");
	printf("These old guns suffer from severe metal fatigue.\n");
	printf("When your men fire these flak guns the barrels burst ");
	printf("and hurt you men.\n");
	printf("%ld humans are killed and %ld are ",hkill,hwound);
	printf("wounded by tired old guns.\n");
		break;

		default:
	akill = dice(range(humans));
	printf("Your flak guns shoot down several alien ships.\n");
	printf("The ships fall into the ocean, where the aliens bail out.\n");
	printf("Seawater turns out to be an aphrodisiac to the aliens.\n");
	printf("The aliens in the ocean ................. REPRODUCE!!!!!!!\n");
	printf("%ld aliens are hatched in the warm ocean.\n", akill);
	akill = - akill;
		break;
	}
	return(akill);
}

long punk(n)
long n;
{
	static long prob[5] ={25, 60, 75, 90, 100};
	extern long hwound, hkill, humans, aliens;
	long akill;
	long i, j;
	
	akill = 0;
	printf("Your men play loud punk rock music at the aliens.\n");
	i = dice(100) + 20 * n + modd();
	for(j = 0; prob[j] < i; j++) ;
	switch(j) {

		case 0:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("Many aliens cannot stand the noise of your punk rock.\n");
	printf("They pack up their things and throw them frantically ");
	printf("into their ships.\n");
	printf("They take off, never to be seen again.\n");
	printf("%ld aliens leave in search of a place ", akill);
	printf("with more class than Earth.\n");
		break;

		case 1:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("The loud punk rock music has no effect on the aliens.\n");
	printf("However, it does set up sympathetic resonances ");
	printf("in their suits.\n");
	printf("This causes their environment suits to go haywire ");
	printf("and cook the aliens.\n");
	printf("%ld aliens are killed because machines ",akill);
	printf("can't stand punk rock either.\n");
		break;

		case 2:
	akill = 0;
	hwound = dice(range(humans / 2));
	if(hwound > humans) hwound = humans;
	printf("Many of your men (understandably) cannot ");
	printf("stand the loud punk rock.\n");
	printf("They get mad and destroy the large speaker systems ");
	printf("playing the music.\n");
	printf("They also attack those men who do like the punk rock.\n");
	printf("%ld humans are wounded in a sudden revival ",hwound);
	printf("of good taste in music.\n");
		break;

		case 3:
	akill = 0;
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("The aliens find the sound of your punk rock irritating.\n");
	printf("They are angry and throw things at your men ");
	printf("to make them be quiet.\n");
	printf("Unfortunately, the things they throw are rather explosive.\n");
	printf("%ld humans are killed by aliens trying ", hkill);
	printf("to get some sleep.\n");
		break;

		default:
	akill = dice(range(humans));
	printf("The aliens really enjoy the loud punk rock music.\n");
	printf("The loud music excites them greatly, and they ");
	printf("really boogie down.\n");
	printf("It excites them so much that they have ");
	printf("an orgy and .....REPRODUCE!!!!\n");
	printf("%ld aliens are added to the expeditionary force.\n",akill);
	akill = - akill;
		break;
	}
	return(akill);
}
long jets(n)
long n;
{
	extern long humans, aliens, hkill, hwound;
	static long prob[4] ={25, 50, 75, 100};
	long akill;
	long i, j;

	akill = 0;
	printf("You send jet fighter planes up to attack the aliens.\n");
	i = dice(100) + n * 15 + modd();
	for(j = 0; prob[j] < i; j++) ;
	switch(j)	{

		case 0:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("The jet fighter planes catch the aliens by surprise.\n");
	printf("Several of the aliens' ships are shot down by ");
	printf("the fighter planes.\n");
	printf("The aliens retreat without taking action in retribution.\n");
	printf("%ld aliens are brought down in one-sided dogfights.\n",akill);
		break;

		case 1:
	printf("The aliens are prepared for your jet fighter attacks.\n");
	printf("Your fighter pilots take very heavy losses in the ");
	printf("air-to-air combat.\n");
	hwound = dice(range(humans / 2));
	if(hwound > humans) hwound = humans;
	printf("%ld humans are wounded while they get shot down.\n",hwound);
		break;

		case 2:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("The aliens suffer some casualties from your ");
	printf("jet fighter planes.\n");
	printf("They get mad and send attack craft to follow ");
	printf("your jets back to base.\n");
	printf("Once they find your bases, the aliens ");
	printf("bomb them unmercifully.\n");
	printf("%ld humans are killed by superior air tactics.\n",hkill);
		break;

		default:
	akill = dice(range(aliens));
	printf("Your jet fighter attacks frighten the aliens badly.\n");
	printf("The alien commander feels obligated to call home ");
	printf("for reinforcements.\n");
	printf("His request is granted, and %ld ",akill);
	printf("more alien troops arrive.\n");
	akill = - akill;
		break;
	}
	return(akill);
}
long dyna(n)
long n;
{
	extern long aliens, humans, hkill;
	static long prob[3] ={45, 80, 100};
	long akill;
	long i, j;

	akill = 0;
	printf("Your men throw dynamite at the aliens.\n");
	i = dice(100) + n * 10 + modd();
	for(j = 0; prob[j] < i; j++) ;
	switch(j)	{

		case 0:
	akill = dice(range(aliens));
	if(akill > aliens) akill = aliens;
	printf("As is often the case, the men in your army are ");
	printf("not very bright.\n");
	printf("They forget to light the fuses before ");
	printf("throwing the dynamite.\n");
	printf("The aliens are even less bright: they eat the dynamite.\n");
	printf("%ld aliens get sick from indigestion and ", akill);
	printf("must be sent home.\n");
		break;

		case 1:
	hkill = dice(range(humans));
	if(hkill > humans) hkill = humans;
	printf("As is often the case, the men in your army are ");
	printf("not very bright.\n");
	printf("They forget to light the fuses ");
	printf("before throwing the dynamite.\n");
	printf("The aliens correct this oversight by lighting ");
	printf("it and throwing it back.\n");
	printf("%ld humans are killed by stupidity ", hkill);
	printf("worthy of an old ethnic joke.\n");
		break;

		default:
	akill = dice(range(humans));
	printf("The dynamite blows the aliens into several pieces.\n");
	printf("However, the aliens are capable of reproducing asexually.\n");
	printf("This means that each of the pieces grows into a new alien.\n");
	printf("%ld new aliens are added by explosive reproduction.\n",akill);
	akill = - akill;
		break;
	}
	return(akill);
}
secret()
{
	extern long hkill, humans;

	printf("You decide to use an experimental, highly deadly,");
	printf(" secret weapon.\n");

	if(dice(100) > 50)	{
	printf("Professor Scheisskopf brings out his ...");
	printf("...... RNA DEPOLARIZER!!!!\n");
	printf("\"It vill destroy alle liffing tings caught in ");
	printf("its beam!\" He giggles.\n");
	printf("Your men set up the highly complex RNA ");
	printf("depolarizer on the front lines.\n");
	printf("They aim it at a large group of aliens and ");
	printf("depress the on/off switch.\n");
	printf("The RNA depolarizer emits a peculiar ");
	printf("whine and greenish glow.........\n");
	printf("........ and absolutely nothing happens to the aliens.\n");
	printf("\"Donnerwetter!\" screams Professor Scheisskopf, ");
	printf("visibly shaken.\n");
	printf("\"Der quark-zartvortler must slightly out of ");
	printf("hypercritical alignment be.\"\n");
	printf("The professor carts his experimental weapon ");
	printf("back to his laboratory.\n");
	}
	else	{
	hkill = dice(6);
	if(hkill > humans) hkill = humans;
	printf("Professor Scheisskopf brings out his ....");
	printf("..... RNA DEPOLARIZER!!!!\n");
	printf("\"It vill destroy alle liffing tings within ");
	printf("its range!\" He giggles.\n");
	printf("Your men set up the highly complex RNA depolarizer ");
	printf("on the front lines.\n");
	printf("They aim it at a large group of aliens and ");
	printf("throw the power switch.\n");
	printf("The RNA depolarizer emits a peculiar whine ");
	printf("and greenish glow.........\n");
	printf("........ and all the humans for a hundred yards ");
	printf("drop dead instantly.\n");
	printf("\"Donnerwetter!\" screams Professor Scheisskopf, ");
	printf("visibly shaken.\n");
	printf("\"Der directional fronkzapdinger must set on ");
	printf("\'Reverse\' be!\"\n");
	printf("The professor carts his experimental weapon ");
	printf("back to his laboratory.\n");
	printf("%ld humans are killed by a slightly ", hkill);
	printf("unfinished secret weapon.\n");
	}
	return(0);
}
long god()
{
	extern long aliens, humans, hkill;
	static char *gods[10] ={"Jehovah","Allah","Zeus","Odin",
			"Quetzalcoatl","Siva","Jesus","The Holy Ancestors",
			"The Great Spirit","Zarathustra"};
	static char *excuse[5] ={"playing golf","shooting pool",
			"drinking beer","tuning up His chariot",
			"making it rain in Buffalo"};
	long i, j, k;
	long akill;

	akill = 0;
	j = dice(100);
	i = dice(10) - 1;
	printf("All the humans pray to %s for victory over the aliens.\n",
		gods[i]);
	printf("All the aliens pray to Fzurgtaprotz for victory over the ");
	printf("humans.\n");
	if(j <= 50)	{
		printf("The humans\' prayers are answered!!\n");
		hkill = 0;
		if(j <= 25)	{
			k = dice(5) - 1;
			akill = 0;
			printf("... but by %s\'s answering service.\n",gods[i]);
			printf("It seems %s is out %s",gods[i],excuse[k]);
			printf(" and can\'t be reached.\n");
			printf("Better luck next time.\n");
		}
		else	{
			akill = aliens;
			printf("%s comes to Earth and zaps",gods[i]);
			printf(" all the aliens.\n");
			printf("\n\n   ZZZAAAAAAAAPPPPPPPPPP!!!!!!!!!!!\n\n");
		}
	}
	else	{
		printf("The aliens\' prayers are answered!!\n");
		akill = 0;
		if(j > 75)	{
			printf("... but by Fzurgtaprotz\' answering");
			printf(" service.\n");
			printf("It seems Fzurgtaprotz is out gzarmphing");
			printf(" plzotls and can\'t be reached.\n");
			printf("Better luck next time.\n");
			hkill = 0;
		}
		else	{
			hkill = humans;
			printf("Fzurgtaprotz comes to Earth and zaps all");
			printf(" the humans.\n");
			printf("\n\n   ZZZAAAAAAAAPPPPPPPPPP!!!!!!!!!!!\n\n");
		}
	}
	return(akill);
}
long trap()
{
	extern long humwnd, aliens, hkill;
	long akill;
	int i;
	static long count = 0;

	akill = 0;
	if(humwnd != 0)	{
		if(count == 0)	{
			printf("You decide to lay a trap for the aliens.\n");
			printf("You truck your wounded out to White Sands ");
			printf("Missile Range.\n");
			printf("You leave them amidst many large \"FREE ");
			printf("LUNCH\" signs.\n");
			printf("The aliens show up and start chowing down ");
			printf("on your delicious wounded.\n");
			printf("You then set off the 20-megaton bomb buried");
			printf(" just under the sand there.\n");
			humwnd = 0;
			i = aliens / 3;
			akill = dice(i) + dice(8);
			akill = dice(i) + akill;
			akill = akill + dice(i);
			if(akill > aliens) akill = aliens;
			printf("All your wounded and %ld aliens",akill);
			printf(" are killed in the blast.\n");
			printf("There is public outcry about your callous");
			printf(" use of wounded men,\n");
			count++;
			printf("but since it worked, you are not lynched.\n");
		}
		else	{
			printf("Your subordinates are alarmed that you want");
			printf(" to try that horrible trick again.\n");
			printf("You are arrested and found guilty of war");
			printf(" crimes.\n");
			printf("As punishment YOU are sent to White Sands ");
			printf("Missile Range.\n");
			printf("The aliens aren\'t stupid enough to fall for");
			printf(" the same trick twice,\n");
			printf("so you are left out there to die ");
			printf("of exposure.\n");
			printf("Without your leadership your army soon falls");
			printf(" apart..................\n");
			hkill = humans;
			akill = 0;
		}
	}
	else	{
		printf("You must have wounded in order to try the trap.\n");
		akill = hkill = 0;
	}
	return(akill);
}
long treaty()
{
	extern long humans, aliens;
	
	printf("You negotiate a treaty with the aliens.\n");
	printf("The war is over, but they get the states of ");
	printf("New Jersey and Utah,\n");
	printf("the entire Indian Subcontinent, France, Kazakhstan,\n");
	if(aliens > humans)	{
		printf("New Zealand, Algeria, Kenya, Prince Edward Island");
		printf(", Hungary,\n");
		printf("the Pribilof Islands, Queen Maud Land, the palace");
		printf(" at Versailles, \n");
		printf("the Detroit Tigers, H. M. S. Victory, your men\'s");
		printf(" tireirons,\n");
		printf("a fleet of 1955 Ford Thunderbirds, a cubic mile");
		printf(" of cheesecorn,\n");
		printf("a dozen pogo sticks, a complete set of W. C.");
		printf(" Fields\' movies,\n");
	}
	printf("Upper Volta, the cities of Bratislava, Omsk, Woomera,\n");
	printf("Yokohama, all of Latin America, Disneyworld,\n");
	printf("and the rights to set up \"Colonel Zurgznart\'s Kentucky\n");
	printf("Fried Humans\" franchises worldwide.\n");
	printf("There is worldwide outcry about giving up Disneyworld.\n");
	printf("As a result, you are the first human in the local Colonel");
	printf(" Zurgznart\'s.......\n");
	printf("...and the aliens think you go very well with a little");
	printf(" mustard.\n");
	return;
}
modd()		/* determines case modifier due to odds */
{
	extern long aliens, humans;
	int answer;

	answer = 0;
	if(aliens > (2 * humans) && (aliens > 50))   answer = -20;
	else if(humans > (3 * aliens) && (aliens > 10))	answer = 20;
	return(answer);
}
range(n)		/* determines which type of dice to roll */
long n;
{
	long i;

	if(n < 10)
		i = 4;
	else if(n < 50)
		i = 6;
	else if(n < 110)
		i = 10;
	else 
		i = n / 10;
	return(i);
}

dice(r)			/* the big dice */
int r;
{
	long roll;
	int i;

	i = rand();
	roll = (i % r) + 1;
	return(roll);
}

main ()			/*  satiric alien invasion game */
{
	char c, cold;
	long akill;	
	long n;
	extern char s[];
	extern unsigned srand();
	extern long humans, aliens, hkill, hwound, humwnd;
				/*hwound: wounded this time*/
				/*humwnd: total wounded*/	
	humans = aliens = 0;
	hwound = hkill = cold = humwnd = 0;
	n = 0;
	printf("ALIENS FROM OUTER SPACE\n");
	(void) srand((unsigned)getpid());
	while(humans <= 0)  {
		printf("How many humans do you want in your army?\n");
		(void) scanf("%ld", &humans); 
	}
	while(aliens <= 0)  {
		printf("How many aliens do you wish to fight?\n");
		(void) scanf("%ld", &aliens);
		if(aliens > humans)  {
			printf("Are you sure that is wise?\n");
			(void) scanf("%s", &c);
			if(c != 'y')   aliens = 0;
		}
	}
	printf("\n\n\n\tTHE ALIENS HAVE LANDED\n\n");
	unrec('?');

/*	MAIN LOOP	*/
	while(aliens > 0 && humans > 0)	{
		printf("Aliens left: %ld \tHumans left: %ld ",aliens,humans);
		printf("\t Wounded Humans: %ld\n",humwnd);
		prompt();
		(void) scanf("%s", s);
		if(s[0] == cold) n++;
		else {
			n = 0;
			cold = s[0];
		}
		switch( s[0] ) 		{
			case '\\':
			printf("Magic Weapons Engaged, Sir.\n");
			akill = aliens;
			break;

			case 'd':
			akill = dyna(n);
			break;

			case 'f':
			akill = flak(n);
			break;

			case 'h':
			akill = handguns(n);
			break;

			case 'j':
			akill = jets(n);
			break;

			case 'l':
			akill = lasers(n);
			break;

			case 'M':
			hwound = medic();
			break;

			case 'n':
			akill = nukes(n);
			break;

			case 'o':
			akill = obscene(n);
			break;

			case 'p':
			akill = punk(n);
			break;

			case 'r':
			akill = rocks(n);
			break;
			
			case 's':
			akill = secret();
			break;

			case 't':
			akill = tires(n);
			break;

			case 'D':
			akill = god();
			break;

			case '?':
			akill = 0;
			unrec('?');
			break;

			case 'T':
			if(s[3] == 'p') {
				akill = trap();
				s[3] = 'x';
			}
			else if(s[3] == 'a') {
				treaty();
				(void) exit(0);
			}
			else unrec('x');
			break;
			
			default:
			unrec('x');
			break;
		}
		aliens = aliens - akill;
		humans = humans - hkill - hwound;
		humwnd = humwnd + hwound;
		hkill = hwound = akill = 0;
	}
	if(aliens <= 0)	{
		printf("\n\n\n\tYOU HAVE DEFEATED ALL THE ALIENS!!!!\n");
		printf("The human race thanks you.\n");
		}
	else		{
		printf("\n\nYour army has been destroyed!\n");
		printf("The human race will be enslaved and used as food ");
		printf("animals.\n");
		}
}
