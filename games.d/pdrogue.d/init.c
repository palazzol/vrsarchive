#include <curses.h>
#include "object.h"
#include "room.h"
#include <sgtty.h>
#include <signal.h>

char *player_name;
short cant_int = 0, did_int = 0;

extern char ichars[];
extern short party_room;

init()
{
	char *getlogin();
	short i;
	int tstp(), byebye(), onintr();

	if (!(player_name = getlogin())) {
		fprintf(stderr, "Hey!  Who are you?");
		exit(1);
	}
	printf("Hello %s, just a moment while I dig the dungeon...",
	player_name);
	fflush(stdout);

	initscr();
	for (i = 0; i < 26; i++) {
		ichars[i] = 0;
	}
	start_window();
	signal(SIGTSTP, tstp);
	signal(SIGINT, onintr);
	signal(SIGQUIT, byebye);
	if ((LINES < 24) || (COLS < 80)) {
		clean_up("must be played on 24 x 80 screen");
	}
	LINES = SROWS;

	srandom(getpid());
	shuffle_colors();
	mix_metals();
	make_scroll_titles();

	level_objects.next_object = 0;
	level_monsters.next_object = 0;
	player_init();
}

player_init()
{
	object *get_an_object(), *obj;

	rogue.pack.next_object = 0;

	obj = get_an_object();
	get_food(obj);
	add_to_pack(obj, &rogue.pack, 1);

	obj = get_an_object();		/* initial armor */
	obj->what_is = ARMOR;
	obj->which_kind = RING;
	obj->class = RING+2;
	obj->is_cursed = obj->is_protected = 0;
	obj->damage_enchantment = 1;
	obj->identified = 1;
	add_to_pack(obj, &rogue.pack, 1);
	rogue.armor = obj;

	obj = get_an_object();		/* initial weapons */
	obj->what_is = WEAPON;
	obj->which_kind = MACE;
	get_weapon_thd(obj);
	obj->is_cursed = 0;
	obj->damage = "2d3";
	obj->to_hit_enchantment = obj->damage_enchantment = 1;
	obj->identified = 1;
	add_to_pack(obj, &rogue.pack, 1);
	rogue.weapon = obj;

	obj = get_an_object();
	obj->what_is = WEAPON;
	obj->which_kind = BOW;
	get_weapon_thd(obj);
	obj->is_cursed = 0;
	obj->damage = "1d2";
	obj->to_hit_enchantment = 1;
	obj->damage_enchantment = 0;
	obj->identified = 1;
	add_to_pack(obj, &rogue.pack, 1);

	obj = get_an_object();
	obj->what_is = WEAPON;
	obj->which_kind = ARROW;
	obj->quantity = get_rand(25, 35);
	get_weapon_thd(obj);
	obj->is_cursed = 0;
	obj->damage = "1d2";
	obj->to_hit_enchantment = 0;
	obj->damage_enchantment = 0;
	obj->identified = 1;
	add_to_pack(obj, &rogue.pack, 1);
}

clean_up(estr)
char *estr;
{
	move(LINES-1, 0);
	refresh();
	stop_window();
	printf("\n%s\n", estr);
	exit(0);
}

start_window()
{
	crmode();
	noecho();
	nonl();
	edchars(0);
}

stop_window()
{
	endwin();
	edchars(1);
}

byebye()
{
	clean_up("Okay, bye bye!");
}

onintr()
{
	if (cant_int) {
		did_int = 1;
	} else {
		signal(SIGINT, SIG_IGN);
		check_message();
		message("interrupt", 1);
		signal(SIGINT, onintr);
	}
}

edchars(mode)
short mode;
{
	static short called_before = 0;
	static struct ltchars ltc_orig;
	static struct tchars tc_orig;
	struct ltchars ltc_temp;
	struct tchars tc_temp;

	if (!called_before) {
		called_before = 1;
		ioctl(0, TIOCGETC, &tc_orig);
		ioctl(0, TIOCGLTC, &ltc_orig);
	}
	ltc_temp = ltc_orig;
	tc_temp = tc_orig;

	if (!mode) {
		ltc_temp.t_suspc = ltc_temp.t_dsuspc = ltc_temp.t_rprntc =
		ltc_temp.t_flushc = ltc_temp.t_werasc = ltc_temp.t_lnextc = -1;
	}

	ioctl(0, TIOCSETC, &tc_temp);
	ioctl(0, TIOCSLTC, &ltc_temp);
}
