.ND
.ds CF - % -
.ds CH 
.nr PO 0.5i
.nr LL 6.5i
.nr PD 0
.ND
.ds CF - % -
.ds LH STAR
.ds CH 
.ds RH TREK
.nr PO 0.5i
.nr LL 6.5i
.nr PD 0
.na
.LP
.FS (R)
Star Trek is a registered trademark of Paramount Pictures.
.FE
.FS (TM)
Star Fleet Battles is a trademark held jointly by Task Force Games
and Amarillo Design Bureau.
.FE
.ce
T R E K   7 3
.ce
A Star Trek Battle Simulation
.sp
.PP
Trek73 is a computer-simulated battle based on the famous
Star Trek television series and
the board game Star Fleet Battles.
Via computer terminal, you can clash with enemy
battle cruisers, such as Klingon D-7's and Romulan Sparrowhawks,
and use the same strategies
that Captain Kirk has used.
Like Kirk, you control a Federation vessel similar to the
Enterprise; a computer program directs the enemy.
Victory can fall into several categories:
.sp
.PP
Decisive Victory -- You completely destroy or cripple the
attacking force.
.sp
.PP
Tactical Victory -- You out-maneuver the enemy using
high-speed escapes, corbomite bluffs, `play dead' tactics;
or the enemy surrenders.
.sp
.PP
Moral Victory -- You surrender or self-destruct and destroy
each other.
.sp
.PP
All distances are measured in megameters, one million meters
(abbreviated `M').
Speed is expressed in `Warp Factors'.
Each warp factor equals 100M per second.\**
.FS
Although technically incorrect, it does save the player from
having to compute cube roots.
.FE
All angles are expressed in common degrees from zero to
360, measured counter-clockwise from the x-axis, similar to
reading a protractor.
Only two dimensions are used.\**
.FS
This saves the player from having to work out problems in
spherical geometry.
.FE
.sp
.PP
Play is as follows:
.sp
.PP
1.  You issue one of a number of commands (fire phasers, change
course, launch antimatter pods, surrender, etc.) by typing
the appropriate code number into the keyboard;
.sp
.PP
2.  The enemy, under programmed instructions, issues a
similar command;
.sp
.PP
3.  Both your commands are executed (phasers are fired,
probes are launched, damages are assessed, courses changed,
etc.) while the vessels move through space;
.sp
.PP
4.  Unless certain end-game conditions are met (you destroy
the enemy, the enemy destroys you, your out-maneuver the
enemy, you both destroy each other, or one party surrenders)
the above steps are repeated.
.sp
.PP
Appendix 1 depicts the Enterprise's power circuits.
.sp
.PP
Appendix 2 displays certain weapon and shield angles.
.sp
.PP
Appendix 3 lists certain weapon and vessel specifications.
.sp
.PP
Appendix 4 lists initial deployment of resources.
.sp
.PP
Appendix 5 contains the designer's notes.
.sp 2
.LP
.in 1i
.ta .5i
.nf
CODE	     COMMAND
====	     =======
.sp
1	Fire Phasers
2	Fire Photon Torpedos
3	Lock Phasers Onto Target
4	Lock Tubes Onto Target
5	Manually Rotate Phasers
6 	Manually Rotate Tubes
7	*Phaser Status
8	*Tube Status
9	Load/Unload Torpedo Tubes
10	Launch Antimatter Probe
.sp
11	Probe Control (Detonate, Redirect, Lock)
12	*Position Report
13	*Position Display
14	Pursue An Enemy Vessel
15	Run From An Enemy Vessel
16	Manually Change Course And Speed
17	*Damage Report
18	Scan Enemy (Damage Report Of Enemy)
19	Alter Power Distribution
20	Jettison Engineering
.sp
21	Detonate Engineering
22	Alter Torpedo And Phaser Firing Parameters
23	Attempt Defenseless Ruse
24	Attempt Corbomite Bluff(s)
25	Surrender
26	Ask Enemy to Surrender
27	Initiate Self-Destruct Sequence
28	Abort Self-Destruct
29	*Survivors Report
30	*Print Version Number
31	*Reprints Above List
.sp
*Does Not Use A Turn
.in 0
.fi
.bp
.ce
Detailed Descriptions of Each Command
.sp
.PP
What follows is a detailed description of each command.
Each command is referred to by a number from 1 to 31.
After the name of the command is given, a synopsis of the
arguments the command requires is given, if any.
These arguments can be entered on the command line, separated
by whitespace, if you wish.
For instance, to fire phasers 1 through 4 with a spread of 15,
you could type '1 1234 15' on the command line.
.PP
It should be noted that all numbers refer to parameters for
the Heavy Cruiser Class ship, and that other ships have
slightly different characteristics.
.sp
.LP
1.  Fire Phasers.
.PP
[Phasers | all] [Spread]
.PP
Phasers are pure energy units which emit
a beam similar to lasers, but of a pulsating nature which
can be `phased' to interfere with the wave pattern of any
molecular form.
Phasers get their power from phaser banks, which in turn,
derive their power from the ship's engines.
Each phaser bank is capable of holding a charge of 10 units.
When firing, these banks discharge, similar to batteries, to
spread their destructive power through space.
After discharging, these banks are then recharged by the
engines.
Each phaser can be set to automatically track a target or
can be manually rotated.
Unless engineering is jettisoned (code 21), phasers only
fire from 0-125 and 235-360 degrees, relative to the ship's
course.
In other words, each vessel has a 110 degree blind side in
back of it in which phasers cannot fire.
If phasers fired into this blind side, they would destroy
the ship's engineering section.
.sp
.PP
The Captain also designates a wide or narrow phaser beam: a
wide beam to disrupt many targets; a narrow beam to inflict
maximum damage on a single target.
The maximum spread of phasers is 45 degrees, the minimum is
10 degrees.
The total beam width is twice the designated spread.
.sp
.PP
The firing percentage of each bank is preset to 100.
In other words, the bank fully discharges when firing.
This can be changed, however, using code 22.
.sp
.PP
The maximum range of phasers is 1000M; the maximum hit
factor is 45 with a ten degree spread, 10 with a forty-five
degree spread.
Phaser hit factors are calculated by the following formula:
.sp
.ce
hit = (bankunits)(firing%)sqrt(1-range/1000)(45/spread)
.sp
.PP
Phasers fire in .2-second intervals starting with bank one.
Phasers inflict heavy damage and casualties, but do not
destroy shields as much as antimatter explosions do.
.sp
.PP
A phaser is unable to fire if damaged, if firing into your
blind side, or if completely discharged.
.sp
.LP
2.  Fire Photon Torpedos.
.PP
[Tubes | all]
.PP
The Enterprise is equipped with six torpedo tubes, which, as
phasers, can be set to automatically track a target or be
manually rotated.
Unless engineering is jettisoned, tubes only fire from
0-135 and 225-360 degrees.
Each tube fires all its antimatter pods, which are
temporarily held suspended in a magno-photon force field.
Photon torpedos can be fired directly at an enemy, laid out
as a mine field, or scattered in an attacker's path as
depth charges.
.sp
.PP
Tubes must be loaded (code 9) prior to firing.
Normally, torpedos are launched at warp 12 in .2-second
intervals, beginning with tube one.
Photon torpedos have a proximity fuse of 200M.
All of these values can be changed by using code 22.
.sp
.PP
Torpedos must be launched with care since the antimatter
pods which are fired can never be recovered.
It is suggested that you not fire more than four torpedos at
any one time, since a certain number of them do miss, or are
destroyed by the enemy firing phasers at them.
It is also suggested that you fire them at distant targets,
beyond 1100M, to avoid the explosion radii of your own
weapons.
Hit factors resulting from antimatter explosions are
calculated as follows:
.sp
.ce
hit = 5(#podscontained)sqrt(1-range/(50(#podscontained)))
.sp
.PP
The maximum hit factor of an antimatter device is five times
the number of pods contained (in the case of torpedos, 50);
its explosion radius is 50 time the number of pods
contained (in the case of torpedos, 500).
Antimatter explosions heavily weaken shields but do not
damage equipment as much as phasers do.
This formula also applies to vessels, engineering sections,
and antimatter probe explosions.
.sp
.PP
A photon torpedo's proximity fuse will not be activated by a
cloaked ship.
.sp
.PP
Tubes are unable to fire if damaged, if firing into your
blind side, or if unloaded.
.sp
.LP
3.  Lock Phasers.
.PP
[Phasers | all] [Target Name]
.PP
Phasers locked on an enemy vessel will automatically aim
towards it.
Although phasers may track a vessel which is in the firing
blind side, they will not fire unless engineering is
jettisoned.
To fire at vessels in the blind spot, simply change course
at least 55 degrees.
Once a phaser is locked, it is not disengaged until the
target is destroyed (in which case it is then rotated to
zero degrees relative), relocked, manually overridden, or
damaged.
.sp
.PP
Phasers can not be locked onto cloaked enemy ships as they
can not find the target.
Phasers that were previously locked onto a non-cloaked ship
will track the enemy's last known course and position when
that ship cloaks.
.sp
.LP
4.  Lock Tubes.
.PP
[Tubes | all] [Target Name]
.PP
Tubes lock and unlock in the same manner that phasers do.
Tubes suffer the same locking limitations that phasers do in
reference to cloaked ships.
.sp
.LP
5.  Manually Rotate Phasers.
.PP
[Phasers | all] [Bearing]
.PP
Manually rotating phasers disengages any previous locks and
positions them as directed, relative to your course.
For example, if your course is 30, and phasers are rotated
45 degrees, they will hit a target bearing 75 degrees.
Rotating phasers into you blind side is permissible,
however, they will not fire.
.sp
.LP
6.  Manually Rotate Tubes.
.PP
[Tubes | all] [Bearing]
.PP
Manually rotating tubes is similar to rotating phasers.
.sp
.LP
7.  Phaser Status.
.PP
Phaser status reports the control (locks and damages),
deployment, levels, firing percentages (normally 100),
and charge/discharge rates (normally +10) of all phasers.
This command does not use a turn.
Cf. Command 22.
.sp
.LP
8.  Tube Status.
.PP
Tube status reports the control, deployment, tube levels,
launch speeds (normally 12), proximity delays (normally
200), and time delays (normally 10) of all tubes.
This command does not use a turn.
Cf. Command 22.
.sp
.LP
9.  Load/Unload Tubes.
.PP
[l | u] [Tubes | all]
.PP
Each specified tube will be automatically loaded with 10 units or
whatever remains in the engines, whichever is less.
Tubes can also be unloaded if the need arises.
.sp
.LP
10.  Launch Antimatter Probe.
.PP
[Pods] [Time] [Proximity] [Target | [<CR> Course]]
.PP
Probes are slow-moving devices equipped with internal
guidance systems which allow them to chase an enemy vessel.
Probes consist of at least ten antimatter pods which are
launched from an undamaged probe launcher at warp three.
As with torpedos, probes are set with time and proximity
fuses, and use the same hit factor formula as do torpedos.
.sp
.LP
11.  Probe Control.
.PP
[y | [n [Probe] [y | [n [Target | [<CR> Course]]]]]]
.PP
Probe control allows you to detonate or redirect probes
which may have missed.
.sp
.LP
12.  Position Report.
.PP
Position reports are vital since valuable information on
courses, bearings and ranges are given to aid the formation
of good strategy.
.PP
Each ship is listed along with its current speed, course,
and bearing.
Also listed is your relative bearing to that ship.
A relative bearing of 0 means you are pointed directly at
the ship, whereas a relative bearing of 180 means you are
pointed directly away from the ship.
Next is the reverse relative bearing, which gives the relative
bearing of you with respect to the ship listed.
.PP
Cloaked ships show up with an asterisk (*) before the name,
and the information displayed is the last available information
on those ships.
If no position report has been performed prior to the enemy ship
engages a cloaking device, no information will be available on 
that ship.
This order does not use a turn.
.sp
.LP
13.  Position Display.
.PP
[Radius of scan]
.PP
Position displays, similar to radar scans, show objects
which surround your vessel.
The Enterprise is indicated by a `+', jettisoned engineering
sections by a `#', probes by a `*', torpedos by a `:', and
enemy vessels by the first letter of their names.
Enemy vessels that are cloaked appear as lower case letters
and remain in their last noted absolute position.
.sp
.LP
14.  Pursue An Enemy Vessel.
.PP
[Target Name] [Warp Factor]
.PP
This order instructs the ship's navigation to face an enemy
vessel whenever possible.
Obviously it is impossible to pursue a cloaked vessel.
.sp
.LP
15.  Run From An Enemy Vessel.
.PP
[Target Name] [Warp Factor]
.PP
This order, just the opposite of order #14, instructs the
navigation to keep the stern of the Enterprise towards an
enemy vessel whenever possible.
Running from a cloaked vessel is not very useful.
.sp
.LP
16.  Manually Change Course and Speed.
.PP
[Course] [Warp Factor]
.PP
This order instructs navigation to maintain a fixed course
and speed.
The following information applies to the above three orders:
.sp
.PP
Your maximum rotation rate when turning is:
.ce
degrees per sec = 5 * (11 - current warp speed)
.sp
.PP
Accordingly, you can turn 55 degrees at warp one, 50 at
warp two, and so on down to 10 degrees at warp nine.
In other words, the faster your speed, the less
maneuverable you are.
You are also less maneuverable if your warp drive is damaged
or destroyed.
Your maximum speed is warp nine, the enemy's is warp eleven.
.sp
.LP
17.  Damage Report.
.PP
This report informs you of certain equipment status.
A destroyed computer make orders 3 (lock
phasers), 4 (lock torpedos), 14 (pursue), 15 (run), 27
(initiate self-destruct), and 28 (abort self-destruct)
impossible to execute.
You will be required to manually rotate phasers and
torpedos, and manually change course and speed.
Destroyed sensors makes orders 13 (position display) and
18 (scan) impossible.
A destroyed probe launcher prevents you from
launching probes.
A destroyed warp drive slows your maximum speed to warp 0.99 and
severly limits your maneuverability.
See order 20 about jettisoned engineering section.
When your crew of 450 dies, your vessel is as good as dead.
There are 350 men aboard each enemy vessel.
.sp
.PP
All of the above systems can be partially damaged.
A damaged warp drive (common) lowers your maximum speed
and maneuverability.
A damaged probe launcher (sometimes) may refuse to launch.
Damaged sensors (rare) may not be able to return position
displays or be able to scan an enemy.
A damaged computer (very rare) will sometimes refuse to lock onto
targets, and in addition, when damaged, may lose some of the
locks held by the weapons or by the helm.
.sp
.PP
Shield percentage is calculated by its energy drain times
its operating efficiency.
Efficiency starts at 100 and declines with each hit.
No damages of any kind are incurred when a shield absorbs
its first hit, no matter how great the hit.
Shield one is 1.5 times as strong as the other three shields.
.sp
.PP
`Efficiency' indicates the number of energy units being
burned per warp-second.
This number is initially one (.75 for enemy) and increases
per hit.
.sp
.PP
`Regeneration' indicates the number of energy units being
gained per second.
Initially set at 10, this number decreases per hit.
.sp
.PP
`Fuel capacity' indicates the number of matter-antimatter
pods a vessel has aboard.
This number rapidly decreases with each torpedo or probe
fired.
.sp
.PP
`Fuel' indicates the number of matter-antimatter pods which
are filled with energy.
This number rapidly decreases when maintaining high warp
speeds or firing phasers.
.sp
.LP
18.  Scan Enemy (Damage Report of Enemy).
.PP
[Ship Name | Probe id | #Ship Name]
.PP
An enemy damage report is essentially the same as the
Enterprise's.
Sensor reports can not be had for cloaked vessels.
.PP
By giving the id number of a probe, information about it can
be gathered.
The same information can be gathered about a ship's (jettisoned)
engineering by prepending a '#' before the ship's name.
.sp
.LP
19.  Alter Power Distribution.
.PP
[Shld 1 drain [* | ... Sh 4]] [Phsr 1 drain [* | ... Ph 4]]
.PP
The synopsis of this command can be confusing.
The first set of numbers gives the drains for each shield.
All four shield drains can be specified, but
if a star is used immediately after a shield drain
(eg, 19 0.5 1*),
then the remaining shields will all be given a drain equal to the
number preceding the '*'.
(Thus, in the above example, shield 1's drain is 0.5, whereas
shields 2, 3, and 4 have a drain of 1).
The same applies to the phaser drains.
.sp
.PP
The power circuits of all vessels are illustrated in
Appendix 2.
Dilithium crystals produce energy much like generators.
This power is then used to maintain warp speeds, recharge
antimatter pods in the engine reserve, recharge phaser
banks, or maintain shield power.
Your initial regeneration is ten, however, the shields normally
drain four units and the engines require one unit per each
warp-second.
.sp
.PP
Shields can be thought of as electromagnets.
The more energy put into them, the stronger their force field
becomes.
Therefore, a shield's overall percentage is calculated by
the following formula:
.sp
.ce
shield percentage = (energy in)(effective %)
.sp
.PP
Notice that dropping power to a shield has the same effect
as having it hit.
Notice also that if your regeneration drops below four,
you may have to discharge your phaser banks to maintain full
shield power.
.sp
.PP
Phaser banks, similar to batteries, not only discharge (when
firing), but also recharge.
Initially, they are set to recharge fully in one second
(+10) so that you can continually use them.
However, they can discharge fully (-10) in one second to
provide extra power to shields, warp engines, or engine
reserve.
.sp
.PP
Under most conditions, you need not concern yourself with
power distribution unless some special need arises.
Distribution, for the most part, is automatic.
Regeneration is calculated first; that power is placed in
reserve, along with any discharged phaser units.
Shield drain is calculated next, then the cloaking device,
then phaser and engine drains.
.sp
.PP
Be concerned with wasting power by indiscriminately firing
phasers and torpedos, maintaining speeds over warp three, or
dumping scores of units onto antimatter probes.
Huge power losses cannot be made up in battle.
.sp
.LP
20.  Jettison Engineering.
.PP
Although this order was never executed in the television
series, it is quite possible according to its producer.
Jettisoning engineering has serious consequences, but it may
be your only course of action.
.sp
.PP
One would jettison engineering if being pursed by vessels,
probes or torpedos, or as a suicidal gesture.
.sp
.PP
The following things happen when engineering is jettisoned:
A: You lose all your fuel and reserve capacity; B: you lose
your regeneration; C: you lose your warp drive; D: your lose
your probe launcher; E: you lose your shields until you
designate phasers to discharge; F: the engineering section
itself decelerates to a stop; G: a ten second time delay on
it is set (hopefully, when it does explode, you are far
from its effects); H: you lose your cloaking device;
I: your phasers and torpedos are now free to fire in any direction.
.sp
.LP
21.  Detonate Engineering.
.PP
[nothing | [y | n]]
.PP
This order, issued after the previous one, manually detonates
your engineering section.
It may also be issued without the previous order, in
which case you will be asked to confirm your (crazy) order.
.sp
.LP
22.  Alter Torpedo And Phaser Firing Parameters.
.PP
[[y [Launch Speed] [Time Delay] [Proximity Fuse]] | n]
[[y [Firing Percentage]] | n]
.PP
This option allows you to change the launch speeds, time and
proximity delays of all torpedos.
At the beginning of play, torpedos are launched at warp
twelve, have ten second time fuses, and 200M proximity
fuses.
Any vessel or engineering section which comes within the
proximity range will cause the torpedo to explode.
.sp
.PP
Phaser firing percentages can also be altered.
A phaser bank need not fire its full charge.
.sp
.LP
23.  Defenseless Ruse.
.PP
[e | p]
.PP
Another name for this tactic is `playing dead'.
When issued, your shields are dropped to zero, and power is
diverted to your engines or phaser banks.
Hopefully, the enemy will believe you dead and come too
close or break off their attack.
You should then be able to fire or run in the opposite
direction.
.sp
.LP
24.  Attempt Corbomite Bluff(s).
.PP
There were two corbomite bluffs in the television series;
one was against a midget operating a huge space vessel the
other was against Romulan attackers.
Both have been incorporated into this game.
Whichever bluff issued is selected randomly.
.sp
.LP
25.  Surrender, If Possible.
.PP
This order sends a message to the enemy, saying that you
wish to surrender.
The enemy will then decide whether or not take you alive.
You will have difficulty surrendering to Romulans, who have
never accepted one.
.sp
.LP
26.  Ask Enemy To Surrender.
.PP
This order ends a message to the enemy demanding that they
surrender.
Please bear in mind that Romulans and Orions are the most suicidal.
.sp
.LP
27.  Initiate Self-Destruct.
.PP
This order activates a twenty-second self-destruct sequence.
Because final destruct does not occur until ten turns after
initialization, it is best to start it early, if at all.
When you do explode, you hope that you explosive force will
also destroy your attackers.
.sp
.LP
28.  Abort Self-Destruct.
.PP
This order, issued after the previous one, halts the
destruct sequence.
Self-destruct cannot be aborted withing five seconds to
detonation.
.sp
.LP
29.  Survivors Report
.PP
This order prints out the number of survivors on board all the ships.
This order does not use a turn.
Cloaked ships are reported as having `???' survivors.
.sp
.LP
30.  Print version number
.PP
This command, which does not use a turn, prints the current version
of TREK73.
.sp
.LP
31.  Save game
.PP
This command saves the current game into a file.
It can be restarted later by using the command line option `-r'.
.sp
.LP
32.  Reprint Above List.
.PP
This command, which does not use a turn, lists code numbers
and associated descriptions of each.
.sp
.LP
33 And Up.
.PP
Future options, currently being designed in Trek74, will
include Dr. Daystrom's paranoid, M5 multi-tronics computer
which will take over while you relax; or battle someone else
who is on another terminal; or battle in teams; or have a
free-for-all against nine other players.
.sp
.bp
.ce
Options
.sp
.PP
In TREK73, all the names of crewmembers are taken from the
Star Trek series.
Through the use of options, the names, as well as other
aspects of the game, can be changed to whatever you want.
.sp
.PP
To use the options, you must add the variable TREK73OPTS to
your environment.
A sample would be (using the C-shell):
.br
.sp
setenv TREK73OPTS 'name=Jerk, ship=Boobyprize, terse'
.sp
.PP
The option string is a list of comma-separated options.
Options are designated as either boolean or string options.
Boolean options are turned on by typing their name and turned
off by prepending 'no' to them.
String options are set equal to the string which follows the "=".
.sp
.PP
There follows a list of all the options, what type it is,
and an explanation of what they mean.
The default for the option is in square brackets following
the option.
.sp
.IP "terse BOOLEAN [noterse]"
This option, when set, turns off the information regarding the
ship's purpose in the area.
It thus reduces the amount of drek on the screen.
If you are on a slow terminal, this is a nice option to have set.
.sp
.IP "shipname STRING [Enterprise]"
This option names your ship.
.sp
.IP "name STRING"
This option names the captain of the ship.
If this option is not set, then the program will ask for a name.
The captain is the one who must make all the decisions of strategy
and tactics for the ship.
.sp
.IP "sex STRING"
This option gives the captain a gender.
If this option is not set, the program will ask for it's value.
If you respond with something that starts with other than "m" or "f",
beware!
.sp
.IP "science STRING [Spock]"
This option names the science officer, who is responsible
for checking the parameters of the captain's commands.
It is also this officer's duty to report damage to the ship
as well as scan for enemy damage.
.sp
.IP "engineer STRING [Scott]"
This option names the chief engineer of the ship.
It is this officer's duty to report on the status of the ship,
especially its energy supply and distribution of the same.
The officer also controls the launching of anti-matter probes.
.sp
.IP "helmsman STRING [Sulu]"
This option names the ship's helmsman.
This officer's duty is to control the speed of the ship
and also controls the firing of the ship's weapons.
.sp
.IP "nav STRING [Chekov]"
This option names the ship's navigator, who is responsible
for the navigation of the ship.
This officer makes changes to the ship's course as directed
by the captain.
This officer also controls any anti-matter probes after they
have been launched.
.sp
.IP "com STRING [Uhura]"
This option names the ship's communications officer.
It is the duty of this officer to handle all communications between
the ship and the rest of the universe.
.sp
.IP "enemy STRING [random]"
If this option is set, it tells the program which race you wish
to fight.
The available races are:
Klingon, Romulan, Kzinti, Gorn, Orion, Hydran, Lyran, or Tholian.
If the option is not set, the race you will fight is chosen at random.
.sp
.IP "foename STRING [random]"
If this option is set, it specifies the name of the commander
of the enemy ship(s).
If this option is not specified, the name is chosen at random.
.sp
.IP "class STRING [CA]"
This option specifies the kind of ship you are commanding.
Allowable classes are: DN, CA, CL, and DD, standing for
dreadnought, heavy cruiser, light cruiser, and destroyer.
In general, the larger the ship, the more weapons and stronger
shields you have, at the cost of less speed and maneuverability.
.sp
.IP "foeclass STRING [CA]"
This option specifies the kind of ship that you are fighting.
The different classes are explained above.
.sp
.IP "silly BOOLEAN [nosilly]"
If this option is set, an additional race is added to the list of
possible races to fight.
This race is the Monty Pythons.
Note that if you wish to always fight the Monty Python's, you merely
have to set the enemy option above.
.sp
.IP "time STRING [30]"
Time is used to specify the time between commands.
The longer this value, the more time may be used in issuing a command.
.sp
.IP "teletype BOOLEAN [noteletype]"
The teletype option causes some of the output to come out as it did
in the original teletype version.
.sp
.IP "savefile STRING [$HOME/trek73.save]"
The savefile option specifies where the data image is to be stored if the
game is saved during play.  ``~'' is not expanded, so the path should be
explicit and fully expanded.
.sp 3
.ce
Command Line Options
.sp
.PP
In all cases, the arguments you place on the command line will supersede
options in the environment.
.sp
.PP
The following is a description of the command line options:
.sp
.IP \-t
Turns on terse mode.
No initial scenario description is given.
This is useful for terminals running at low baud rates.
This option is normally off.
.sp
.IP \-c
Allows the specification of the Federation captain's name.
.sp
.IP \-s
Specify the sex of the captain of the Federation vessel.
.sp
.IP \-S
Specify the name of the Science Officer of the Federation vessel.
.sp
.IP \-E
Specify the name of the Chief Engineer of the Federation vessel.
.sp
.IP \-C
Specify the name of the Communications Officer of the Federation vessel.
.sp
.IP \-N
Specify the name of the Navigator of the Federation vessel.
.sp
.IP \-H
Specify the name of the Helmsman of the Federation vessel.
.sp
.IP \-f
Specify the name of the enemy commanding officer.
.sp
.IP \-r
Specify the race of the enemy.
The race should be one of the following:
Klingon, Romulan, Kzinti, Gorn, Hydran, Lyran, Tholian, Orion,
or Monty Python.
.sp
.IP \-d
Set the delay time for command entry.
Higher times can be useful for novices or for playing on very slow
terminals.
.sp
.IP \-y
Silly option.
Adds the Monty Pythons as a possible enemy race.
This option is normally off.
.sp
.IP \-T
Teletype option.
Causes certain parts of the output to come out as they did on the
original teletype implementation.
Doesn't do much for the game on crts.
This option is normally off.
.sp
.IP \-n
Specify the name of the Federation vessel.
The default name for the Federation vessel is randomly chosen from a
set of names.
.sp
.IP \-F
Specify the class of the enemy vessel(s).
Allowable classes are Destroyer (DD), Light Cruiser (CL),
Heavy Cruiser (CA), and Dreadnought (DN).
If the argument is none of the above, the program assumes that this is
the name of a file where a player-designed ship is stored.
.sp
.IP \-l
Specify the class of the Federation vessel.
Available classes are the same as the enemy's.
.sp
.IP \-R
Restore the game from the savefile.  It is assumed that the TREK73OPTS
contains the name of the savefile, otherwise it is not possible to restart
the game with the -R option.  In case the savefile name is not in TREK73OPTS,
the game may be restored by issuing the command with the path to the savefile
as the first argument.
.bp
.ce
Designer Ships
.sp
.PP
A feature of TREK73 allows you to have more than just the standard
four ship types.
The program
.I shipyard
(6) allows new ship types to be created.
.I Shipyard
in conjunction with the -F and/or -l command line option allows
battle between, say, the Enterprise and the Death Star.
.PP
See the manual page for
.I shipyard
for more information about designer ships.
.ce
Simple Strategy
.sp
.PP
If you are a beginner, a simple strategy to follow is A:
fight only one attacker; B: pursue him (code 14) at warp
factor one; C: lock on all phasers (code 3); D: continuously
take position reports (code 12) and watch his range; E:
when he gets within 1000M, fire all phasers (code 1) and
keep on firing when he is in range; F: When the enemy is out
of range, take damage reports and scans of the enemy (codes
17 and 18).
.sp
.PP
After a few trial games using the above strategy, you will
want to become as efficient as the enemy at firing photon
torpedos.
Finally, when you master launching antimatter probes, you
can designate more that one attacker.
.sp 3
.ce
Trek73's History
.sp
.PP
Trek73 was programmed on a Hewlett-Packard 2000C system by
William K. Char, Perry Lee, and Dan Gee.
In January, 1973, Mr. Char started with a few ideas and five
months later, in May, introduced $SPACE, his first version.
.sp
.PP
Space had only 14 commands and comprised one 10K program.
Response was so great that new ideas flooded in and in June,
work on Trek73 was begun.
Over 70 recordings of past shows were reviewed to
reconstruct dialogue and vessels.
On October 8, 1973, Trek73 was introduced.
.sp
.PP
In 1984, Dave Pare at Univeristy of California at San Diego and Chris Williams
at the University of California at Berkeley independently translated
the BASIC code into C to run under BSD UNIX.\**
.FS
UNIX is a registered trademark of Bell Laboratories.
.FE
.sp
.PP
In April 1985, Jeff Okamoto and Peter Yee, both
at the University of California at Berkeley combined the two
versions into one, fixing bugs and adding new commands and concepts.
.sp
.PP
Ideas and bug reports should be sent to:
.br
ARPA: okamoto@ucbvax.berkeley.edu and yee@ucbvaxberkeley.edu
.br
UUCP: ..!ucbvax!okamoto and ..!ucbvax!yee
.bp
.ce
APPENDIX 1
.nf
.sp 4
             Secondary Hull                        Primary Hull
              (engineering)
                                              -------
:-----------------------------------------\\   | .5  |      . . . .
::           N C C - 1 7 0 1              |}  |0 / 1|---< . . . . .
::________________________________________/   |     |      . . . .
            warp engines     ^                | .5  |      . . . .
                            +++               |0 / 1|---< . . . . .
  =====                      | fuel           |     |      . . . .
   |+|             +       OOOOO reserve      | .5  |      . . . .
   |+| ---------> +++ ---> OOOOO --- +++ ---> |0 / 1|---< . . . . .
 -------           +       *****              |     |      . . . .
 |     |                   *****       shield | .5  |      . . . .
 -------                     |  \\     control |0 / 1|---< . . . . .
dilithium                    *   \\            -------      . . . .
crystal                     ***  +++                       shields
generator                    *     \\phaser control
                             |      --------                 : : :
                             |      |-10/10|-OOOO>. . : : : | : :
        <***:   <[=====| <---+      |-10/10|-**OO>. . :      : : :
        probe   launcher     |      |-10/10|-****>
                             V      |-10/10|-****>
                       =*******=    -------- phaser banks
               photon  =*******=
              torpedo  ===*******
                tubes  =======*******
                       =========  *******
                       =========      *******
+ Energy unit
O Matter-anti-matter pod
* Filled matter-anti-matter pod
---+++---> Energy transfer
---***---> Pod transfer
.fi
.bp
.ce
APPENDIX 2
.br
.ce
Heavy Cruiser
.nf
.sp 7
                                  Shield 2
                                     |
                        135            90
                          \\\\,,,,,,,,''-``,,,,,,,,
                       ,,''\\   .Phaser.Firing An``,,  45
                     ,'     \\ \\125              gles`,/
                   ,'        \\. . Torpedo.          . `,
   :------------------------\\ \\135_-----_ Fir        .  ,
   :________________________/   _-     1 -_ ing       .  `
                 |    ||       /        0  \\ Angles   .  |
          Shield |  ,_^^_____/|      _   7  | .       .  | Shield
             -180| [          {     (o)   1 } .       .  |0-
            3    |  `-vv-----\\|      -   C  | .       .  |   1
                 |    ||       \\        C  /  .       .  |
   :-----------------------\\    -_     N _-  .        .  ,
   :_______________________/      -_____-   .       .   '
                   `,       /225. . . . . .       .   ,'
                     `,    /                    .   ,' \\
                       ``,/ /235. . . . . . . . ,,''   315
                         /`````````,,_,,''''''''
                        225            270
                                     |
                                          Shield 4
.fi
.bp
.ce
APPENDIX 3
.sp 2
.ce
Weapon And Vessel Specifications
.ce
Destroyer Class Vessel
.ce
Enemy exceptions are enclosed within [brackets]
.sp
.DS
.ta 2.8iR 3.3i
.ce
Phasers
	Number of banks	2
	Max range	1000 megameters
	Max spread	90 degrees (45+45)
	Min spread	20 degrees (10+10)
	Max hit with 45 degree spread	10
	Max hit with 10 degree spread	45
	Loss of shield 1 per hit	hit/3
	Loss of shields 2-3-4 per hit	hit/2
	Firing angles with engineering	0-150, 210-360 degrees
	Max charge per bank	10 units
	Min charge or discharge time	1 sec
.sp
.ce
Tubes
	Number of tubes	4
	Max range	12,000 megameters
	max launch speed	warp 12
	Max time delay	10 seconds
	Max proximity delay	500M
	Max explosion radius	500M
	Max number of pods launched	10
	Max hit factor	50
	Loss of shield 1 per hit	hit/2.25
	Loss of shields 2-3-4 per hit	hit/1.5
	Firing angles with engineering	0-160, 200-360 degrees
.sp
.ce
Probes
	Number of probe launchers	1
	Max range	3000M
	Max launch speed	2
	Max time delay	15 sec
	Max proximity delay	any
	Max explosion radius	50 times number of pods
	Max pods launched	fuel available
	Max hit factor	10 times number of pods
	Loss of shields per hit	same as torpedos
	Firing angles with engineering	all
.sp
.ce
Vessels
	Max turning rate	120 degrees
	Max speed	warp 10 [12]
	Min units burned per warp-second	.5 [.5]
	Crew	200 [150]
.DE
.bp
.ce
Weapon And Vessel Specifications
.ce
Light Cruiser Class
.ce
Enemy exceptions are enclosed within [brackets]
.sp
.DS
.ta 2.8iR 3.3i
.ce
Phasers
	Number of banks	4
	Max range	1000 megameters
	Max spread	90 degrees (45+45)
	Min spread	20 degrees (10+10)
	Max hit with 45 degree spread	10
	Max hit with 10 degree spread	45
	Loss of shield 1 per hit	hit/4.5
	Loss of shields 2-3-4 per hit	hit/3
	Firing angles with engineering	0-140, 220-360 degrees
	Max charge per bank	10 units
	Min charge or discharge time	1 sec
.sp
.ce
Tubes
	Number of tubes	4
	Max range	12,000 megameters
	max launch speed	warp 12
	Max time delay	10 seconds
	Max proximity delay	500M
	Max explosion radius	500M
	Max number of pods launched	10
	Max hit factor	50
	Loss of shield 1 per hit	hit/3
	Loss of shields 2-3-4 per hit	hit/2
	Firing angles with engineering	0-150, 210-360 degrees
.sp
.ce
Probes
	Number of probe launchers	1
	Max range	3000M
	Max launch speed	2
	Max time delay	15 sec
	Max proximity delay	any
	Max explosion radius	50 times number of pods
	Max pods launched	fuel available
	Max hit factor	10 times number of pods
	Loss of shields per hit	same as torpedos
	Firing angles with engineering	all
.sp
.ce
Vessels
	Max turning rate	77 degrees
	Max speed	warp 9 [11]
	Min units burned per warp-second	.75 [.5]
	Crew	350 [250]
.DE
.bp
.ce
Weapon And Vessel Specifications
.ce
Heavy Cruiser Class
.ce
Enemy exceptions are enclosed within [brackets]
.sp
.DS
.ta 2.8iR 3.3i
.ce
Phasers
	Number of banks	4
	Max range	1000 megameters
	Max spread	90 degrees (45+45)
	Min spread	20 degrees (10+10)
	Max hit with 45 degree spread	10
	Max hit with 10 degree spread	45
	Loss of shield 1 per hit	hit/4.5
	Loss of shields 2-3-4 per hit	hit/3
	Firing angles with engineering	0-125, 235-360 degrees
	Max charge per bank	10 units
	Min charge or discharge time	1 sec
.sp
.ce
Tubes
	Number of tubes	6
	Max range	12,000 megameters
	max launch speed	warp 12
	Max time delay	10 seconds
	Max proximity delay	500M
	Max explosion radius	500M
	Max number of pods launched	10
	Max hit factor	50
	Loss of shield 1 per hit	hit/3
	Loss of shields 2-3-4 per hit	hit/2
	Firing angles with engineering	0-135, 225-360 degrees
.sp
.ce
Probes
	Number of probe launchers	1
	Max range	3000M
	Max launch speed	2
	Max time delay	15 sec
	Max proximity delay	any
	Max explosion radius	50 times number of pods
	Max pods launched	fuel available
	Max hit factor	10 times number of pods
	Loss of shields per hit	same as torpedos
	Firing angles with engineering	all
.sp
.ce
Vessels
	Max turning rate	55 degrees
	Max speed	warp 9 [11]
	Min units burned per warp-second	1 [.75]
	Crew	450 [350]
.DE
.bp
.ce
Weapon And Vessel Specifications
.ce
Dreadnought Class
.ce
Enemy exceptions are enclosed within [brackets]
.sp
.DS
.ta 2.8iR 3.3i
.ce
Phasers
	Number of banks	6
	Max range	1000 megameters
	Max spread	90 degrees (45+45)
	Min spread	20 degrees (10+10)
	Max hit with 45 degree spread	10
	Max hit with 10 degree spread	45
	Loss of shield 1 per hit	hit/7.5
	Loss of shields 2-3-4 per hit	hit/5
	Firing angles with engineering	0-125, 235-360 degrees
	Max charge per bank	10 units
	Min charge or discharge time	1 sec
.sp
.ce
Tubes
	Number of tubes	8
	Max range	12,000 megameters
	max launch speed	warp 12
	Max time delay	10 seconds
	Max proximity delay	500M
	Max explosion radius	500M
	Max number of pods launched	10
	Max hit factor	50
	Loss of shield 1 per hit	hit/6
	Loss of shields 2-3-4 per hit	hit/4
	Firing angles with engineering	0-135, 225-360 degrees
.sp
.ce
Probes
	Number of probe launchers	1
	Max range	3000M
	Max launch speed	2
	Max time delay	15 sec
	Max proximity delay	any
	Max explosion radius	50 times number of pods
	Max pods launched	fuel available
	Max hit factor	10 times number of pods
	Loss of shields per hit	same as torpedos
	Firing angles with engineering	all
.sp
.ce
Vessels
	Max turning rate	30 degrees
	Max speed	warp 8 [10]
	Min units burned per warp-second	2 [1.5]
	Crew	600 [450]
.DE
.bp
.ce
APPENDIX 4
.sp 2
.ce
Initial Settings
.ce
Destroyer class
.sp 1
.ce
Enemy exceptions are enclosed within [brackets]
.sp
.DS
.ta 2.8iR 3.3i
	Crew	200 [150]
	Speed	Warp 1
	Course	0 [0-360] degrees
	Engine Efficiency	.5 [.5]
	Fuel Level	100
	Fuel Capacity	150
	Regeneration Rate per Second	8.0
	Helm Lock	None [None]
	Phaser Deployment	0, 0
	Phaser Bank Levels	2 x 10 units
	Phaser Charge/Discharge Rates	2 x 10 units
	Shield Drain	2 x 1 units
	Shield Percentage	2 x 100
	Torpedo Deployment	60, 0, 0, 300
	Tube Levels	4 x Zero
	Total Charged Pods Available	190
.sp
.DE
.ce
Initial Settings
.ce
Light Cruiser Class
.sp 1
.ce
Enemy exceptions are enclosed within [brackets]
.sp
.DS
.ta 2.8iR 3.3i
	Crew	350 [250]
	Speed	Warp 1
	Course	0 [0-360] degrees
	Engine Efficiency	.75 [.5]
	Fuel Level	125
	Fuel Capacity	175
	Regeneration Rate per Second	10
	Helm Lock	None [None]
	Phaser Deployment	90, 0, 0, 270
	Phaser Bank Levels	4 x 10 units
	Phaser Charge/Discharge Rates	4 x 10 units
	Shield Drain	4 x 1 units
	Shield Percentage	4 x 100
	Torpedo Deployment	60, 0, 0, 300
	Tube Levels	4 x Zero
	Total Charged Pods Available	190
.sp
.DE
.bp
.ce
Initial Settings
.ce
Heavy Cruiser Class
.sp 1
.ce
Enemy exceptions are enclosed within [brackets]
.sp
.DS
.ta 2.8iR 3.3i
	Crew	450 [350]
	Speed	Warp 1
	Course	0 [0-360] degrees
	Engine Efficiency	1 [.75]
	Fuel Level	150
	Fuel Capacity	200
	Regeneration Rate per Second	10
	Helm Lock	None [None]
	Phaser Deployment	90, 0, 0, 270
	Phaser Bank Levels	4 x 10 units
	Phaser Charge/Discharge Rates	4 x 10 units
	Shield Drain	4 x 1 units
	Shield Percentage	4 x 100
	Torpedo Deployment	120, 60, 0, 0, 300, 240
	Tube Levels	6 x Zero
	Total Charged Pods Available	190
.sp
.DE
.ce
Initial Settings
.ce
Dreadnought Class
.sp 1
.ce
Enemy exceptions are enclosed within [brackets]
.sp
.DS
.ta 2.8iR 3.3i
	Crew	600 [450]
	Speed	Warp 1
	Course	0 [0-360] degrees
	Engine Efficiency	2 [1.5]
	Fuel Level	200
	Fuel Capacity	200
	Regeneration Rate per Second	15
	Helm Lock	None [None]
	Phaser Deployment	90, 90, 0, 0, 270, 270
	Phaser Bank Levels	6 x 10 units
	Phaser Charge/Discharge Rates	6 x 10 units
	Shield Drain	4 x 1 units
	Shield Percentage	4 x 100
	Torpedo Deployment	120, 60, 60, 0, 0, 300, 300, 240
	Tube Levels	8 x Zero
	Total Charged Pods Available	190
.sp
.DE
.bp
.ce
APPENDIX 5
.sp 2
.ce
Designer's Notes
.sp 2
.PP
When I was about 10 or 11, I would go to the Lawrence Hall of Science
quite frequently.
There, on their time-sharing system, I would sit at a Teletype Model 33
and play trek.
.PP
Sadly, LHS replaced their system and trek went away.
It was my intention that it should not be forgotten.
I had had a copy of a slightly different version of trek written in BASIC,
but it was sadly unimplementable on my Apple computer.
.PP
When I learned of an implementation of trek written in C, I jumped at
the chance to bring it to the systems here at UC Berkeley.
At that time, the game was mostly a direct translation of the original
BASIC source.
Many of the commands and routines were either missing or faulty.
With Peter Yee, we worked together to fix up the program and managed
to get it running.
.PP
Once it was running, the time came for improvements.
I admit to pulling many concepts from the game Star Fleet Battles, most
notably the different races (Hydrans, Lyrans, etc.) and the ship names.
.PP
This version of the game represents many hours of thinking and debugging.
I hope you enjoy playing it as much as I did coding it.
.sp
.IP " " 30
-Jeff Okamoto
.sp 2
.PP
My first experience with TREK73 was also at the Lawrence Hall of Science.
I had been taking classes in Time-Sharing Basic and noticed that other
people always talked about a game called $TREK that was a real CPU hog
and was usually turned off.
Naturally I was intrigued.
Soon I was paying $2.00 an hour for the chance to play that game.
Many long hours and quite a few dollars went into playing $TREK,
so it was with a certain sadness that I learned that the DG Ecllipse on
which $TREK ran was being phased out.
I made several attempts to obtain the source before it went away, but
I was unable to get it.
.PP
Fortunately for me, Dave Pare at UC San Diego was also a fan of the game,
and more importantly, he had an outdated copy of the source from an
HP 2000.
Dave had started to implement the game in C to run under 4.2 BSD UNIX.
Expressing my interest to Dave, I was able to get a copy of Dave's code
and thus the TREK Project at Berkeley was started.
I spent endless hours tweaking with Dave's code, implementing some of
the fifteen or so commands that he had not yet translated.
.PP
At about this time, I learned that Christopher Williams, here at Berkeley,
had also tried to implement the game in C.
What is more, he had a copy of the source (in BASIC) from Berkeley
High School, and had implemented most of the commands.
Merging the work that Chris had done into my copy of Dave's work led
to a fairly complete version of the game.
There still remained a large number of bugs, poor ideas, and outright
mistakes in the code, but it ran.
.PP
Jeff Okamoto, being a fan of the game and a Star Fleet Battles
player, was greatly interested in hacking on the game to bring up to
par with the version that ran at LHS and to extend it even beyond
that.
Thus our partnership was formed and the current version of the game
represents several hundred hours of our joint work (and play).
Also represented are the suggestions, modifications and bug fixes
we received from numerous people, including (to name a few)
Matt Dillon, David Sharnoff, Joel Duisman, all at Berkeley, and
Roger Noe at Rockwell International.
.PP
It is hoped that this implementation of a classic game will bring joy
(and perhaps fond remembrances) to all who play.
.sp
.IP " " 30
Live Long and Prosper,
.br
-Peter Yee
