#include "words.h"

main()
{

	long time();
	int rnd(), n, which, howmany;
	char p(), *name;

	srand(time(0));

	name = one(names);
	which=rnd(2);
	if (which == 0)
		printf("Dear Penthouse:\n\n");
	if (which == 1)
		printf("Dear Sirs:\n\n");

	which=rnd(6);
	if (which == 0)
	{
		printf("\tLet me tell you just how %s\n",one(adj));
		printf("life can be!\n");
	};
	if (which == 1)
	{
		printf("\tI've read a lot of forum letters, and\n");
		printf("not believed a word of them, until I had\n");
		printf("the most %s experience of my life%c\n",one(adj),p());
	};
	if (which == 2)
	{
		printf("\tWhat a %s day%c\n",one(adj),p());
	};
	if (which == 3)
	{
		printf("I know you're not going to believe this,\n");
		printf("but this really happened%c\n",p());
	};
	if (which == 4)
	{
		printf("I'm a %s, %s guy,\n",one(adj),one(madj));
		printf("and I'm going with the most %s girl you have ever seen%c\n",one(fadj),p());
	}
	if (which == 5)
	{
		printf("%s and I have been meaning to write for a long time.\n",name);
	}

	which=rnd(4);
	if (which == 0)
	{
		printf("One day, I was outside,\n");
		printf("spring cleaning my %s %s%c\n",one(adj),one(noun),p());
	};
	if (which == 1)
	{
		printf("Just yesterday, I was out in my\n");
		printf("back yard, %s %sing my %s %s%c\n",one(adv),one(pen),one(fadj),one(noun),p());
	};
	if (which == 2)
	{
		printf("As I remember, I was so wasted, I was trying\n");
		printf("to see if you could get off on %s %sing%c\n",one(noun),one(stim),p());
	};
	if (which == 3)
	{printf("One Saturday night, after I had just returned from a grueling game of squash,\n");
	printf("I was ready to jump into the shower when %s,\n",name);
	printf("our %s stuck-up neighbor,\n",one(adj));
	printf("came in wantint to talk to me about our physics homework.\n");
	}

	which=rnd(3);
	if (which == 0)
	{
		printf("Then, a car stopped in front of my house,\n");
		printf("and this %s %s jumped out,\n",one(fadj),one(girl));
		printf("walked right up to me and %sed me\n",one(stim));
		printf("right on the %s%c\n",one(tongue),p());
	};
	if (which == 1)
	{
		printf("Then, this %s\n",one(fadj));
		printf("%s walked up, and asked me my name!\n",one(girl));
	};
	if (which == 2)
	{
		printf("I glanced up, and there stood\n");
		printf("a %s beautiful %s %s\n",one(adv),one(fadj),one(girl));
		printf("wearing a %s %s%c\n",one(adj),one(clo),p());
	};

	which=rnd(2);
	if (which == 0)
	{
		printf("This seemed a bit odd, but I, being a\n");
		printf("pretty %s %s, thought it was just %s\n",one(madj),one(boy),one(adj));
		printf("enough to be %s.\n",one(adj));
	};
	if (which == 1)
	{
		printf("I was a bit surprised, of course, but I\n");
		printf("didn't mind a bit.\n");
	};

	which=rnd(4);
	if (which == 0)
	{
		printf("After the usual %sing about the\n",one(talk));
		printf("weather, and the %s on television, I\n",one(noun));
		printf("asked her in to see my house.\n");
	};
	if (which == 1)
	{
		printf("We %sed for a while, then\n",one(talk));
		printf("I invited her inside.\n");
	};
	if (which == 2)
	{
		printf("As neither of us was in a hurry,\n");
		printf("she and I wandered inside, away from\n");
		printf("prying eyes.\n");
	};
	if (which == 3)
	{
		printf("Since we were such a %s pair, we\n",one(adj));
		printf("decided to vanish into the %s depths of\n",one(adj));
		printf("the dark house.\n");
	};

	which = rnd(3);
	if (which == 0)
	{
		printf("When we got in the\n");
		printf("door, she turned into a different person!\n");
	};
	if (which == 1)
	{
		printf("Once inside, out of sight, she\n");
		printf("%sed that she was a rather forward\n",one(talk));
		printf("sort of girl, and hoped I didn't object!\n");
	};
	if (which == 2)
	{
		printf("\"%s\" she %sed, indicating that\n",one(exc),one(talk));
		printf("she was both horny and %s.\n",one(fadj));
	};

	which=rnd(2);
	if (which==0)
	{
		printf("I could tell that we both were %s\n",one(adj));
		printf("enough to get %s, and she %sed that\n",one(reac),one(talk));
	};
	if (which==1)
	{
		printf("She %sed how much she was attracted to my\n",one(talk));
		printf("%s body (I am a %s %s), and\n",one(madj),one(madj),one(boy));
	};

	which = rnd(3);
	if (which == 0)
	{
		printf("her boyfriend, the %s %s, hadn't\n",one(fadj),one(boy));
		printf("%sed her, or even %sed her %s\n",one(pen),one(stim),one(tits));
		printf("in %d days.\n",rnd(43)+2);
	};
	if (which == 1)
	{
		printf("she usually %sed with her %s,\n",one(pen),one(rel));
		printf("but her whole family had gone away to\n");
		printf("see her %s %s, the %s.\n",one(adj),one(rel),one(girl));
	};
	if (which == 2)
	{
		printf("she hadn't seen her %s boyfriend since\n",one(fadj));
		printf("he started %sing with her %s.\n",one(pen),one(rel));
	};

	which = rnd(2);
	if (which == 0)
	{
		printf("The way she kept letting me see flashes\n");
		printf("of her %s, I began to suspect that she was\n",one(hers));
		printf("ready for %s %s %sing.\n",one(some),one(adj),one(pen));
	};
	if (which == 1)
	{
		printf("She then began %sing my %s %s %s\n",one(stim),one(madj),one(part),one(adv));
		printf("under my %s, and I began to get the\n",one(clo));
		printf("idea that she was horny (%d years of\n",rnd(5)+5);
		printf("college hasn't exactly made me dumb!).\n");
	};

	which = rnd(2);
	if (which == 0)
	{
		printf("Of course, I %sed her %s %s.\n",one(stim),one(tits),one(adv));
	};
	if (which == 1)
	{
		printf("I returned the favor, of course, and\n");
		printf("began %sing her %s %s.\n",one(stim),one(fadj),one(tits));
	};

	which=rnd(2);
	if (which == 0)
	{
		printf("She looked into\n");
		printf("my %s eyes, and suddenly placed her %s\n",one(madj),one(part));
		printf("against my %s!\n",one(his));
	};
	if (which == 1)
	{
		printf("\"%s!\" I said suddenly,\n",one(exc));
		printf("as she had just slipped her %s\n",one(fadj));
		printf("hand down my %s and grabbed my %s!\n",one(clo),one(his));
	};

	which=rnd(2);
	if (which == 0)
		printf("I understood instantly!\n");
	if (which == 1)
		printf("I knew what she had on her %s mind.\n",one(fadj));

	which=rnd(2);
	if (which == 0)
	{
		printf("She %sed that she was %s (I agreed\n",one(talk),one(adj));
		printf("with her), and I %sed that I was\n",one(talk));
		printf("too (%s so!).\n",one(adv));
	};
	if (which == 1)
	{
		printf("We raced %s up to my room, where she %sed\n",one(adv),one(talk));
		printf("that she loved my %s %s collection.\n",one(adj),one(noun));
	};

	which = rnd(2);
	if (which == 0)
	{
		printf("After a little %s %sing, we\n",one(adj),one(stim));
		printf("put Brian Eno's album, \"The way you\n");
		printf("%s my %s's %s\" on the\n",one(stim),one(rel),one(noun));
		printf("stereo (at the wrong speed!).\n");
	};
	if (which == 1)
	{
		printf("Then she climbed onto my %s and got down to\n",one(noun));
		printf("business.\n");
	};

	if (rnd(3)<2)
	{
		printf("She smiled as I removed\n");
		printf("her %s %s %s.\n",one(fadj),one(clo),one(adv));
	};

	if (rnd(2) == 0)
	{
		printf("I %sed her %s %s\n",one(stim),one(fadj),one(tits));
		printf("with my whole face!\n");
	};
	if (rnd(3) == 0)
	{
		printf("I %sed her %s %s until\n",one(stim),one(fadj),one(tits));
		printf("she %s %s!\n",one(reac),one(adv));
	};

	which = rnd(2);
	if (which == 0)
	{
		printf("She %s peeled off my remaining clothes\n",one(adv));
		printf("so that she could %s my %s with her %s!\n",one(stim),one(his),one(tongue));
	};
	if (which == 1)
	{
		printf("She then undid\n");
		printf("my %s %s, revealing a %s %s at attention.\n",one(clo),one(adv),one(madj),one(his));
	};

	which = rnd(2);
	if (which == 0)
	{
		printf("She %sed it with her %s %s, making it\n",one(stim),one(fadj),one(tongue));
		printf("even more %s than it normally is.\n",one(madj));
	};
	if (which == 1)
	{
		printf("She %sed it all over with her %s\n",one(stim),one(fadj));
		printf("%s, until it was %s to her liking.\n",one(tits),one(madj));
	};

	which=rnd(3);
	if (which == 0)
		printf("Then we undressed.\n");
	if (which == 1)
		printf("We then ripped each others' clothes off.\n");
	if (which == 2)
	{
		printf("I tore her clothes off so I could get\n");
		printf("at her %s %s.\n",one(fadj),one(hers));
	};

	if (rnd(3)<1)
		printf("She was %s!\n",one(fadj));

	if (rnd(4)<1)
		printf("I couldn't keep my eyes (or hands) off her!\n");

	if (rnd(2)<1)
	{
		printf("I was so excited, that after only\n");
		printf("%d minutes of %sing her %s %s, I\n",rnd(100),one(stim),one(fadj),one(tits));
		printf("jumped on her and %sed her for all I was worth!\n",one(pen));
	};

	if (rnd(3)<1)
	{
		printf("\"%s!\" she %sed,\n",one(exc),one(talk));
		printf("as I %sed my %s %s\n",one(pen),one(madj),one(his));
		printf("into her %s, %s %s.\n",one(fadj),one(fadj),one(hers));
	};

	/*   read of "add.c" here      locate by token ;;; */


	if (rnd(5) == 0) {
		printf("The %s %s as the I %s lowered\n",one(noun),one(reac),one(adv));
		printf("my %s %s toward her naked %s body.\n",one(madj),one(his),one(fadj));
	};

	if (rnd(3) == 0) {
		printf("She %s %s me %s as I %sed her %s with\n",one(adv),one(stop),one(vain),one(pen),one(hers));
		printf("my %s, %s %s!\n",one(adj),one(madj),one(his));
	};

	if (rnd(5) == 0) {
		printf("After I had\n%sed my %s %s\n",one(pen),one(madj),one(his));
		printf("inside her %s %s, I heard her %sing that\n",one(fadj),one(hers),one(talk));
		printf("I was a \"%s %s %s with a %s %s %s.\"\n",one(adj),one(adj),one(boy),one(madj),one(madj),one(his));
	};

	if (rnd(6) == 0)
		printf("She then %s %s %sed me on our %s %s %s!\n",one(adv),one(adv),one(pen),one(adj),one(adj),one(noun));

	if (rnd(5) == 0) {
		printf("\"I wish I could have been %sed,\" she %sed, as the two of\n",one(pen),one(talk));
		printf("us used the %s %s, with the %s looking on.\n",one(adj),one(noun),one(noun));
	};

	if (rnd(3) == 0) {
		printf("\"You're %sing me!\" she %sed, lying down on my %s %s\n",one(pen),one(talk),one(adj),one(noun));
		printf("as I %s massaged her %s %s %s with my %s %s.\n",one(adv),one(fadj),one(fadj),one(tits),one(madj),one(tongue));
	};

	if (rnd(6) == 0) {
		printf("\"%s!\" %sed my friend, the %s %s as I\n",one(exc),one(talk),one(fadj),one(girl));
		printf("%sed her %s %s\n",one(stim),one(fadj),one(tits));
		printf("and %sed my %s %s into her %s %s.\n",one(pen),one(madj),one(his),one(fadj),one(hers));
	};

	if (rnd(4) == 0) {
		printf("\"That reminds me of my %s's %s %s %s!\"\n",one(rel),one(adj),one(adj),one(noun));
		printf("she %sed as she %s %sed my %s %s\n",one(talk),one(adv),one(stim),one(madj),one(his));
		printf("with her %s %s %s.\n",one(adv),one(fadj),one(tongue));
	};

	if (rnd(3) == 0) {
		printf("I %sed her and %sed her %s with my %s\n",one(pen),one(stim),one(tits),one(madj));
		printf("%s until she %sed for me to start %sing her %s instead!\n",one(tongue),one(talk),one(stim),one(hers));
	};

	if (rnd(6) == 0) {
		printf("Her %s gay %s shook his %s %s %s at\n",one(fadj),one(boy),one(fadj),one(fadj),one(his));
		printf("her %s body and called her a %s, %s %s (?).\n",one(madj),one(madj),one(madj),one(girl));
	};

	which = rnd(3);
	if (which == 0)
	{
		printf("After we were through, though, she just\n");
		printf("dressed, %sed my %s a last\n",one(stim),one(his));
		printf("time, and left.\n");
	};
	if (which == 1)
	{
		printf("After that, we fell asleep, and when\n");
		printf("I awoke, she was gone.\n");
	};
	if (which == 2)
	{
		printf("I was so zonked by it, I just crashed.\n");
		printf("When I came to, I found a note:\n\n");
		printf("Dear %s %s:\n\n",one(madj),one(boy));
		printf("\tIt was %s.  I loved the way you\n",one(adj));
		printf("%sed me, but I am not as %s as you.\n",one(pen),one(madj));
		printf("You're really %s in my book!\n\n",one(adj));
		printf("\t\t\tLust,\n\n");
		printf("\t\t\t\tyour %s %s\n\n",one(fadj),one(girl));
	};

	which=rnd(3);
	if (which == 0)
		printf("I never saw her again, but if I do, I won't mind!\n\n");
	if (which == 1)
	{
		printf("We never met again, but some lucky guy\n");
		printf("may have the %s experience I had!\n\n",one(adj));
	};
	if (which == 2)
	{
		printf("I've %sed a lot of %ss since\n",one(pen),one(girl));
		printf("then, but none of them have been as\n");
		printf("%s as she was.\n\n",one(fadj));
	};

}

int
rnd(range)
int range;
{
	int rand();

	return range == 0 ? 0 : (rand()>>6)%range;
}

char
p()
{
	if (rnd(5)<1)
		return '!';
	else
		return '.';
}
