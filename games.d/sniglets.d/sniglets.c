/*
 *	The Sniglets that appear in this file are taken from:
 *		"Sniglets" - Rich Hall & Friends
 *	Collier Books, Macmillian Publishing Company, New York
*/
char *rule[] = {
"Aquadextrous - adj. Possessing the ability to turn the bathtub faucet\n\
on and off with your toes.\n\
",
"Blithwapping - v. Using anything BUT a hammer to hammer a nail into the\n\
wall, such as shoes, lamp bases, doorstops, etc.\n\
",
"Burbulation - n. The obsessive act of opening and closing a\n\
refrigerator door in an attempt to catch it before the automatic\n\
light comes on.\n\
",
"Carperpetuation (kar' pur pet u a shun) - n. The act, when vacuuming,\n\
of running over a string at least a dozen times, reaching over and\n\
picking it up, examining it, then putting it back down to give the\n\
vacuum one more chance.\n\
",
"Magnocartic - n. Any automobile that, when left unattended, attracts\n\
shopping carts.\n\
",
"Cinemuck - n. The combination of popcorn, soda, and melted chocolate\n\
which covers the floors of movie theaters.\n\
",
"Elbonics - n. The actions of two people maneuvering for one armrest\n\
in a movie theatre.\n\
",
"Flannister - n. The plastic yoke that holds a six-pack of beer\n\
together.\n\
",
"Fenderberg - n. The large glacial deposits that form on the insides\n\
of car fenders during snowstorms.\n\
",
"Furbling - v. Having to wander through a maze of ropes at an airport\n\
or bank even when you are the only person in line.\n\
",
"Genderplex - n. The predicament of a person in a restaurant who is\n\
unable to determine his or her designated restroom (e.g. turtles and\n\
tortoises).\n\
",
"Gleemites - n. Petrified deposits of toothpaste found in sinks.\n\
",
"Gurmlish - n. The red warning flag at the top of a club sandwich\n\
which prevents the person from biting into it and puncturing the\n\
roof of his mouth.\n\
",
"Idiot Box - n. The part of the envelope that tells a person where to\n\
place the stamp when they can't quite figure it out for themselves.\n\
",
"Krogt - n. (chemical symbol: Kr) The metallic silver coating found\n\
on fast-food game cards.\n\
",
"Lactomangulation - n. Manhandling the \"open here\" spout on a milk\n\
carton so badly that one has to resort to using the \"illegal\" side.\n\
",
"Mittsquinter - n. A ballplayer who looks into his glove after\n\
missing the ball, as if, somehow, the cause of the error lies there.\n\
",
"Mustgo - n. Any item of food that has been sitting in the\n\
refrigerator so long it has become a science project.\n\
",
"Narcolepulacy (nar ko lep' ul ah see) - n. The cantagious action of\n\
yawning, causing everyone in sight to also yawn.\n\
",
"Pediddel - n. A car with only one working headlight.\n\
",
"Petribar - n. Any sun-bleached prehistoric candy that has been\n\
sitting in the window of a vending machine too long.\n\
",
"Phosflink - v. To flick a bulb on and off when it burns out (as if,\n\
somehow, that will bring it back to life).\n\
",
"PIYAN (pi' an) - n. (acronym: \"Plus If You Act Now\") Any\n\
miscellaneous item thrown in on a late night television ad.\n\
",
"Purpitation - v. To take something off the grocery shelf, decide you\n\
don't want it, and then put it in another section.\n\
",
"Scribline - n. The blank area on the back of credit cards where\n\
one's signature goes.\n\
",
"Slurm - n. The slime that accumulates on the underside of a soap bar\n\
when it sits in the dish too long.\n\
",
"Spagmumps - n. Any of the millions of Styrofoam wads that accompany\n\
mail-order items.\n\
",
"Spirobits - n. The frayed bits of left-behind paper in a spiral\n\
notebook.\n\
",
"Spirtle - n. The fine stream from a grapefruit that always lands\n\
right in your eye.\n\
",
"Squatcho - n. The button at the top of a baseball cap.\n\
",
"Telepression - n. The deep-seated guilt which stems from knowing\n\
that you did not try hard enough to \"look up the number on your\n\
own\" and instead put the burden on the directory assistant.\n\
",
"Snacktrek - n. The peculiar habit, when searching for a snack, of\n\
constantly returning to the refrigerator in hopes\n\
that something new will have materialized.\n\
",
"Yinkel - n. A person who combs his hair over his bald spot, hoping\n\
no one will notice.\n\
",
"Nugloo (nug' lew) - n. Single continuous eyebrow that covers the\n\
entire forehead.\n\
"
};
main()
{
	long time();
	
	srand((unsigned)time((long *)0)+getpid());
	printf("%s",rule[(rand()>>8)%(sizeof(rule)/sizeof(char *))]);
}
