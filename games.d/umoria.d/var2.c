#include "constants.h"
#include "types.h"

treasure_type mush = {"& pint~ of fine grade mush",  80, ',', 0x00000000,
			1500, 0, 308, 1, 1, 0, 0, 0, 0, "1d1", 1};

/* Each type of character starts out with a few provisions...	*/
/* Note the the entries refer to array elements of INVENTORY_INIT array*/
byteint player_init[MAX_CLASS][5] = {
		{   0, 103,  41,   6,  32},     /* Warrior       */
		{   0, 103,  41,   6,  66},     /* Mage          */
		{   0, 103,  41,   6,  70},     /* Priest        */
		{   0, 103,   6,  32,  66},     /* Rogue         */
		{   0, 103,  41,   6,  66},     /* Ranger        */
		{   0, 103,  41,   6,  70}      /* Paladin       */
};

int total_winner = FALSE;

/* Following are store definitions				*/

/* Store owners have different characteristics for pricing and haggling*/
/* Note: Store owners should be added in groups, one for each store    */
owner_type owners[MAX_OWNERS] = {
{"Erick the Honest       (Human)      General Store",
	  250, 0.75, 0.08, 0.04, 0, 12},
{"Mauglin the Grumpy     (Dwarf)      Armory"       ,
	32000, 1.00, 0.12, 0.04, 5,  5},
{"Arndal Beast-Slayer    (Half-Elf)   Weaponsmith"  ,
	10000, 0.85, 0.10, 0.05, 1,  8},
{"Hardblow the Humble    (Human)      Temple"       ,
	 3500, 0.75, 0.09, 0.06, 0, 15},
{"Ga-nat the Greedy      (Gnome)      Alchemist"    ,
	12000, 1.20, 0.15, 0.04, 4,  9},
{"Valeria Starshine      (Elf)        Magic Shop"   ,
	32000, 0.75, 0.10, 0.05, 2, 11},
{"Andy the Friendly      (Halfling)   General Store",
	  200, 0.70, 0.08, 0.05, 3, 15},
{"Darg-Low the Grim      (Human)      Armory"       ,
	10000, 0.90, 0.11, 0.04, 0,  9},
{"Oglign Dragon-Slayer   (Dwarf)      Weaponsmith"  ,
	32000, 0.95, 0.12, 0.04, 5,  8},
{"Gunnar the Paladin     (Human)      Temple"       ,
	 5000, 0.85, 0.10, 0.05, 0, 23},
{"Mauser the Chemist     (Half-Elf)   Alchemist"    ,
	10000, 0.90, 0.11, 0.05, 1,  8},
{"Gopher the Great!      (Gnome)      Magic Shop"   ,
	20000, 1.15, 0.13, 0.06, 4, 10},
{"Lyar-el the Comely     (Elf)        General Store",
	  300, 0.65, 0.07, 0.06, 2, 18},
{"Mauglim the Horrible   (Half-Orc)   Armory"       ,
	 3000, 1.00, 0.13, 0.05, 6,  9},
{"Ithyl-Mak the Beastly  (Half-Troll) Weaponsmith"  ,
	 3000, 1.10, 0.15, 0.06, 7,  8},
{"Delilah the Pure       (Half-Elf)   Temple"       ,
	25000, 0.80, 0.07, 0.06, 1, 20},
{"Wizzle the Chaotic     (Halfling)   Alchemist"    ,
	10000, 0.90, 0.10, 0.06, 3,  8},
{"Inglorian the Mage     (Human?)     Magic Shop"   ,
	32000, 1.00, 0.10, 0.07, 0, 10}
};

store_type store[MAX_STORES];

/* Stores are just special traps 		*/
treasure_type store_door[MAX_STORES] = {
{"The entrance to the General Store."              , 110, '1',0x00000000,
    0,      0, 101,   0,   0,   0,   0,   0,   0, "0d0"  ,  0},
{"The entrance to the Armory."                     , 110, '2',0x00000000,
    0,      0, 102,   0,   0,   0,   0,   0,   0, "0d0"  ,  0},
{"The entrance to the Weapon Smiths."              , 110, '3',0x00000000,
    0,      0, 103,   0,   0,   0,   0,   0,   0, "0d0"  ,  0},
{"The entrance to the Temple."                     , 110, '4',0x00000000,
    0,      0, 104,   0,   0,   0,   0,   0,   0, "0d0"  ,  0},
{"The entrance to the Alchemy Shop."               , 110, '5',0x00000000,
    0,      0, 105,   0,   0,   0,   0,   0,   0, "0d0"  ,  0},
{"The entrance to the Magic Shop."                 , 110, '6',0x00000000,
    0,      0, 106,   0,   0,   0,   0,   0,   0, "0d0"  ,  0}
};

int store_choice[MAX_STORES][STORE_CHOICES] = {
	/* General Store */
{105,104,103,102,102,104,42,105,104,27,26,5,4,3,3,2,2,2,1,1,1,1,1,1,1,1
},
	/* Armory        */
{30,31,32,33,34,35,36,37,38,39,40,41,43,44,45,46,47,30,33,34,43,44,28,29,30,31
},
	/* Weaponsmith   */
{ 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25, 6, 7,23,25,23,25
},
	/* Temple        */
{71,72,73,74,59,59,77,79,80,81,84,85,13,14,15,96,97,98,100,79,79,80,80,81,72,98
},
	/* Alchemy shop  */
{55,56,57,58,58,60,61,62,63,64,65,66,75,76,78,82,83,60,61,62,63,64,98,99,98,98
},
	/* Magic-User store*/
{67,49,50,51,52,53,54,48,68,69,69,70,86,87,88,89,90,91,92,93,94,95,86,101,68,88
}
};


/* code for these are all in sets.c  */
int general_store(), armory(), weaponsmith(),
  temple(), alchemist(), magic_shop();

/* Each store will buy only certain items, based on TVAL */
int (*store_buy[MAX_STORES])() = {
       general_store, armory, weaponsmith, temple, alchemist, magic_shop};


/* Following are arrays for descriptive pieces			*/

atype colors[MAX_COLORS] = {
  "Amber","Azure","Blue","Blue Speckled","Blue Spotted",
  "Black","Black Speckled","Black Spotted",
  "Brown","Brown Speckled","Brown Spotted",
  "Bubbling",
  "Chartreuse","Clear","Cloudy",
  "Copper","Copper Spotted","Crimson","Cyan",
  "Dark Blue","Dark Green","Dark Red","Ecru",
  "Gold","Gold Spotted",
  "Green","Green Speckled","Green Spotted",
  "Grey","Grey Spotted","Hazy","Indigo",
  "Light Blue","Light Green","Magenta","Metallic Blue",
  "Metallic Red","Metallic Green","Metallic Purple",
  "Misty",
  "Orange","Orange Speckled","Orange Spotted",
  "Pink","Pink Speckled",
  "Plaid","Puce","Purple","Purple Speckled",
  "Purple Spotted","Red","Red Speckled","Red Spotted",
  "Silver","Silver Speckled","Silver Spotted","Smoky",
  "Tan","Tangerine","Topaz","Turquoise",
  "Violet","Vermilion","White","White Speckled",
  "White Spotted","Yellow"
};

atype mushrooms[MAX_MUSH] = {
  "Blue","Black","Brown","Copper","Crimson",
  "Dark blue","Dark green","Dark red","Gold",
  "Green","Grey","Light Blue","Light Green",
  "Orange","Pink","Plaid","Purple","Red","Tan",
  "Turquoise","Violet","White","Yellow",
  "Wrinkled","Wooden","Slimy","Speckled",
  "Spotted","Furry"
};

atype woods[MAX_WOODS] = {
  "Applewood","Ashen","Aspen","Avocado wood",
  "Balsa","Banyan","Birch","Cedar","Cherrywood",
  "Cinnibar","Cottonwood","Cypress","Dogwood",
  "Driftwood","Ebony","Elm wood","Eucalyptus",
  "Grapevine","Hawthorn","Hemlock","Hickory",
  "Ironwood","Juniper","Locust","Mahogany",
  "Magnolia","Manzanita","Maple","Mulberry",
  "Oak","Pecan","Persimmon","Pine","Redwood",
  "Rosewood","Spruce","Sumac","Sycamore","Teak",
  "Walnut","Zebra wood"
};

atype metals[MAX_METALS] = {
  "Aluminum","Bone","Brass","Bronze","Cast Iron",
  "Chromium","Copper","Gold","Iron","Lead",
  "Magnesium","Molybdenum","Nickel",
  "Pewter","Rusty","Silver","Steel","Tin",
  "Titanium","Tungsten","Zirconium","Zinc",
  "Aluminum Plated","Brass Plated","Copper Plated",
  "Gold Plated","Nickel Plated","Silver Plated",
  "Steel Plated","Tin Plated","Zinc Plated"
};

atype rocks[MAX_ROCKS] = {
  "Amber","Agate","Alexandrite","Amethyst","Antlerite",
  "Aquamarine","Argentite","Azurite","Beryl","Bloodstone",
  "Calcite","Carnelian","Coral","Corundum","Cryolite",
  "Diamond","Diorite","Emerald","Flint","Fluorite",
  "Gabbro","Garnet","Granite","Gypsum","Hematite","Jade",
  "Jasper","Kryptonite","Lapus lazuli","Limestone",
  "Malachite","Manganite","Marble","Moonstone",
  "Neptunite","Obsidian","Onyx","Opal","Pearl","Pyrite",
  "Quartz","Quartzite","Rhodonite","Rhyolite","Ruby",
  "Sapphire","Sphalerite","Staurolite","Tiger eye","Topaz",
  "Turquoise","Zircon"
};

atype amulets[MAX_AMULETS] = {
  "Birch","Cedar","Dogwood","Driftwood",
  "Elm wood","Hemlock","Hickory","Mahogany",
  "Maple","Oak","Pine","Redwood","Rosewood",
  "Walnut","Aluminum","Bone","Brass","Bronze",
  "Copper","Iron","Lead","Nickel","Agate","Amethyst",
  "Diamond","Emerald","Flint","Garnet",
  "Jade","Obsidian","Onyx","Opal","Pearl","Quartz",
  "Ruby","Sapphire","Tiger eye","Topaz","Turquoise"
};

dtype syllables[MAX_SYLLABLES] = {
  "a","ab","ag","aks","ala","an","ankh","app",
  "arg","arze","ash","aus","ban","bar","bat","bek",
  "bie","bin","bit","bjor","blu","bot","bu",
  "byt","comp","con","cos","cre","dalf","dan",
  "den","doe","dok","eep","el","eng","er","ere","erk",
  "esh","evs","fa","fid","for","fri","fu","gan",
  "gar","glen","gop","gre","ha","he","hyd","i",
  "ing","ion","ip","ish","it","ite","iv","jo",
  "kho","kli","klis","la","lech","man","mar",
  "me","mi","mic","mik","mon","mung","mur","nej",
  "nelg","nep","ner","nes","nis","nih","nin","o",
  "od","ood","org","orn","ox","oxy","pay","pet",
  "ple","plu","po","pot","prok","re","rea","rhov",
  "ri","ro","rog","rok","rol","sa","san","sat",
  "see","sef","seh","shu","ski","sna","sne","snik",
  "sno","so","sol","sri","sta","sun","ta","tab",
  "tem","ther","ti","tox","trol","tue","turs","u",
  "ulk","um","un","uni","ur","val","viv","vly",
  "vom","wah","wed","werg","wex","whon","wun","x",
  "yerg","yp","zun"
};
