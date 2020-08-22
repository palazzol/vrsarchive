@make(report)
@pageheading(center "Version 1.24")
@begin(titlepage)
@begin(titlebox)
@majorheading(CARNEGIE-MELLON UNIVERSITY)
@heading(Computer Science Department)
@heading(VLSI Group)
@center(A. Nowatzyk)



@b(PCB - A Printed Circuit Board Editor)

@value(date)
@end(titlebox)
@blankspace(4cm)
@begin(researchcredit)
Disclaimer: The program described in this manual was written to aid
hardware design projects at CMU. No effort was made to insure portability
and no commitment is made to support this program for other
environments. This program may be distributed freely provided that no
commercial interests are involved and that the copy-right notices are
not removed from the program sources.
@end(researchcredit)
@end(titlepage)
@chapter(Overview)
PCB is an integrated router and editor to design printed circuits. It
adopts the @i(correct by construction) principle, that is it will enforce
a certain set of design rules (such as cooper traces have to maintain a
minimal separation, holes have to be aligned etc). PCB maintains a data base
of the desired circuit so that it can make sure that signals of different
net cannot be shorted.

Building a PC board usually involves a number of steps, such as placement
of the components, routing the various connections, preparing a geometry
file for the photographic masks, adding design changes. PCB aids these steps
by:
@begin(itemize)
Placement aids: Components can be moved on a graphic screen. For a given
placement, the wire densities can be displayed to give hints on how to improve
the placement.

Automatic router: Depending on the complexity and density of the circuit, PCB
can route up to 95% of all connections without human intervention.

Manual editing facilities: Wires can be entered with a pointing device like a
mouse or graphics tablet. Existing wires can be moved without loss of
connectivity and the router can be used interactively on a certain connection,
on a certain net or on a certain area of the board. There are commands to
erase wires on a individual basis or on entire areas.

Output facilities: PCB can generate a CIF file that complies with the MOSIS
design rules for 4 layer boards. It takes care of power and ground
connections, can improve the wires by removing unnecessary jogs, adds fillets
to holes in order to ease the removal of components and calls external
supplied pieces of geometry. In addition, topological bitmaps can be plotted
directly on a Versatec plotter which is much faster that using CIF for
intermediate plots.

Change facilities: The internal database can be updated in order to add or
remove nets (a list of points that should form an electrical connection),
components (a particular IC or any other actual device on a PC board)
or types (a generic description for a class of components, say the layout
of an 7400) without rebuilding the entire board.
@end(itemize)
PCB is mainly an interactive program (the router and the wire improver can
also run as a background job without a graphics terminal) that is almost
completely controlled by the mouse / graphics tablet. This has the advantage
that the user does not have to switch between keyboard and mouse. The
disadvantage is that the 4 keys of the mouse are used for a large number of
functions that change according to the current context which causes some
confusion to new user.

The basic data structure of PCB is a bitmap. This turned out to be a very
robust data structure as it survived a great many bugs without loosing work.
It also supports most functions efficiently without performance degradation
for large designs. The disadvantage is the size: both memory and disk space
requirements are high (1-3 Mbytes) with corresponding I/O time demands which
are noticeable during startup and save operations.

The graphics terminal is used extensively. It turns out that a useful
display for a PC board need considerably more objects than a typical
VLSI design. This rules out frequent redraws and calls for incremental
updates of the graphic screen. The hardware of the AED-512 terminal supports
zoom and pan functions and PCB uses them. Unfortunately, the new generation
of Berkeley VLSI design tools decided to reprogram the AED and removed
the pan option, so PCB runs only on AED with the original set of ROMs.

@chapter(Getting Started)
This chapter tries to explain the functions of PCB by describing how to
perform typical PC board editing operations. The next chapter will provide
a list of PCB features that is intended as a reference for the more
experienced user.

@section(Invoking PCB)
Normally, PCB needs on a AED-512 color graphic terminal that should connect
to the host over a serial line. Direct lines are necessary because most
graphic commands are send as 8-bit binary characters (RAW mode) that tends
to confuse the ethernet front ends. PCB resides in /usr/vlsi/bin on the
@i(UNH) and @i(VLSI) VAX. It should be started on the normal CRT terminal
next to the AED. PCB searches a data base that locates a AED display
for a given terminal. If this search fails, the user is prompted for the
device name of the AED (say @i(/dev/tty09)). Because PCB reads the
coordinates from the graphic tablet that is attached to the AED, it is
necessary to run @i(sleeper) on the AED (see the unix manual entry for
@i(sleeper)). @i(sleeper's) only function is to lower the read-protection
of the AED terminal line. If PCB or some other program that uses the AED
is killed or core-dumpes, the AED terminal line may not be released. In this
case it is necessary to stop @i(sleeper) (type control-L twice on the AED
after a reset) and restart it.

When PCB is started, it will ask for a @i(checkpoint file). This file
is produced by earlier PCB runs and contains the entire PC board description
along with all work that was done so far. @i(Checkpoint files) are binary,
large and do not change their size. It is necessary to keep them in order
to make changes to a completed board (unless you like to re-build it from
scratch). A @i(checkpoint file) is usually named 'pcb.SAV' and PCB tries to
locate it in the current directory.

In case of a new board, PCB will ask for the board size (in metric
centimeters), a scale factor (which applied to the @i(type) and
@i(component) files) and for three files that describe the circuit:
@begin(itemize)
The @b(type) file, that contains descriptions of the used devices (say the
pin-locations of a generic 7400).

The @b(component) file, that contains the name and types of all devices
of the desired PC board (say a U1 of type 7400 and a U2 of type 7400).
This file may also contain placement information, that is where a particular
device is located on the board.

The @b(netlist) file, that describes all electrical connections that should
be made (say pin#2 of U1 is connected to pin#3 of U2).
@end
How these files are produced is beyond the scope of PCB. They could
be created with a normal text editor or (preferably) with some schematic
capture system (like @i(CLEO), @i(DP), @i(SIL), the Daisy workstation etc.).
Note that an error in these files will guarantee that the resulting board is
in error (GIGO principle applies). It is desirable that the circuit is
tested before a PC board is build (say a simulation or a wire-wraped
prototype).
@section(The Type Definition File)
PCB tries to use the default file 'test.ty' in the current working directory
before the user is prompted for an alternate file.
The @i(Type Definition) file starts with the line "*types:", which is checked
to be sure that it is a type file. The remainder of the file is expected
to contain an unordered sequence of type descriptions. Each description
starts with the name of a component followed by a ":" in a line (for example
"7400:"), optional items, a list of pin descriptors and an empty line which
is used to terminate one type description.

Each pin is characterized by its location in a right-handed
coordinate system that uses units of 0.1" times the scale factor.
The X and Y coordinates
must be separated by a blank, tab or comma and can be followed by a pin name.
There are 4 reserved pin names: VCC, VEE, VTT and GND that will be connected
to power planes. PCB will ignore all other pin names (not the pins!).

Naming a pin with VCC, VEE, VTT or GND will cause PCB to connect that pin to
the corresponding power supply layer. Currently, no distinction is made
between VCC, VEE and VTT: they all connect to the inner 'power' layer, while
GND uses the other inner layer. It is expected that designes that uses
multiple power supplies uses some other tools (say @i(CEASAR)) to add the
propper cuts to the 'power' layer (this feature may be added when there is a
need for more than one power supply).

Comments may be appended to any line: a ';' can be used to terminate a line
which causes PCB to skip to the first character of the next line.

There are 3 optional statements that can be used to describe @i(external
devices). Normal devices are taken care of by PCB, which includes the
generation of holes, pads, solder mask cuts, fillets and a silkscreen box
that has the name, an orientation mark and a pin-1 mark. There are however
devices like switches, power pads, edge connectors, text-logos etc. that
need special attention. These devices are declared 'external'. PCB will
not produce geometry for them. Instead, a call to a CIF symbol is inserted
in the output file. External devices may have pins that can be used like
normal pins. PCB will run a copper trace to the center of the pin location
and the external CIF description has to take care of a hole or any other
item that needs an electrical connection. It is also possible to declare
taboo zones for an external device that will make sure that no wire will
enter these areas (say the center of a large mounting hole). The optional
statements must start with a keyword at the begin of a new line:
@begin(itemize)
@b(EXTCIF: <n>) - This defines a type to be extern and associates it with
a CIF-symbol number @i(n). To prevent confusion with PCB's symbols, this
number must be in the range 500-999. This statement must preceed other
options.

@b(BOX: <x_length> <y_length>) - This defines a box that must contain the
entire external geometry. The external symbol is expected to be in the first
quadrant (only positive coordinates). The box will be display on the screen
when the device is placed. Devices may have no pins (say a text string).

@b(BLOCK: <x_low> <y_low> <x_high> <y_high>) - This defines an area that
may not be used for wires, pins or via-holes. The router and designrule
checker will use this information to keep that part of the board free of
electrical signals. A type definition may have any number of block
statements. They will be displayed in the color of the PC board frame.
@end(itemize)
@section(The Component Definition File)
PCB tries to use the default file 'test.cp' in the current working directory
before the user is prompted for an alternate file.  The @i(Component
Definition) file starts with the line "*components:" and contains a
unordered sequence of lines, each one describing the instantiation of a
component of a previously defined type.  Each line has 2 or 5 fileds
separated by blanks, tabs or commas (no separator at the beginning of the
line): The instantiation name, the type name and optional: the x and y
coordinates of the component origin (right handed system, in units of 0.1" *
the scale factor) and its orientation (an integer 0-3, that specifies counter
clockwise rotations in 90 degree increments starting at 0 for no rotation).
Pins may not overlap or lay outside of the board. Components that are not
placed or that were assigned to an invalid place will cause PCB to add
a placement phase before routing is attempted. During this placement phase,
the user can interactively place devices on the board. PCB will compute
total wire length, wire densities and congestion areas for a given placement
which is intended to aid finding a good placement. The placement is not
final: even after wires were routed, components can be moved to better
places. Of all input information to pcb, the components location part
is the least critical: a poor placement can make it harder to route a board,
but it would still work (of cause there might be a critical clock signal
that does not like a long wire...).
@section(The Netlist File)
PCB tries to use the default file 'test.nl' in the current working directory
before the user is prompted for an alternate file.  The @i(netlist) uses
either the @i(Multiwire) netlist format with type 3 pin designators or a
@i(DP) netlist format. The first format was chosen to allow the use of
@i(Multiwire)
or pc-boards without the need for different netlist formats. The second
format was added because it is supported by several local tools. The netlist
file has to start with "Cnetlist:" for a @i(Multiwire) file or with
"DPnetlist:" for @i(DP) netlists.

Any input file may have comments that are preceded by an semicolon. In case
of netlist, the multiwire convention of a 'C' at the beginning of the line
is also supported. ";"-Comments in @i(Multiwire) netlists should be
restricted to columns 73-80 to avoid problems with multiwire submissions.

Examples for both netlists are given in the appendix. Nets that are named
"GND", "VCC", "VEE" and "VTT" received a special processing: they may appear
multiple times in the file as they tend to be large and they need no routing
as the connections are made directly to the ground/power plane. Warning: if
a pin of an external type is connected to a power net, PCB will @b(not)
perform the connection because the external geometry is not accessible to
PCB. The user must take care of these cases when he designs the external
type.
@section(Output Files)
PCB produces a CIF file called "pcb.cif" (if it already exits, the user will
be prompted for an alternate file name), that complies with the MOSIS
specification for pc-board cif-files. CIF-files tends to be large and
plotting a cif-file takes a considerable amount of time (oder of hours).
The plot preview command can be used to plot an approximation to the final
board that can be used to plan routing. This command is much faster
(order of one minute) because it simply dumpes the bitmap on the Versatec
plotter.

PCB also writes a file called "pcb.SAV" (binary), which contains the bit map
of the edited pc-board. This file has constant size depending on the size
parameters used for compiling PCB. The bit map is necessary to modify
pc-boards later on. There is no (and probably will be no) way to reconstruct
the bit map from the CIF-file. In addition to the bit map, it contains the
entire data base used by pcb. A version of this file is written after
editing 25 nets to avoid inconveniences in cases of system crashs or program
bugs. PCB has a fair amount of internal consistency checks. In case of such
problems (due to bugs or memory errors), the bit-map will be written to
"pcb.ERR" for problem analysis and recovery (see the chapter on @i(trouble)).

The @i(list) commands can be used to write a version of the current @i(type),
@i(component) and @i(netlist) files. These file are generated from the
internal data base and will look different than the original ones (other
spacing, no comments, case-folded, scale-factor = 1), but they do contain
the very same information. The @i(component) file will have the most recent
placement information that differs from the original in cases when components
were moved.
@section(Edit Commands)
All frequently used commands are activated by pressing one of the 4 mouse
buttons. Their function depends on a @i(context) and on the location of
the cursor when the key is pressed. The @i(context) is displayed in the
top, left corner of the screen (it will disappear for highly enlarged
views because the the text would not fit on the screen otherwise). The
 @i(context) can be viewed as the state of a finite state machine that is
used to select one of several functions that are bound to the mouse keys.
This is very similar to an electronic watch were 4 keys can control a large
number of functions (setting times, dates, display modes, alarms, countdowns
etc.). Once this scheme is understood, the mouse becomes a very effective
input device.

The top line of the AED is used to display information and error messages.
It is divided into 3 fields: the current context (left), the number of
unfinished nets and a general purpose area. Significant messages or messages
that are issued after a long operation will also send a 'Bell' character to
the normal terminal.

As mentioned before, the cursor location is also used to disambiguate between
different functions. In all contexts, a key pressed while the cursor is
@b(outside) of the screen, has a different function. This is normally used
to ask for menus and to change the display magnification. There are more uses
of cursor dependent functions that will be explained below.

The association between functions and keys tries to be consistent (sort of).
Pressing a key while the cursor is positioned on the screen will usually
use the cursor coordinates:
@begin(description)
@b(Yellow)@\is normally used to select something.

@b(White)@\is normally used to move the center of the screen to a new place,
that is to look at a different section of the board.

@b(Blue)@\is normally used to supply some coordinates. For example the
lower left corner of a box, the direction/length of a vector, or the
location for some object.

@b(Green)@\is normally used to initiate some operation. This is the most
'dangerous' key, as the operation usually causes significant changes, for
example deleting objects.
@end(description)
Pressing keys while the cursor is not on the screen cannot supply a
coordinate. It is used as an escape for more global functions:
@begin(description)
@b(Yellow)@\usually increases the display magnification (zoom in).

@b(White)@\usually calls the function menu.

@b(Blue)@\usually decreases the display magnification (zoom out).

@b(Green)@\usually calls the parameter menu.
@end(description)
The are exceptions to these assignments, but it gives a rough guidline
on how the mouse is used.
@subsection(Placing a Component)
Once PCB has read the input files, it will display the current view of the
board on the AED screen along with a full-screen cross hair cursor. The
entire board may not fit on the screen so that you can see only the lower
left portion of it. The cursor may be used as a ruler so that positions
can be aligned easily to other objects on the board.

Once the cursor is displayed (it should follow the mouse), PCB is waiting
for a command. If the context is set to "@b(CP_place)" for @i(component
place), one or more components are not placed on the board yet. If the
context is "@b(Start)", all components ar placed and PCB is ready to
route wires (see below).

Along with the "@b(CP_place)" message, the name and type of one unplaced
component is displayed and a component box is drawn on the center of the
screen. This box represents the component that needs to be placed.  The blue
key will cause PCB to move the box to the current cursor location, provided
that it is on the screen. The corresponding coordinates will appear in the
top line. This command should be used to move the box to the intended place.
The component box can be rotated by pressing the green key while the cursor
is outside of the screen (don't go too far outside because the tablet will
stop polling the keys when it can't locate the mouse). Once the component
is correctly placed, the yellow key - pressed while the cursor is on the
screen - will actually insert it. This may fail as pins may not overlap with
other pins, holes must be aligned to a drill grid, pins may not lie outside
of the PC board, pin and forbidden areas (in case of external component) may
not touch existing wires etc. A error message will be issued if the command
could not complete.

PCB selects the components that needs to be placed in the order they were
defined. However, this may not be the most convenient order to place them.
The green key - pressed with the cursor on the screen but not pointing to
an placed component - is used to cycle through the currently unplaced
components. The same key is used to @b(delete) components when the cursor
is pointing to an placed component. Deleting something in PCB will cause
the object to reappear later, so deleting a component will cause it to be
entered to the list of unplaced components. To delete something permanently,
a change to the data base is necessary (see the section on ECO's).

Keys not mentioned so far have their default functions: The white key moves
the screen to other portions of the board (while the cursor is on the
screen) or call the function menu (while the cursor is not on the screen).
The yellow and blue keys combined with an off screen cursor cause a zoom in
or out.
@subsection(Changing the Placement)
Once all components are placed, PCB will advance to the "@b(Start)" context
which is intended for manual routing of wires. "@b(Start)" and most other
contexts will assume a stable component placement. In order to change the
completed placement, the function menu provides the "@b(Move component)"
option.  Pressing the white key while the cursor is not on the screen will
call the function menu. Items are selected by moving the cursor on the menu
text and pressing any key (if the cursor does not point to an item nothing
will be selected).

Selecting "@b(Move component)" will cause a the context to be changed to
"@b(CP_move)". Now the yellow key will select a component for moving when
the cursor points to a component. The component box will appear on the old
position and the name/type is displayed. The blue key is used to designate a
new position. The white/green key cause left/right rotations when used with
the cursor not on the screen. The green key - cursor on the screen - will
terminate the command by moving the component to the new place, provided
that it is legal. Component that have wire attached to them cannot be moved,
the wires must be removed first. The command will have no effect if no
component is selected.
@subsection(Routing a Wire)
When the "@b(Start)" context is entered, PCB will pick a net that need to
be connected. The pins of this net will change their color to a flashing
purple. If the net is partially done, its wire will be highlighted.

Clicking the yellow or green key on any part of the selected net will remember
that point as the start point for a new copper trace. The context will
change to "@b(Route)". If these keys are pressed on an other net, the current
one will be droped and the new one selected. If the new net is not completed,
the "@b(Route)" will be entered.

Once in "@b(Route)", the yellow/blue keys will put down copper trace for the
component/solder side. A wire is drawn from the last location to the current
cursor location. A via-hole is inserted whenever sides are changed.
If via-holes are placed close to an other hole, the gap may become too small
to allow a wire to be routed between these hole. PCB will insert block
marks around these hole that have the same color as via holes. These blocks
are permissible only to wires that belong to corresponding holes. The
design rule checker will prevent any illegal connection. It will also
prevent 2 wires of the same net but on different sides to cross without an
connection (via hole). This is not a design-rule, but a requirement of
PCB's data structure (see pparm.h for an explanation if you are curious).

Normally, the "@b(Route)" context will terminate when the new wire touches
other parts of the net to form a connection. It is legal to lay out loops
or circles. To partially route a wire (that is let it end in open space)
do a 180 degree turn.

The green key may be used to undo the most recent straight piece of trace
(segment). However, the cursor must not point to any wire when the key is
pressed: it would delete it otherwise. That is, the delete function undoes
the most recently routed piece of wire if it cant find anything else to
delete. Note that the cursor must be on the screen for this operation or you
would call the parameter menu.

In order add a little more confusion, the yellow/blue keys will display
a map that has the board, the currently visible fraction of it and the
pins of the current net when they are pressed in the "@b(Start)" context
while no net its next to the cursor location.

And finaly one more feature: clicking the yellow/green key in the "@b(Start)"
context @b(twice) while pointing to a wire without moving the cursor will
invoke the plow/straight function (see below).
@subsection(Deleting Wires)
Wire can be deleted by pointing the cursor to an unselected wire segment
and pressing the green key in the "@b(Start)", "@b(Route)" or "@b(Edit)"
context. The segment will be removed along with all wires that lose their
connection to a pin. In general, all wires must have at least one connection
to a component pin. PCB will not allow wires that have no connection to a
net. This feature can be used to delete long wires: by removing the first and
last segment, the entire wire will disappear.

Occasionally, it is necessary to delete all wires in a certain area of the
board. The "@b(Area-Delete)" option of the function menu can be used to
delete all wires in a rectangular area. The area-delete option will cause
a switch to the "@b(AR_Del)" context and will draw a rectangle in the
center of the screen. The yellow / blue keys are used to define the upper,
right / lower, left corner of the rectangle. Once the rectangle is position
over the desired area, the green key will execute the delete operation.
During the "@b(AR_del)" context, the white/green keys - when the cursor is
not on the screen - will call a menu that can restrict the delete operation
to one side of the board only. This menu is also used to abort the delete
operation.
@subsection(Connection to Power/Ground)
Power and ground connection are special because they use the inner layers of
the board. Normally this is used to provide power to a chip. For example,
all 7400 in a design need ground at pin 7 and +5V (= Vcc) at pin 14. The
pin names @b(GND) and @b(Vcc) in the @i(type) file will cause these
connections for every instantiation (that is for all 7400's in the design).
Because these connections become part of a CIF symbol that PCB generates,
this is the most space saving way to specify the power supply.

There are ground/power connections that are needed only for one particular
device (say one nand gate of a 7400 is used as an inverter and the unused
input should be tied to Vcc). Because the type file would affect @b(all)
devices of a given type, it is not used to specify individual connections.
These connections are specified by adding the pin to the ground or power net
in the @i(netlist) file. This will cause a certain pin of a particular
device to be connected directly to ground or power.

Finally, it is sometimes necessary to make a power/ground connection with
a wire on one surface layer: for example to form a jumper that can be cut.
An other example are ground traces between 2 signal wires that are used to
reduce cross talk. These wires are listed as normal nets. They can either
use a power or ground pin of a nearby device or a special devices can be
added that has just one pin with a connection to the desired plane.
PCB will issue a warning in these cases because normally nets are not
expected to connect to ground or power, but it will treat this net like any
other.
@subsection(Using a Router)
PCB has 3 built-in routers with low, medium and high search capabilities
(say router L, M and H).
Unfortunately is the CPU hunger proportional to the capabilities. Normally,
the M router is in use: it looks for a rough path first and uses a maze
runner that will be confined to the rough path. The confinement reduces the
search space for the maze runner so that it runs faster, but it may also
overlook a possible connection. The H router can be configured to find
a path whenever one exist. The L router considers only traces that run on
one side only and have a maximum of 2 jogs, which uses practically no CPU
time at all.

There are 4 ways to invoke the router: as a batch job that tries to route the
entire board, on a certain area of the board, on a certain set of nets or
on the current trace. The batch operation will be described later.

The "@b(Area route)" option on the function menu will call the router on all
wire segment that start and end in a specified rectangle. This function is
similar to the area-delete function, only wires are added instead of deleted.

The "@b(Learn Sequence)" option of the function menu will select nets and
stores them in a list (see below). "@b(Route sequence)" will call the router
on all nets in the current list.

Finally, the router are called whenever the cursor is near a component pin
in the "@b(Route)" context. Often it is convenient to route a part of the
wire and to let the router finish it.
@subsection(Wide wires)
PCB is mainly intended for digital logic boards that uses only one copper
trace width for all signal. Occasionally, it is however necessary to add
larger wires (say for power supply lines). This is done by expanding a
normal wire with the "@b(Expand wire)" option on the function menu.
PCB is compiled for 3 width: 0.013", 0.038" and 0.063" (the routine
'expand' in pwide can be changed for wider wires). Expanding a wire will
not cause shorts or other design rule violations: the actual size is limited
by the geometric constraints of the actual layout.

Wires should be expanded after all other work is done, in particular after a
wire staightening phase (it would undo most of the expansion). Expanded
wires cannot be expanded further and it takes several delete operations to
remove an expanded wire (delete assumes success when it removed one
segment).  Expanded wires will appear as a collection of several overlapping
small wires in the CIF output.
@subsection(Planning the Work)
When PCB is routing an entire board (batch operation), it has no knowledge
on which wires may be critical (say a clock distribution tree) or which
may form a bus that should run similar paths (to minimize skew). In these
cases it is useful to route the special wire manually and use the
auto-route facilities on the partially routed board.

The auto-router has its own way of planning the work: it assigns a cost
function to each net that will determine the order in which routing is
attempted (long nets will be routed before short ones). It then attempts to
route the nets that pass a certain filter and that can be completed with
a certain resource limit (via-hole and wire length). There are 5 paths
that differ in the filter (becomes more permissive) and the resource limits
(become more generous).

Manual routing needs also some planning. In order to minimize the graphic
screen updates, PCB preferes nets in the current display area. This
preselecting of nets to route can be changed to a user-defined sequence.
The "@b(Learn sequence)" option on the function menu will cause the
"@b(Learn)" context. The yellow key will now select nets and add them to
a internal buffer. It will refuse nets that are already completed or that
are already selected. The buffer will act like a stack: the last net that
was selected will be the first one to be presented for routing when the
"@b(Learn)" context is terminated (blue key). The buffer will also be used
to route a sequence. The green key causes all incomplete net to be
'connected' on the text overlay. This looks like a random collection of
straight lines: all point-to-point connections are drawn by straight line.
This overview display will disappear on the next screen movement (white key)
or when the "@b(Learn)" context is terminated.

As a general strategy: it is easier to route long connections first. It will
become harder to route long traces when there are a lot of them blocking
the path.
@subsection(Changing Colors)
The colors used on the AED may not suit everyones taste. The "@b(Change
color)" option on the function menu makes it easy to change them. The change
option will present a menu of layers (say via-holes, solder side, etc).
Once an item is selected, 3 bars for the red, green and blue intensity
will appear. Clicking the cursor on one of these bars will adjust the color
table. Clicking it elsewhere terminates the command.

The first item on the menu (@b(Flip sides)) is different: it exchanges the
priority of the solder and component side layers. This decides which one
is drawn on top. It also affects the select operations: if the cursor is
clicked on a spot that has both layers, the one 'on top' is used.
@subsection(Plowing a Wire)
Changing an completed board or adding the @i(last) wires usually requires
some existing traces to be moved around to open a path. This can be done by
deleting an existing wire, routing the current one and then re-routing the
old trace. The move wire operation is intended to simplify this operation.
Wires can be moved in the "@b(Start)" context by placing the cursor on the
offending wire and clicking the yellow/blue key @b(twice). A white
(text-colored) spot will appear at the cursor location and the context is
changed to "@b(Trace_mv)". Pressing the blue key will draw a vector from the
spot to the current cursor location. This is the length and direction of the
desired displacement. The green key will execute the operation. It is
possible to select an other wire by using the yellow key (or no wire at all
to escape from this command without change).
@subsection(Straighten a Wire)
The @i(straight wire) facility is actived exactly like the @i(move wire)
function except that a 0-length displacement is specified.
Pressing the green key with a 0-length displacement (say the blue key was not
used) activates the wire improver. It will try to shorten the wire and to
remove unnecessary bends. This function comes in 2 flavors: straighten by
move wire parts or by rerouting it. Moving is the default that can be
changed with the parameter menu. The @i(Move straight) function uses a local
operator to decide for each point of the wire wether it should be moved to a
new place or not. The operator slides back and forth until no improvement is
made. This has the effect of replacing the wire with a rubber band under
tension. The wire cannot tunnel trough any objects. The final result of this
function is not unique: normally there are a lot of ways that have equal
length which is a consequence of the slope being restricted to integer
multiples of 45 degrees.

The reroute option simply removes the wire and uses the router to insert it
again. The solution is unique because the router has a deterministic
strategy to find the shortest path. There are however 2 solutions depending
on which end the router is started from. The router preferes to start
diagonally. This can be used to control the route of the new wire because
the router will be started from the end that is next to the cursor/spot.
This features makes up for neat bundels if used properly. Unlike the move
option, the reroute option considers all possible paths: it can 'tunnel'
through pins.
@subsection(Batch operation)
Routing an entire board takes a significant amount of CPU time
(a 300 component / 400 net board needs about 40 min CPU time on a VAX 780
and about 4 Mbyte of memory). While it is fun to see the router at work,
it takes a good deal of cycles to update the AED, so this is reserved for
debugging sessions.

The "-b" option to the PCB command line will start the auto-router and
suppress all graphics output. No AED terminal is necessary in this mode.
PCB will report its progress after each pass (there are 5). Each pass takes
about 50% longer than the previous one.

The "-s" option will run the wire improver on all existing wires. It will
try each wire 3 times (@i(move, move, reroute)) in different orders because
these changes are not independent. The "-s" option uses about 30% less CPU
time than the auto-router, which is still substantial.
@subsection(Router Parameters)
The M and H router (for medium and high performance) are controlled by a set
of parameters which can be changed with parameter menu. Selecting the
"@b(Route parameters)" entry will cause a set of prompts to the terminal.

The first prompt selects the router for interactive use: 0 (=full) is the
high end version and use more CPU time. A '1' (=confined) will decide on a
rough route first and confines the search to this route. The next set of
prompts depends on the router selection. The confined router selection will
ask for:
@begin(description)
@i(Max wrong distance for single side runs):@\Normally the traces on the
component/solder side run 'horizontally'/'vertically'. This heuristic
usually helps to route a board. The @i(wrong) distance is the distance
that a 'horizontal'/'vertical' wire may run in y/x in raster units (1/80" for
the current set design rules that allow for 2 wires between IC pins).
This parameter controls the number of jogs of the rough route: if the actual
distance is less than this value, a single side run is attempted.

@i(Border size for single side traces):@\For a given 2 point connection,
consider the smallest rectangular box that contains both points (the small
side of this box must be less than the value of the previous parameter).
This parameter enlarges this box by a boarder of the specified amount. The
maze runner will consider only traces that are within this box.

@i(Border size for multiple side traces):@\This parameter is similar to the
previous one, except that it applies to each segment of a rough path.

@i(Min length for multiple via hole traces):@\ As a measure to reduce the
number of via-holes, a minimal distance between holes for the same trace is
enforced. In general, the router tend to be rather generous with via-holes.

@i(Min length for each segment):@\The rough path can consists of a number of
vertical and horizontal segment. This parameter enforces a minimal length
for each segments. Again, this is a measure to reduce the number of via-holes.

@i(Max number of via holes for one connection):@\This gives a upper limit on
the number of via holes that can be used for one point to point connection.

@i(Bonus for rectangular vias):@\The router distinguishes between 2 types of
via-holes: rectangular and diagonal ones. The holes are the same, but the way
the wires approach the hole is different: A @i(rectangular via-hole) is one
with wires that have a slope of an integer multiple of 90 degrees.
 @i(Diagonal via-holes) have at least one wire with a slope that is an odd
integer multiple of 45 degrees. It turnes out that the later type obstructs
more routing area and are less desirable for dense boards. However,
 @i(horizontal) ones use more wire length and may be less desirable when
enough routing space is available. The bonus is expressed in terms of
wire length.
@end(description)
The full maze runner uses a different stategy: it considers basically the
entire area of the rectangle defined by the 2 point of the desired
connection plus a certain boarder. It is controlled by the following set of
parameters:
@begin(description)
@i(Boarder size for routing area):@\This enlarges the area that confines the
search space (it would be rather slow to consider the entire board for every
net).

@i(Max path in wrong direction):@\Again, the enforces the
horizontal/vertical orientation of the 2 wire planes. Here this parameter
controlls the insertion of virtual fences into the maze.

@i(Max number of via holes in one segment):@\This limits the number of
via-holes for one connection.

@i(Penalty for HV via holes):@\Each @i(horizontal) via-hole will have the
cost of the specified trace length.

@i(Penalty for D via holes):@\Each @i(diagonal) via-hole will have the
cost of the specified trace length.

@i(Penalty progression for via holes):@\Each new via-hole will cost this
amount more than the previous one. This favors traces with fewer through
holes.

@i(Via hole alignment):@\Via holes must be aligned to drill grid of 1/40".
However, via holes that are aligned to a larger grid obscure less routing
area. A value of 2, 3, ... will cause the router to consider only via-holes that
fall on a 2/40", 3/40", ... grid.
@end(description)
It takes a bit of experimenting to tune the router for a particular
application. The default values have the flavor of educates guesses.
The auto router uses a schedule of parameters that is independent of
this menu.
@subsection(Diagnostic Functions)
Well, PCB is too large to become bug-free. Therefore, there are facilities
to deal with this problem. The diagnostic option of the function menu is one
of them. A couple of test programs are linked to the diagnostic menu that
will appear when @b(Diagnostics) was selected on the function menu. These
functions are rather harmless, so they were kept in the "@i(production)"
version :-).
@begin(description)
View:@\Prints the bitmap around the cursor on the terminal. Each hex-number
corresponds to one location in the bitmap. See the module 'pparm.h' for the
bit assignment. The router uses an auxiliary bit-map (ABM) that will appear
only if it is present for the cursor location. See 'pleer.h' to interpret the
ABM.

Verify:@\Scans all nets and compares the 'done' bit of the data base to the
actual layout. The layout overrides the 'done' bit in cases when they
disagree. It also updates the nets-to-go count which is maintained
incrementally for the sole purpose of en/dis-couraging the user (depends on
the value: 200 nets is 8 hours worth of work when done manually).

Clean:@\Removes all temporary mark and select bits from the bitmap. Leftover
select bits will cause the design rule checker to avoid places for no
obvious reason.

Flip:@\Was added for a project that confused solder with component side.
It will move all wires from one to the other side.
@end(description)
Except for @i(flip), no damage can occur to the data-base and @i(flip) is
reversible. Bugs should be rare enough so that these function should have
no real use (famous last words).
@section(Changing a layout)
PCB can update the database to correct earlier errors (for example the
netlist was wrong) or to add new circuits. In either case, one or more input
files to PCB were changed by means beyond the scope of PCB. Once the changes
are specified (by a new set of @i(type), @i(component) and @i(netlist)
files), PCB can perform them with the "@b(Update)" option on the parameter
menu.

The input files form a hierarchy: the @i(component) file depends on the
 @i(type) file and the @i(netlist) depends on the @i(component) file. This
dependency implies that changes on a lower level have consequences on the
net higher level. For example: if a 7400 was defined as a 16pin DIP and a
net uses a pin 15 of it then changing it to the correct 14 pin device will
cause a problem to the net that refered to the (no longer existing) pin 15.
So in order to correct the 7400-problem, all nets refering to it must be
removed before the change takes place. There are 2 ways to do this: gentle
and brutal.

The gentle way is the default as it can cause less trouble to the old board
if the new set of files is in error. A complete update is accomplished by a
5 step process:
@begin(enumerate)
Update the netlist with a version that has only nets that uses components
that will remain unchanged. Nets that differ or are not listed will be
removed from the PCB data base. If there was a wire layed out for a removed
net, it will be removed too.

Update the component file with a version that has only devices of types that
will remain unchanged. Components that are not listed will be erased.

Update the type file with the new version.

Update the component file with the new version: this may add any number of
new items.

Finally, update the netlist with the complete, new netlist.
@end(enumerate)
The first 2 steps can be skipped if only new circuitry is added. An other
way of simplifying the update is by using the @i(brutal) option: PCB will
delete all objects that depend on definitions that are being changed. For
example, if a type is replaced then all component that refere to it will be
deleted automatically. Likewise, if a component is changed, all nets that
refere to it will be removed. Obviously, it is necessary to follow the above
order of updates (step 3-5) for this approach.
@chapter(PCB Functions)
The next chapter is intended as a reference listing of all features of PCB.
The intended use of these features should be outlined from the previous
chapter. For each context the functions attached to the mouse keys are
listed. Colors given in capital letters indicate that the binding is in
effect if the cursor is not on the screen when the key is pressed.
@section("Start" context)
This is the initial context for manually routing a wire. PCB will enter this
context after start-up if there are unfinished nets and all components are
placed.
@begin(description)
@b(yellow):@\If clicked on a flashing pin,
it selects this pin for the start of
a new copper trace. If it is clicked on a wire that belongs to this net, it
selects that point to be the start of a new trace.
If the key is pressed on a pin or wire that does not belong to the
preselected net, PCB tries to select the net specified by that pin.
If there is no such net, or if the cursor is
pointing to empty space, a over-view map of the board, the current window
and the pins of the selected net is displayed in white. This map will
disappear on the next cursor entry. It is displayed automatically for nets
that do not fit on the screen.

@b(blue):@\Same function as yellow.

@b(green):@\If the cross hair points to an unselected wire trace, the wire
segment (a straight piece of the trace) is deleted. If it is clicked twice
over uninhabited area, the current net is deselected an a new one is chosen
without completing the current one (It will reappear later).

@b(white):@\It moves the center of the current window to the cross hair location.

@b(YELLOW):@\Increase the zoom magnification.

@b(BLUE):@\Decrease the zoom magnification.

@b(GREEN):@\The parameter menu is displayed. Options can be selected by moving
the cursor on the text an pressing any key (see the parameter menu section).

@b(WHITE):@\The function menu is displayed. Options can be selected by moving
the cursor on the text an pressing any key (see the function menu section).
@end(description)
Pressing the yellow or blue key @i(twice) while keeping the cursor at the
same location will switch to the "@b(Trace_mv)" context.
@section("Route" Context)
This context follows the start context once a start point on an unfinished
trace is selected.
@begin(description)
@b(yellow):@\It adds a wire trace on the component side from the last point to
a position close to the cursor (green line). Only wires that have a slope
that is an integer multiple of 45 degrees are supported. PCB selects the
best approximation to the desired slope. The wire will not cross unselected
wires on the same side or pins of other nets. If it crosses a wire of the
same net, it will stop at that point and form a connection (wires of the
same net that run on different sides on the board are not allowed to cross
without a connection (via hole). This makes the implementation of the
design rule checker a little easier). It may not be possible to allocate a
via hole at that point, in which case the wire will not be connected.
If the last wire segment runs on the other side, a via hole will be inserted.
The via hole may be shifted by one display unit to ensure alignment to the
drill grid. There may be no space to allocate a via-hole, causing a "beep"
on the terminal without any other effects. If the end-point is close to a
selected pin, an attempt is made to connect the wire to that pin if it is not
the origin of this wire. This call to the router depends on the parameter
selected for interactive routing (see parameter menu).

@b(blue):@\This function is similar to the previous one, except that the
solder side (red lines) is used.

@b(green):@\The delete key does not drop nets any longer. Instead it removes
the most recently entered wire segment if it doesn't finds unselected wires
to delete. Removing the very first wire segment causes the start point to be
deleted as well. In this case the "@b(Start)" context is restored.

@b(white):@\Moves the window center to the cursor position.

@b(YELLOW):@\Increase the zoom magnification.

@b(BLUE):@\Decrease the zoom magnification.

@b(GREEN):@\The parameter menu is displayed. Options can be selected by moving
the cursor on the text an pressing any key (see the parameter menu section).

@b(WHITE):@\The function menu is displayed. Options can be selected by moving
the cursor on the text an pressing any key (see the function menu section).
@end(description)
@section("Edit" context)
PCB will enter this context if all net are connected. The board is basically
ready at this point. This context is mainly intended for cosmetic changes or
producing output files.
@begin(description)
@b(yellow):@\No function assigned.

@b(blue):@\No function assigned.

@b(green):@\If the cross hair points to an unselected wire trace, the wire
segment (a straight piece of the trace) is deleted. If it is clicked twice
over uninhabited area, the current net is deselected an a new one is chosen
without completing the current one (It will reappear later).

@b(white):@\It moves the center of the current window to the cross hair location.

@b(YELLOW):@\Increase the zoom magnification.

@b(BLUE):@\Decrease the zoom magnification.

@b(GREEN):@\The parameter menu is displayed. Options can be selected by moving
the cursor on the text an pressing any key (see the parameter menu section).

@b(WHITE):@\The function menu is displayed. Options can be selected by moving
the cursor on the text an pressing any key (see the function menu section).
@end(description)
@section("Learn" Context)
This context is enter through the @i(learn sequence) option on the function
menu. It serves to select a sequence of nets for manual or automatic routing.
This is mainly a way to organize work.
@begin(description)
@b(yellow):@\is used to select a net if the cursor points to a pin or wire.
It will refuse to enter completed nets and it will reject nets that are
already selected.

@b(green):@\causes a overview of unconnected nets to be displayed. White
lines are drawn for each missing connection. The white key will get rid of
this display.

@b(blue):@\exits the learn session and restores the @i(Start) context.

@b(white):@\moves the display area (as in the other contexts).

@b(YELLOW):@\No function assigned.

@b(BLUE):@\No function assigned.

@b(GREEN):@\No function assigned.

@b(WHITE):@\No function assigned.
@end(description)
@section("AR-Del" Context)
This context is invoked by the @i(Area-Delete) option on the function menu.
A white rectangle is drawn. This confines the area that will be deleted. It
can be moved and reshaped by the cursor:
@begin(Description)
@b(yellow):@\moves the upper, right corner.

@b(blue):@\moves the lover, left corner.

@b(white):@\controls the display area.

@b(green):@\executes the delete.

@b(YELLOW):@\Increase the zoom magnification.

@b(BLUE):@\Decrease the zoom magnification.

@b(GREEN):@\A menu appears that allows to restrict the delete operation to a
particular side (or no side at all).

@b(WHITE):@\Same as GREEN.
@end(Description)
@section("A-route" Context)
This context is similar to the @i(AR-Del) context. However, the
green key will start the routing instead of delete operation.
The router are controlled by the parameter menu (see below).
@section("CP_move" Context)
This context allows to move components to different board
locations. All wires to a component must be disconnected before the component
can be moved. PCB will not allow a move off the board, on top of an other
component (pins may not overlap and must maintain a minimal separation).
This context is started though the function menu.
@begin(description)
@b(yellow):@\selects a component to be moved. The name and type of the
selected device will be displayed and the component outline will appear.

@b(blue):@\will move the component outline to a new position.

@b(green):@\will terminate the command by moving the component to the
position of the outline (provided that it is possible).

@b(white):@\controls the display area.

@b(YELLOW):@\Increase the zoom magnification.

@b(BLUE):@\Decrease the zoom magnification.

@b(GREEN):@\Rotate the component clockwise.

@b(WHITE):@\Rotate the component counter clockwise.
@end(description)
@section("Trace_mv" Context)
This context is entered by clicking the yellow or blue key twice in the
start context while the cursor does not move. A white (actually 'text
colored') dot will appear at the cursor location.
@begin(description)
@b(yellow):@\Selects a wire segment to be moved or straightened. The cursor
should point to a part of a trace. The white dot will move to the new
location.

@b(blue):@\Will draw a vector from the dot to the new cursor location.
This specifies the length and direction of the desired displacement. If the
length is 0 (or this key is not used at all), the wire will be straightened.

@b(green):@\Executes the operation if a wire is selected. If no wire is
selected, nothing will happen and the @i(start) context is restored.

@b(white):@\It moves the center of the current window to the cross hair location.

@b(YELLOW):@\Increase the zoom magnification.

@b(BLUE):@\Decrease the zoom magnification.

@b(GREEN):@\The parameter menu is displayed. Options can be selected by moving
the cursor on the text an pressing any key (see the parameter menu section).

@b(WHITE):@\No function assigned.
@end(description)
@section("CP_place" Context)
This context will appear if there are unplaced components. This may happen
after an update operation or at the start of a new board. PCB selects a
component to be placed and displayes its shape in the center of the screen.
@begin(description)
@b(yellow):@\Tries to place the component.

@b(blue):@\Positions the component symbol to a new location.

@b(green):@\Deletes the component at the cursor location if there is one.
A deleted component will reappear later for placement. If there is no
component to be deleted, PCB selects the next component for placement
(skipping the current one). This can be used to cycle through all unplaced
components.

@b(white):@\It moves the center of the current window to the cross hair location.

@b(YELLOW):@\Increase the zoom magnification.

@b(BLUE):@\Decrease the zoom magnification.

@b(GREEN):@\The component is rotated counter clockwise.

@b(WHITE):@\The function menu is displayed. Options can be selected by moving
the cursor on the text an pressing any key (see the function menu section).
@end(description)
@section(The Parameter Menu)
The parameter menu lists options that globally changes the way PCB works.
The first 4 items toggle between 2 states. Features to display information
about the board are also included on this menu.
@begin(description)
@i(Beep on/off):@\causes a bell-character (not) to be send to the terminal for
significant messages.

@i<(no) Adjust window>:@\When enabled, PCB tries to move the window
automatically to show the entire net.

@i(Enable/Disable H-route):@\When enabled, the medium or high power routers
are used to complete nets. When disabled, only the low power router is used.
(BTW: 'H' was intended to stand for home route - let me know of better
mnemonics)

@i(Learn off):@\The contents of the learn buffer is flushed.

@i(Route parameters):@\The router is controlled by a set of parameters
that can be changed for better performance in a particular situation.
The parameters will appear on the terminal. A <cr> uses the default.

@i(Print statistics):@\Board statistics (like size, the number of holes,
etc) are printed on the terminal.

@i(Wire density):@\A graph showing the projected wire densities is
displayed at the boarder of the screen. This is intended to indentifies
placement problems.

@i(Congestion area):@\The number of net that cross a particular part of the
board are displayed as a superimposed set of squares. The size of each
square is proportional to the number of net in that area.

@i(List):@\Will dump the current version of the @i(type), @i(component) or
@i(netlist) file. A second menu will be displayed to select which file should
be listed. PCB will prompt for a new file name if the default file names
already exist.

@i(Update):@\Will read in a new version of the @i(type), @i(component) or
@i(netlist) file. A second menu will be displayed to select which file should
be listed. PCB will prompt for a new file name if the default file names
are not present.
@end(description)
@section(The Function Menu)
The functions of the menu
can be selected by placing the cross-hair on the text and pressing any key.
The currently implemented functions are:
@begin(description)
@i(Plot Preview):@\dumps a copy of the bit-map on the Versatec plotter.
Actually, the bit-map is compiled into an other format that has a 2 times
higher resolution. This file is spooled to the Versatec and will be deleted
after plotting. The process takes about 2-5 Minutes for a large board (40 by
35 cm) which is considerably faster than using @i(Cifplot), which takes
several hours.

@i(CIF-Output):@\produces a CIF-output file. These files can be quite large
and it takes some time to generate and write them. If there are external
devices, their names are listed and a prompt for the corresponding cif file
is issued. This file is included as it is, that is no processing other than
copying takes place. The cif file should not have the @b(E)-record at the
end. The default cif-output file is 'pcb.cif'. PCB will not overwrite it
without a prompt.

@i(Exit):@\Terminates the edit session and saves intermediate results on
the file 'pcb.SAV'. This file is absolutely necessary for future changes to
the board.

@i(Quit):@\Terminates without saving. A verification guards against accidental
loss of work.

@i(Diagnostics):@\will produce an other menu with diagnostic functions for
debugging purposes (see the section on Diagnostics).

@i(Learn sequence):@\will enter the @i(Learn) context. This allows to store
a sequence of nets on a stack (last in - first out) to work on. Normally,
nets are selected by the editor by some build-in heuristic, but the user
may find a different sequence easier to route. This applies to the routing
functions too.

@i(Area-Delete):@\is used to erase large parts of the board. It creates the
@i(AR-Del)-context.

@i(Area-Route):@\causes the auto-router to work on a specific area of the
board.

@i(Route sequence):@\The auto-router is started on the nets selected by the
learn command.

@i(Change color):@\This allows to change the color tables of the display. The
most recent choice is stored in the save file and will be used for future
edit sessions.

@i(Move Component):@\This function is used to move components to other
locations. It starts the @i(CP-move) context.

@i(Expand Wire):@\Will increase the size of a selected wire segment.
@end(description)
@chapter (In Case of Trouble)
PCB was written to fill a local vacuum for PC board design tool and not
to build a commercial grade software product. A large program like PCB that
has a small user base will have quite a few bugs. PCB has a few facilities
to cope with bugs in a graceful way: internal consistency checks, restartable
checkpoint file and a redundant & robust data-structure help to preserve
work entered to PCB (in fact adding features, debugging and real use went on
in parallel with lots of program crashes but no major loss of works).

The internal consistency checks test assertion that should be true when the
program executes according to expectation. They are frequent enough to
confine problems early and guard against core dumps. When an assertion
fails, the data structure of PCB is dumped to 'pcb.ERR'. The old checkpoint
file (pcb.SAV) should be secured before 'pcb.ERR' is renamed to 'pcb.SAV'
and PCB is restarted. Almost all problems so far were related to dynamic
data structures that are reconstructed when PCB is started. The data-base
rarely changes (except for the net-done bits that can be reconstructed from
the bitmap: @i(Verify) of the diagnostic function set) and the bitmap will
show any trouble directly: say a wire is missing. Problems in the bitmap are
local and can be solved with the normal edit commands (say adding the lost
wire). If the board looks ok on the screen, it is ok: what you see is what
you get (this is the robust part).

The appearance of a strange color on the screen (called @i(Trouble)) indicates
that some function failed to remove all temporary select bits that
consequently confuse other operations. The @i(Clean) function of the
diagnostic menu erases these bits.

The @i(list) function of the parameter menu can be used to verify the
data-base: Use the @i(list) function at the start of a new board to
dump the @i(type, component) and @i(netlist) file. A @i(list) after a
suspected problem should produce the same files that can be compared to the
old ones with the unix @i(diff). Likewise, a @i(update) with the original
files should cause no changes.

It is a good idea to save the work before making major changes. Updating the
data base (say loading a new netlist) qualifies as major change. The
 @i(update) code is new and - unlike most of PCB - it does change the data
base.
@chapter (Implementation Hints)
The interesting parts of the
code uses creative, efficient C-programming which is to say that inner loops
are not meant to be comprehensible. Certain inner loops were improved by
looking at the generated assembly text (for example "@b(a & ~b)" is faster
on a VAX than "@b(a & b)"). Integers are assumed to be large enough to hold
a pointer. So in general, PCB is typical public domain quality code.

Most of PCB is straight forward application code that should run on other
systems without problem. However, the code that deals with the graphic
terminal (AED-512) and the plotter (Versatec V-80) are dependent on the
environment.

The graphic terminal code is in 'paed.c' and 'psub.c'. The routine
@i(init) prepares the line to the AED and the routine @i(finish) restores
the old state. The routine @i(select_AED) tries to locate the graphic
terminal. PCB assumes that the AED terminal line runs at 9600 baud and that
it can be switched to 19200 baud by using the external clock 'A' (EXTA) on the
VAX serial line interface. The line speed is important because PCB is I/O
bound during screen updates.

The plotter code is in 'pmap.c'. Plotting is done by writing the bitmap to
a certain location (here: /usr/vlsi/tmp) and creating an entry for the
plot-demon (here: /usr/spool/vpd). The entry has a pointer to the bitmap
and is created in a 2 step process: it is written to a temporary file
(tmp<process-id>.<plot-number>) that is renamed to (dfa<p-id>.p-#) once it
is complete. The demon is looking for files that start with 'dfa' and the
rename-operation should prevent the demon to work on incomplete entries.
Once entry and bitmap are in place, the demon (/usr/lib/vpd) is called.

The Caltech Intermediate Form (CIF) is described in @i(Introduction to VLSI
systems) by Mead and Conway on page 115-127. The layers for the MOSIS PC
board facilities are described in their documents (see below).
@section(Future Features)
...are not likely for a while. The current set of function is reasonably
useful as is. Tempting new features would be:
@begin(itemize)
More than 4 layer support

Automatic handling of multiple power supplies

Improving the router
@end
@chapter (Acknowledgements)
Many thanks to J. Kueffler (Robotics institute) and L. Butcher (Engineering
Lab) who had the questionable pleasure of being the first and second user.
They contributes a lot valuable suggestions concerning the capabilities and
the user interface of PCB.
@appendix(Sample Type file)
@begin(verbatim)
*types:
74F00:
0.000,0.000	pin1
1.000,0.000	pin2
2.000,0.000	pin3
3.000,0.000	pin4
4.000,0.000	pin5
5.000,0.000	pin6
6.000,0.000	GND
6.000,3.000	pin8
5.000,3.000	pin9
4.000,3.000	pin10
3.000,3.000	pin11
2.000,3.000	pin12
1.000,3.000	pin13
0.000,3.000	VCC

SWITCH:
EXTcif: 510
Box: 8.000,2.000
0.000,1.000	pin1
1.500,1.000	pin2
3.000,1.000	pin3
8.000,0.000	pin4
8.000,2.000	pin5
@end(verabtim)
@appendix(Sample Component file)
@begin(verbatim)
*components:
A2	CONN-16	5.000,10.000	1
B2	CONN-50	5.000,22.000	1
AA11	74F04	16.000,5.000	0
AA19	74LS245	49.000,11.000	0
AA30	74LS245	49.000,16.000	0
AA18	.1	23.500,5.000	1	; decouppling capacitor
AA10	.1	14.500,5.000	1
AA29	.1	35.500,5.000	1
AC16	82586				; unplaced
AD15	.1	21.500,13.000	1
AA40	.1	47.500,6.000	1
AC40	.1	47.500,11.000	1
AA41	74LS373	37.000,5.000	0
AC41	74LS373	25.000,5.000	0
AC04	.1	9.500,16.000	1
AC05	8023	11.000,16.000	0
AE05	J1	10.500,13.000	1
AE06	243	12.000,10.000	1
AE07	243	13.000,10.000	1
AE08	39.2	14.000,10.000	1
@end(verbatim)
@appendix(Sample Netlist file)
The first one uses the DP format:
@begin(verbatim)
DPnetlist:
!Gnd\ ASDFG-3 ASDFG-1 FA01-6 FA01-1 DG46-2 FH36-1 FH21-1 FH14-7
!Gnd\ FH13-1 FG37-14 FG22-14 FE36-1 FE21-1 FD37-14
!Vcc\ AC05-20 AC04-2 AA41-20 AA40-2 AA30-20 AA29-2 AA19-20 AA18-2
!Vcc\ AA11-14 AA10-2
%AA11-1\ AA11-1 AC05-6
%AA11-10\ AA11-10 BB22-11
%AA11-3\ AA11-3 AA11-2
%AA11-5\ AA11-5 AA11-4
WATCH DOG ENABL\ DG46-3 CH43-1 BF22-12
X1\ CC01-11 CF13-1 CF15-1
X2\ CC01-12 CF17-1 CF15-2
-12 V\ FA01-3 ASDFG-2
@end(Verbatim)

The next example uses the @i(Multiwire) format:
@begin(verbatim)
Cnetlist:
CGND	 ASDFG-3 ASDFG-1 FA01-6 FA01-1 DG46-2 FH36-1 FH21-1 FH14-7
CGND	 FH13-1 FG37-14 FG22-14 FE36-1 FE21-1 FD37-14
CVcc	 AC05-20 AC04-2 AA41-20 AA40-2 AA30-20 AA29-2 AA19-20 AA18-2
CVcc	 AA11-14 AA10-2
1	 AA11-1 AC05-6
2	 AA11-10 BB22-11
3	 AA11-3 AA11-2
4	 AA11-5 AA11-4
-1
@end(verbatim)
@appendix(MOSIS PC board information)
Information on the MOSIS pc-board service can be obtained by sending
an information request to @b(mosis@@usc-isif.Arpa). Information requests are
handled automatically and should follow the format described in the MOSIS
user manual. To retrieve the PC-board related information, the message body
should look like this:
@begin(verbatim)
	Request: Information
	Topic: PCBFAB
	Topic: PCBTEC
	Request: End
@end(verabtim)
