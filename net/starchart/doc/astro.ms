.ND

.ds St Astronomy\ Tools
.OH '\\*(St''%'
.EH '%''\\*(St'
.PP
.PP
.PP
.PP
.PP
.PP
.PP
.PP
.sp 10
.ce
.ps +10
.ce
Astronomy\ Tools
.ps -10
.sp 2
.ce
Craig\ Counterman
.PP
.bp
Copyright \(co 1990 Craig Counterman.  All rights reserved.
.bp
.PP
.PP
.PP
.PP
.PP
.PP
.bp
.NH
Introduction
.br
.PP
These programs are intended to assist observational astronomers.
.PP
Starchart produces astronomical charts from a variety of databases,
producing output for, in rough order of quality, PostScript, X11,
sunview, Atari St, IBM PC, X10, tektronix, unix plot, HP laserjet, and
tty displays, on Unix, vms and other systems.  It is straightforward to
write drivers for other output devices.  Observe is a multifunction
program for use in planning an observing session.  It calculates the
positions of planets and their satellites, asteroids and comets,
calculates rising and setting times, and much more.
.PP
.PP
This software may be redistributed freely, not sold.
.PP
No representation is made about the suitability of this
software for any purpose.  It is provided "as is" without express or
implied warranty, to the extent permitted by applicable law.
.PP
.sp 2
.SH
DISCLAIMER\ OF\ WARRANTY
.br
.PP
The author  disclaims all warranties  with regard to  this software to
the   extent  permitted  by applicable   law,  including all   implied
warranties  of merchantability  and  fitness. In  no event shall   the
author be liable for any special, indirect or consequential damages or
any  damages whatsoever resulting from  loss of use, data or  profits,
whether in an action of contract, negligence or other tortious action,
arising  out of  or in connection with the  use or performance of this
software.
.PP
.PP
.PP
.NH 2
What\ the\ Programs\ Do
.br
.PP
Several programs are included in this package, all intended to aid
observational astronomers.  The \fBobserve\fR program calculates positions
of moving objects and helps in planning an observing session.  There are
several starchart programs for preparing astronomical charts; there is a
separate program for each output device.  There are also several support
programs which operate on data files for these programs.
.PP
Starchart programs are quite general star map drawing programs and
have many potential uses, but their unique utility is in preparing
custom charts for particular observing projects, finding charts, and a
pages that can be put in observing notebook and annotated at will.
.PP
The \fBobserve\fR program provides many facilities of use in planning
an observing session.  It can generate ephemerides of planets, minor
planets, and comets, and puts the coordinates in files to be used with
starchart programs.  It calculates the approximate altitude and azimuth
of obects at sun rise and set and morning and evening twilights.  It can
calculate positions of the major satellites of Jupiter and Saturn.  Of
importance to observers is the generation of the timetable of events for
a night.
.PP
Other programs are provided to facilitate the use of these programs.
.PP
.NH 2
The\ Sky
.br
.PP
There is a sky.  There are things in the sky.  The starchart programs
draw maps of things in the sky.  The \fBobserve\fR program helps you plan
to look at things in the sky.
.PP
The things in the sky include stars, planets, nebulae, clusters of
stars, and galaxies. For thousands of years, people have grouped the
stars in the sky into patterns, and constellations.  More recently, the
sky has been divided into areas based on these constellations.  For
hundreds of years, astronomers have used a latitude-longitude grid for
defining the locations of celestial objects.  The longitude is usually
referred to as right ascension or RA, the latitude is the declination or
DEC.  Through the year, the sun follows a path in the sky, called the
Ecliptic, which is the plane of the earth's orbit.  Other planets in the
solar system are roughly in this plane.
.PP
The locations of stars, nebulae, clusters, and galaxies have been
tabulated for hundreds of years.  These programs use computerized forms
of such databases.
.PP
.NH 2
Coordinates
.br
.PP
You should be familiar with the method of specifying a location on the
surface of the earth: two coordinates are used: latitude north of the
equator and meridian of longitude east of Greenwich.  A similar system
is used to specify the locations of points in the sky.  the meridians
are called right ascension, and the latitude is called declination.
RA is measured from the point at which the sun crosses the equator in
March.  It is measured in hours, with 15 degrees = 1 hour.
.PP
There are two other coordinate systems commonly used in addition to
RA--dec.  These apply only to a particular observation location and
time.  The first of these is Hour angle -- declination.  It is similar
to R.A.--declination, but instead of R.A., the angle is measured west
from the line from north to south passing directly overhead.  The second
is altitude -- azimuth (alt--az): altitude in degrees above the horizon
and azimuth in degrees east from north.
.PP
.NH 2
Epoch\ and\ Equinox
.br
.PP
The coordinates of a ``fixed'' object are actually only approximately
constant.  Two factors change the coordinates in R.A. and declination.
.PP
.NH 3
Proper\ Motion\ and\ Epoch
.br
.PP
First, stars (and other galactic objects) are not fixed in space.  The
sun and stars all move.  Only the closer stars move significantly, but
closer stars are also brighter, so this is an important effect.  The
\&\fIepoch\fR is the time for which the coordinates are valid including
the effects of this \fIproper motion\fR. 
.PP
.NH 3
Precession\ and\ Equinox
.br
.PP
The second and more important factor is that the R.A.  and dec. 
coordinate system changes: the north pole and thus the equator change
due to \fIprecession\fR.  R.A.  changes as the equator moves and changes
the point at which the sun crosses the equator.  The \fIequinox\fR date,
or \fIequator and equinox\fR is the time at which the coordinate system
is valid.  Equinox 2000.0 and 1950.0 are commonly used coordinate
systems.  The equator and equinox of the date is also occasionally
important. 
.PP
The starchart programs use data in equinox 2000.0 coordinates.
.PP
.NH 2
Additional\ Corrections\ to\ Positions
.br
.PP
Precession and epoch were discussed above.  In addition there are
smaller effects which must be considered for precise astrometry.  These
include effects of motion about the earth-moon barycenter, light travel
time, nutation, and aberration of starlight.
.PP
.NH 2
Magnitudes
.br
.PP
Magnitudes are defined with respect to standards.
.PP
Objects emit photons.  The number of photons per second observed from an
object under given conditions and equipment is the intensity of light. 
A brighter object emits more photons.  The difference in magnitudes
between two objects is -2.5 times the log (base 10) of the ratio of
intensities, i.e.
m = m_std - 2.5 log(s/s_std)
.PP
Magnitudes are measured in different ways.  The most important are
visual, photographic, and photometric.  Visual and photographic
magnitudes are measured using those methods to estimate relative
magnitudes.  Photometry is the counting of photons received from
objects.  Standard filters are generally used.  The most common set is
UBVRI, Ultraviolet to Infrared.  The V filter approximates the response
of the eye, while the B filter is approximately the response of
photographic film. 
.PP
For a magnitude number to be truely meaningful, the system used must be
specified.  Generally V or visual may be assumed.
.PP
For many objects (V-B), that is the magnitude measured photometrically
with the V filter minus the B magnitude, indicates the color of the object.
.PP
.NH 2
Time\ and\ Date
.br
.PP
Time is a very complex subject.
.PP
.NH 3
Time
.br
.PP
Time systems include UT (= UT1), UT0, TA1, and UTC.  These times may
differ by a second or so.  They are based on the rotation of the earth. 
As the earth slows, and since the day is not \fIexactly\fR 86400 seconds
long, leap seconds are occasionally inserted.  They are all
approximately the time at 0 degrees longitude.  Most astronomical times
are quoted in UT (universal time).  The differences are rarely critical
for amateurs. 
.PP
Another significant time system TDT (formerly ET), and TBT.  TDT or
Terrestrial Dynamical Time is based on the orbits of the planets, as is
TBT or Terrestrial Barycentric Time (based on the center of motion of
the earth-moon system).  These times currently differ from UT by about a
minute.  TDT is the time which should be used for planetary calculations.
.PP
Time zones relate local time to the time at 0 longitude.  Be aware that
there are some fractional time zones in the world.  Daylight savings
time (or ``summer'' time) is an additional complication.  You should
learn how your time zone is related to the time at 0 longitude (UT or
GMT).  EST is 5 hours behind, EDT is 4 hours behind.
.PP
These times are all related to the position of the sun: the sun should
be overhead at about noon local standard time.  A different time is
\&\fIsidereal\fR time, based on the positions of the stars overhead.  Two
important sidereal times are GST or Greenwich Sidereal time, and LST or
local sidereal time.
.PP
.NH 3
Date
.br
.PP
Calendars are confused and confusing.  To avoid confusion between the
many calendars in use historically, JD \fIJulian date\fR is used.  The
JD 0 is a day more than 4000 years BC (BCE).
.PP
.NH 2
Atmospheric\ Effects
.br
.PP
Refraction affects alt-az  coordinates: light from objects are bent
by the atmosphere, making them appear higher in the sky than they would
if there were no atmosphere.  The error can reach 34 minutes of arc at the
horizon.  Extinction, absorption and scattering make objects fainter the
more atmosphere the light from them must pass through (that is, the
closer to the horizon they are).  Other effects of the atmosphere are
seeing (the effect which produces twinkling of stars), scintillation,
airglow, and of course light pollution.
.PP
.NH 2
Elements\ of\ an\ Observing\ Session
.br
.PP
To avoid frustration and ensure meeting goals you should plan your
observing session in advance.  More serious the goal the more carefully
you should plan.  At least, having a plan may help you avoid wondering
what to do next on a clear night. 
.PP
A timeline of events ensures that an object will be observable, and
helps schedule a night to observe all objects when they are well placed. 
The events are sun and moon rise and set, astronomical twilight, and the
rising, setting and transit times of objects.  In addition, objects
should be observed when possible when they are above 30 degrees above
the horizon, or at least above 20 degrees.  These times should also be
noted in the time line.
.PP
Charts are used in identifying planets, asteroids, and comets, and
finding objects.  For very faint objects, a photographic atlas of the
area should be xeroxed.
.PP
A notebook is an important part of observing.  It can be of scientific
notebook quality, or a simple note of what objects you observed and how
they looked.  Starchart programs may be used to produce finder charts
which you can annotate and keep in a notebook.
.PP
.NH 2
Hints
.br
.PP
Use grep, awk and shell scripts.  Unix tools can be very helpful in many
situations.
.PP
.PP
.PP
.bp
.NH
Observing
.br
.NH 2
The\ \fBobserve\fR\ Program
.br
.PP
This program is used to prepare for astronomical observations and for
ephemeris calculation.  It can calculate the position of the major
planets, the satellites of Jupiter and Saturn, and minor planets and
comets given either orbital elements or a tabulated ephemeris.
Coordinates of stationary objects may also be input.  It calculates
(approximate) rise and set times, and transit times, of all objects to
be observed.  It calculates the (approximate) altitude and azimuth of
objects at sun rise and set, and morning and evening twilight.
.PP
The information (coordinates, rise and set times) may be prepared for
either a single time or a sequence of times.
.PP
Output includes a file containing the timetable of events for the
evening or evenings.  The coordinates of the sun and moon, plus any
planets, minor planets, and comets are placed in a readable text file,
with other calculated values for solar system objects (e.g. distance to
earth).  The coordinates of all objects to be observed are placed in
files in formats to be read by the other programs in this family, namely
\&\fBdataconv\fR and the starchart charting programs.  A file listing the
(approximate) altitude and azimuth of objects at sun rise and set, and
morning and evening twilight is output.  If satellite positions are to
be computed, these coordinates are placed in one file, and a separate
PostScript file graphically showing their positions relative to the
primary is also produced.
.PP
These output files are optional: any subset of the possible output files
may be produced.
.PP
Parameters set the location of the observer, control what objects are to
be observed, and designate the time or times of interest.
.PP
Currently, there are some limitations to the program:
.IP
.IP \(bu\ 
Minor corrections such as parallax and nutation are not made.
.IP
.IP \(bu\ 
Positions are accurate enough for most applications other than
occultations.
.IP
.IP \(bu\ 
Rise and set times may be off by several minutes.
.IP
.IP \(bu\ 
Positions of Saturn's satellites are approximate, good enough for
identification of satellites.
.PP
.PP
.PP
.NH 2
Controls\ and\ Input\ for\ the\ \fBobserve\fR\ program
.br
.PP
Command line options set the location of the observer, control what
objects are to be observed, control the output files produced, and
designate the time or times of interest.  The default location of the
observer is set when the program is compiled.
.PP
.IP \f(CW\`-m\'\fR\ 
Meridian of longitude, measured East of Greenwich.  The USA is West of
Greenwich, and the longitude is negative for all USA locations.  One to
three numbers can be used: degrees, degrees and minutes, or degrees
minutes and seconds. 
.IP
.IP \f(CW\`-l\'\fR\ 
Latitude.  One to three numbers can be used: degrees, degrees and
minutes, or degrees minutes and seconds. 
.IP
.IP \f(CW\`-a\'\fR\ 
Altitude in meters. 
.IP
.IP \f(CW\`-z\'\fR\ 
Time zone in hours East of Greenwich, again, this number is negative
for USA locations.  This does not include any effects of daylight
savings. 
.IP
.IP \f(CW\`-d\'\fR\ 
The date or dates of observation in UT are specified with this flag.
The dates for the \f(CW\`-d\'\fR option are each specified as a string
consisting of month, day, and optional year (use if different from the
current year).  The month may be encoded as above, e.g. \f(CW\`Aug\'\fR for
august.  Using the first three letters of the English name for the month
always works, as does the Roman numeral form.  Some other common
abbreviations also work.  The year may also be specified, the default is
the current year.  The day may be fractional, e.g. 1.25 is 6 am UT on
the first.  An optional third parameter is the increment of time to be
used in stepping between the two dates. 
.IP
.IP \f(CW\`-o\'\fR\ 
This sets names used for the output files and controls which output
files are produced.  The output file names all have the form
\&\f(CW\`\fIoutfile_root\fR.XXX\'\fR, where \fIoutfile_root\fR is set at
compile time (usually to \f(CW\`planet\'\fR), or is set to the argument for
this option. The \f(CW\`-o\'\fR may be followed with a letter or letters from
the set "aeios" to select the altaz, eph, sif, obs, and star files,
respectively.  e.g. \f(CW\`-oae austin\'\fR would produce only the output files
"austin.altaz" and "austin.eph".
.IP
.IP \f(CW\`-p\'\fR\ 
The positions of any or all of the major planets at the time(s) may be
calculated.  This is specified either as \f(CW\`-p\'\fR which causes the
positions of all planets to be calculated, or individual planets may be
specified by following the \f(CW\`-p\'\fR with a letter or letters from the
sequence "MVmJsUN".  The positions of the sun and moon are always
calculated, since they always have some effect on observing conditions.
.IP
.IP \f(CW\`-s\'\fR\ 
The -s option causes the \f(CW\`.sat\'\fR and \f(CW\`.sat_PS\'\fR files to be
produced for the satellites of Jupiter and Saturn.  \f(CW\`-s\'\fR implies
\&\f(CW\`-p\'\fR.  With the \f(CW\`-si\'\fR option the drawings in the PS file are
flipped north to south (if for one time) or east to west (if for
multiple times) to produce an inverted view. 
.IP
.IP \f(CW\`-f\'\fR\ 
Input object data is contained in files in several formats.  The
\&\f(CW\`-f \fIfilename\fR \fIformat\fR\'\fR option sets this input file.
.IP
.IP \f(CW\`-n\'\fR\ 
For some input file formats, the name of the object must be specified
using the \f(CW\`-n\'\fR option. 
.IP
.PP
Sample data files should be used for the file formats used for input
files.  The format names are \f(CW\`obj\'\fR for fixed objects, \f(CW\`ell_e\'\fR
and \f(CW\`par_e\'\fR for parabolic orbital elements, and \f(CW\`emp\'\fR,
\&\f(CW\`empb\'\fR, \f(CW\`aa\'\fR, \f(CW\`st\'\fR, and \f(CW\`iau\'\fR for tabulated
ephemerides.  The fixed object format contains the coordinates, name,
magnitude, type and size of objects to be observed.  The other file
formats are used for comets and minor planets, and are discussed below.
.PP
.PP
The output files are named \f(CW\`\fIoutfile_root\fR.XXX\'\fR where XXX is:
.IP \f(CW\`altaz\'\fR\ 
altitude and azimuth of objects at sun rise and set, and morning and
evening twilight. 
.IP
.IP \f(CW\`eph\'\fR\ 
ephemeris of sun, moon and objects specified. 
.IP
.IP \f(CW\`obs\'\fR\ 
observability of objects: rise and set times of objects, twilight times,
etc. 
.IP
.IP \f(CW\`star\'\fR\ 
\&\f(CW\`lineread\'\fR format file containing coordinates (equinox 2000) of the
object(s), sun, moon. 
.IP
.IP \f(CW\`sif\'\fR\ 
\&\f(CW\`sif\'\fR format file containing the same information as the .star file.
The separation character is \f(CW\`;\'\fR. 
.IP
.IP \f(CW\`sat\'\fR\ 
Locations of the major satellites of Jupiter and Saturn with respect to
the primary. 
.IP
.IP \f(CW\`sat_PS\'\fR\ 
PostScript file drawing either: one page showing appearance of Jupiter
and Saturn with satellites, and relative sizes and orientations of
Mercury, Venus, Mars, Jupiter, and Saturn, and the Moon; or several
pages showing Jupiter and Saturn with moons on a sequence of times if a
range of dates was specified. 
.IP
.PP
.NH 2
Planetary\ Positions
.br
.PP
Planetary positions are calculated with moderate precision.  Formulae
are largely from \fIAstronomical Formulae for Calculators\fR by Jean
Meesus.  Minor corrections are currently ignored.
.PP
.NH 2
Comets\ and\ Minor\ Planets
.br
.PP
Comets and minor planet positions may be calculated either directly from
the orbital elements or by interpolating a tabulated ephemeris.  The
calculation from orbital elements is most convenient, but the tabulated
ephemeris is likely to be slightly more accurate.
.PP
.PP
.NH 3
Orbits
.br
.PP
The coordinates of objects in elliptical or parabolic orbits may be
calculated from orbital elements given in files of format \f(CW\`ell_e\'\fR
and \f(CW\`par_e\'\fR respectively.
.PP
Orbital elements are:
.IP \f(CW\`a\'\fR\ 
semimajor axis, A.U.
.IP
.IP \f(CW\`q\'\fR\ 
perihelion distance, A.U.
.IP
.IP \f(CW\`e\'\fR\ 
eccentricity
.IP
.IP \f(CW\`i\'\fR\ 
inclination (degrees)
.IP
.IP \f(CW\`omega\'\fR\ 
argument of perihelion
.IP
.IP \f(CW\`Omega\'\fR\ 
longitude of ascending node
.IP
.IP \f(CW\`n\'\fR\ 
mean motion (degrees/day)
.IP
.IP \f(CW\`M\'\fR\ 
Mean anomaly at epoch
.IP
.IP \f(CW\`T\'\fR\ 
Time of perihelion.
.IP
.PP
For elliptical orbits, \f(CW\`q\'\fR and \f(CW\`T\'\fR are not needed; for
parabolic orbits, only \f(CW\`q\'\fR, \f(CW\`i\'\fR, \f(CW\`omega\'\fR and \f(CW\`Omega\'\fR
are needed. 
.PP
.PP
Orbital elements are referred to a certain equinox, and apply on a
certain epoch date.
.PP
If \f(CW\`a\'\fR is not given, it may be calculated from \f(CW\`a\'\fR =
\&\f(CW\`q\'\fR/(1-\f(CW\`e\'\fR). 
.PP
If \f(CW\`n\'\fR is not given, it may be calculated from \f(CW\`n\'\fR =
0.985609/(\f(CW\`a\'\fR*sqrt(\f(CW\`a\'\fR)). 
.PP
If \f(CW\`M\'\fR is not given, use \f(CW\`M\'\fR = (\f(CW\`Epoch_date\'\fR - \f(CW\`T\'\fR)
* \f(CW\`n\'\fR. 
.PP
The magnitude of an asteroid may be calculated from two parameters:
\&\f(CW\`G\'\fR and \f(CW\`H\'\fR.  The magnitude of a comet may be calculated from
the parameters \f(CW\`g\'\fR and \f(CW\`kappa\'\fR. 
.PP
.NH 3
Interpolation\ of\ Ephemerides
.br
.PP
All ephemeris formats have date, RA, DEC, then other info.  Date is the
month in characters, then the day of the month.  How the month is
encoded is format dependent, examples include \f(CW\`IX\'\fR \f(CW\`Sept.\'\fR
\&\f(CW\`Sep\'\fR \f(CW\`Sep.\'\fR \f(CW\`September\'\fR.  Year is current year unless
specified in command line, and is the year of the first date.  Dates
must be in increasing order: 3 followed by 4, December followed by
January.
.PP
The formats are:
.IP \f(CW\`emp\'\fR\ 
Format used in the Russian \fIEphemerides of minor planets\fR.
.IP
.IP \f(CW\`empb\'\fR\ 
Format used in the Russian \fIEphemerides of minor planets\fR for
bright and unusual asteroids.
.IP
.IP \f(CW\`aa\'\fR\ 
Format used by the \fIAstronomical Almanac\fR.
.IP
.IP \f(CW\`st\'\fR\ 
Format commonly used by \fISky and Telescope\fR magazine.
.IP
.IP \f(CW\`iau\'\fR\ 
Format commonly used by IAU Circulars.
.PP
.NH 2
Time\ Line\ of\ Events
.br
.PP
The \f(CW\`.obs\'\fR file contains a timeline of events for the night or nights
of observation.  Events include sun and moon rise and set, morning and
evening twilight, and for each object to be observed: the rise and set
times,  the times when they are 20 degrees and 30 degrees above the
horizon, and the time of their transit (when they cross the line from
north to south passing directly overhead, and are at their maximum
altitude above the horizon).
.PP
The times, especially sun and moon rise and set are approximate.
.PP
The timeline should help you plan your evening, so you know when to
begin and end (twilight), and when the moon will interfere.  It helps
you plan to observe the objects when they are well placed, and ensure
that you can observe an object before it has fallen too low in the sky.
.PP
.NH 2
Satellites\ of\ Jupiter\ and\ Saturn
.br
.PP
The positions of the major satellites of Jupiter and Saturn are
calculated for the time or times of interest.  The Saturn satellite
positions in particular are approximate, but are certainly good enough
for identification purposes.
.PP
The positions are output in the \f(CW\`.sat\'\fR file, and also illustrated in
PostScript in the \f(CW\`.sat_PS\'\fR file.
.PP
If more than one time is specified, the PostScript program draws many
Jupiters and Saturns with their moons on a page (with separate pages for
Jupiter and Saturn).  If one time is specified, a single page is
produced containing large drawings of Jupiter and Saturn with their
moons.  As a bonus, this single page also illustrates the phases and
orientations of Mercury, Venus, Mars, Jupiter and Saturn (with rings),
and the moon.
.PP
.bp
.NH
Charts
.br
.NH 2
The\ Starchart\ Programs\ 
.br
.PP
The starchart programs are variations of a single program, producing
output for different output devices.  All the programs produce
astronomical star charts from available databases.
.PP
.PP
.PP
.NH 2
Purpose\ of\ Starchart
.br
.PP
The starchart programs have many potential uses: in education, preparing
illustrations, becoming familiar with the constellations and the sky;
but it is primarily useful for preparing for observing.  It can prepare
large scale maps, finder charts, variable star charts, and illustrations
for inclusion in an observing notebook.
.PP
A variety of map projections are available for various needs.
.PP
.NH 2
Features
.br
.PP
There are many many features of potential use.  The programs can:
.IP
.IP \(bu\ 
Allow for the creation of star charts of medium quality, comparable to
published works in data content.
.IP
.IP \(bu\ 
Allow for the creation of custom star charts for use as finder maps for
specific objects, including the labeling of stars with their magnitudes.
Inverted maps may also be produced.
.IP
.IP \(bu\ 
Allow for the creation of figures of the sky overhead at any location
and time, as in the Sky and Telescope centerfold, or planispheres.
.IP
.IP \(bu\ 
Optionally include in the above: planets (including the sun, moon,
asteroids, comets), fixed stars, nebulae, clusters, etc., lines for
constellation shapes, constellation boundaries, paths of planets, the
ecliptic, galactic equator, milky way isophots, etc.
.IP
.IP \(bu\ 
Use color for objects, and stars by spectral type, when available for
the given device.
.IP
.IP \(bu\ 
Label stars with their name, number or letter, and/or magnitude.
.IP
.IP \(bu\ 
Label other objects by name.
.IP
.IP \(bu\ 
Allow custom star databases to be used to produce maps of the sky in,
for example, 10,000 B.C.
.IP
.IP \(bu\ 
Interact on the display to allow for interactive sky exploration, if the
device supports such interaction.
.PP
Starchart is intended to produce useful displays on any available
device, and to produce best results on high quality graphics printers,
especially those using PostScript.  Output may be captured in files to
be edited for especially customized maps.  Comments are included in the
output whenever possible to facilitate this editing.  Given a good
object-oriented editor, professional quality maps may be easily
produced.
.PP
The program has a set of core routines for user interface and data
input, while a set of specialized driver routines is provided for each
output device.
.PP
.NH 2
Non-\ and\ Mis-\ Features
.br
.PP
There are some limitations remaining.
.PP
The output is limited by the capabilities of the device.  However, even
the best of the drivers has the following problems: it cannot detect and
avoid overlapping labels and stars, and it cannot allow the user to
interact with the display and move the labels.  These limitations are
what distinguish the output from these programs from a professionally
prepared map.
.PP
The current programs do not have user configurable Legend areas.
.PP
.NH 2
Objects\ Drawn
.br
.PP
The objects drawn currently include:
.PP
.IP \f(CW\`Stars\'\fR\ 
Size represents the magnitude.  For most devices, this is done by
rounding to the nearest integer magnitude, and drawing the star based on
this; however, some versions, e.g. \fBstarpost\fR for PostScript, allow
for continuous scaling of the size.  Color is determined from the star's
spectral class. 
.IP
.IP \f(CW\`Planets\'\fR\ 
On some devices, separate symbols are drawn for each planet, on others
they are all represented by the same symbol.  Major planets should be
distinguished from comets and asteroids.  Color may be assigned in the
database file. 
.IP
.IP \f(CW\`Nebulae\'\fR\ 
Diffuse, planetary and unspecified nebulae may be
distinguished.  If the device can support it, size may represent the
diameter of the nebulae (if provided in the database).  Color may be
assigned in the database file. 
.IP
.IP \f(CW\`Galaxies\'\fR\ 
Spiral, elliptical and unspecified galaxies may be
distinguished.  If the device can support it, size may represent the
diameter of the galaxy (if provided in the database).  Color may be
assigned in the database file. 
.IP
.IP \f(CW\`Clusters\'\fR\ 
Open, globular and unspecified clusters may be
distinguished.  If the device can support it, size may represent the
diameter of the cluster (if provided in the database).  Color may be
assigned in the database file. 
.IP
.IP \f(CW\`Other\'\fR\ 
Other identified objects.  If the device can support it,
size may represent the diameter of the object (if provided in the
database).  Color may be assigned in the database file. 
.IP
.IP \f(CW\`Unknown\'\fR\ 
For objects which haven't been identified.  If the
device can support it, size may represent the diameter of the object
(if provided in the database).  Color may be assigned in the database
file. 
.IP
.IP \f(CW\`Vectors\'\fR\ 
Solid, dashed, or dotted lines may be drawn using commands in a
database.  Lines are drawn as projected segments of great circles, or by
linear interpolation in latitude and longitude between endpoints.  Not
all line styles may be supported on some devices.  Color may be assigned
in the database file. 
.IP
.IP \f(CW\`Text\'\fR\ 
Invisible objects in a database are used to place text
labels in the chart. 
.IP
.IP \f(CW\`Comments\'\fR\ 
Lines for comments are allowed in database files.  They have no effect
on displayed output. 
.IP
.IP \f(CW\`Filled\ areas\'\fR\ 
Implemented as filled areas when possible for the device, otherwise as
outlines.  Area boundaries may be segments of great circles or defined
by linear interpolation in latitude and longitude between endpoints.
.PP
.NH 2
Projection\ Modes
.br
.PP
There are currently five projection modes available for a map.  The
default is Sanson's sinusoidal projection, which is area-preserving, but
fails near the poles.  This projection preserves both area and linearity
in Declination (y axis).  It gives good conformality (angle correctness)
near the equator, so it is useful along the Ecliptic.  Lines of RA
converge at the poles (unlike cylindrical projections), though
Cassiopeia and the Dipper reproduce well.
.PP
Stereographic projection keeps circles circles, and can be used near the
poles.
.PP
Orthographic projection is simple but very distorting; it is as if the
sphere of the sky were finite in size and viewed from infinity.
.PP
Gnomonic projection projects great circles as straight lines, and is
therefore very useful for charts to be used to plot meteor paths.  See
for example the October 1988 \fISky and Telescope\fR.
.PP
Rectangular projection is the simplest: latitude and longitude are
translated directly to x and y of the map.  Note that this is neither
cylindrical nor Mercator projection.
.PP
Only the main map projection mode may be changed through the command
line and \f(CW\`.starrc\'\fR format control files; \f(CW\`mapwin\'\fR control may change
the projection mode of the thumbnail map.
.PP
.NH 2
The\ Chart
.br
.PP
A \fIchart\fR is a single page or display region.  Within that region are
one or more \fImap windows\fR, each of which may have an associated
\&\fIlegend\fR region.  Each map window is independent of others.  Its
attributes are defined in a structure which contains its location and
size, and parameters and flags used in determining what is placed in the
window.  The detailed appearance of each graphic element to be displayed
in the window is up to the driver.  The starchart main routines only
define the location of the element, and report which routine is calling
the display function.  For each line in every star, planet, vector, or
other file, the main routines determine only \fIwhat\fR is to be
drawn, and \fIwhere\fR on the entire device page it appears.  The
device specific drivers determine exactly how it appears on the
device. 
.PP
The page may have a title region, which is drawn once per page.  This
region is entirely left to the driver.
.PP
The initial values of map window structures are set in the device
driver.  User input from the \f(CW\`.starrc\'\fR file and the command line may
change these values, and then the driver has an opportunity to override
all settings, possibly through additional user input.  The structure may
be read and written to \f(CW\`mapwin\'\fR files, which may be used for input to
different drivers and (carefully) editted by the user.
.PP
.NH 3
Layers
.br
.PP
A map window consists of several \fIlayers\fR.  All layers are optional.
The order of layers is determined independently for each map window.
.PP
A map window has an \f(CW\`outline\'\fR, which is determined by the projection
mode.  For the Sanson's projection, RA and DEC tick axes are drawn.
.PP
A grid of arbitrary origin and spacing in RA and DEC may be drawn.
The RA and DEC lines are in two separate layers, called \f(CW\`ra_grid\'\fR
and \f(CW\`dec_grid\'\fR.
.PP
The \f(CW\`ecliptic\'\fR is a separate layer.
.PP
A file defining constellation \f(CW\`boundaries\'\fR is provided, in
\&\f(CW\`lineread\'\fR format.  The file contains vectors which are drawn in a
separate layer.  Similarly, a \f(CW\`patterns\'\fR file contains figures
for the constellations.  A third file and layer provides names for the
constellations (\f(CW\`constlnames\'\fR).
.PP
Finally the set of files defined by the user is plotted.  These
currently include by default and in order a star file \f(CW\`yale.star\'\fR,
a file which is an index to files separated by region of sky called
\&\f(CW\`index.indx\'\fR, a nebula database \f(CW\`neb.star\'\fR, and a planet
location file \f(CW\`planet.star\'\fR.  Additional files may be furnished by
the user.
.PP
The default is to treat these as one layer, \f(CW\`allfiles\'\fR, and draw
the objects in each file in order.  Alternatively, for each map window,
the order may be changed to draw all the symbols in all the files
followed by all the names in all the files, etc.  The supported elements
which are controlled by the data files are: glyph (symbol of object),
name (text to the right of object), magnitude label (text below and to
the right of object), vectors, and filled areas.  These are layers
\&\f(CW\`allfiles\'\fR, \f(CW\`allglyphs\'\fR, \f(CW\`allnames\'\fR, \f(CW\`allmaglbls\'\fR,
\&\f(CW\`allvectors\'\fR, and \f(CW\`allareas\'\fR.
.PP
Each file has its own cutoff magnitudes for visibility of objects symbol
(\fImaglim\fR) name label (\fIlbllim\fR) and Bayer/Flamsteed label
(\fIgklim\fR).
.PP
Finally, a \f(CW\`legends\'\fR layer displays the legends area, if any, for a
window.
.PP
To stress: any layer in any window may be skipped, and they may be drawn
in any order, independently.
.PP
.NH 2
User\ Controls
.br
.PP
As many chart controls as feasible may be set by various methods of
user interaction.
.PP
.PP
.NH 3
Command\ Line\ Controls
.br
.PP
Currently defined command line arguments are described below.
Additional flag interpretation may be done by the device driver:
unidentified flags are passed to a driver-specific function which
attempts to interpret the flag. All others are undefined, and produce a
usage message and error exit.
.PP
.nf
.br
   
Flag	Additional argument type(s)   Mnemonic
-r	float			   Right ascension
-d	float			   Declination
-s	float			   Scale
-m	float			   Magnitude limit
-l	float			   name Label magnitude limit
-g	float			   Greek label magnitude limit
-p	[string]		   Polar,
				   or Projection = string
				   Possible values ``sansons'',
				   ``stereographic'',
				   ``orthographic'',
				   ``gnomonic'', ``rectangular''.
-b	--none--		   Big map: full page map
-i	--none--		   Invert: flip map top to bottom
-t	string			   Title
-y	string			   Yale.star: star data file
-h	string			   Harvard index.indx index file
-n	string			   Neb.star: nebula data file
-w	string			   Wanderers: planet.star planet data file
-f	string			   File: user file
-c	string [string]		   Constellation: a three or four character
				   mnemonic followed by an optional file name;
				   the file is searched for an entry
				   corresponding to that mnemonic: the entry
				   specifies the RA DEC Scale and Title.
-v	float float		   Variable finder: labels stars with their
				   magnitudes if they are within
				   arg2 mags. of arg1
-x	float float [float float]  X grid: grid in RA and DEC,
				   spacing arg1 and arg2, origin at arg3 arg4
-e	string			   Extra: chart control file.
-u	--none--		   User input.  Allow keyboard-based user input
-a	string			   Additional Argument string:
				   passed to driver for additional control.
.fi
.PP
.NH 3
Control\ and\ Specification\ Files
.br
.PP
The controlling variables of the chart are set by default at compile
time, and many may be overridden by command line arguments.  However,
more control is afforded by \f(CW\`.starrc\'\fR and \f(CW\`-e \fIextra\fR\'\fR
files.  The starchart programs attempt to read an initialization file,
whose name is set at compile time (default \f(CW\`./.starrc\'\fR).
Additional such files may be read through the \f(CW\`-e\'\fR commandline
argument.  The file contains lines of the form \f(CW\`\fIvariable\fR
\&\fIvalue\fR\'\fR or \f(CW\`\fIvariable\fR=\fIvalue\fR\'\fR, with comments
beginning with \f(CW\`#\'\fR ignored.  The variables and their values are
given below.  These definitions reset the values of program variables,
overriding their current value.  These may in turn be overridden by
later command line arguments or additional \f(CW\`.starrc\'\fR files
specified with \f(CW\`-e\'\fR.  Any variables not understood by the main
program are passed to a driver specific function which may interpret it.
.PP
.nf
.br
   
Variable	    Value type(s)	 comment
ra		    float	  # same as -r
dec		    float	  # same as -d
scale		    float	  # same as -s
title		    string	  # same as -t
findconst	    string	  # same as first argument of -c
constfile	    string	  # same as second argument of -c
namelim		    float	  # same as -l
lbllim		    float	  # same as -l
maglim		    float	  # same as -m
gklim		    float	  # same as -g
starfile	    string	  # same as -y
indexfile	    string	  # same as -h
nebfile		    string	  # same as -n
planetfile	    string	  # same as -w
userfile	    string	  # same as -f
mapfile		    string string # set mapfile array in main routines:
				  #       sets elements in order
				  #       overrides previous settings
				  #    overrides default yale.star, etc.
				  #    second arg is type of file
				  #    as specified in index.indx
boundfile	    string	  # sets file for "boundaries" layer
patternfile	    string	  # sets file for "patterns" layer
constlnamefile	    string	  # sets file for "constlnames" layer
layers		    string	  # same as all_layer:
all_layer	    string	  # sets array of layers in main routines:
				  #     sets elements in order
				  #     overrides previous settings
				  # possible values :
				  # "outline" "ra_grid" "dec_grid" "ecliptic"
				  # "boundaries" "patterns" "constlnames"
				  # "allfiles" "allglyphs" "allnames"
				  # "allmaglbls" "allvectors" "allareas"
				  # "legends"
polarmode	    logical	  # same as -p if logical is nonzero or is TRUE
bigflag		    logical	  # same as -b if logical is nonzero or is TRUE
projection_mode	    string	  # set projection mode
				  # may be "sansons", "stereographic"
				  # "orthographic", "gnomonic" or "rectangular"
invert		    logical	  # same as -i if logical is nonzero or is TRUE
precess_to          float	  # Precess to equinox and ecliptic of year.
chart_type	    string	  # set chart type: "fullpage" or "threepart"
				  # "threepart" is default: Large window,
				  # thumbnail, and legend
				  # "fullpage'' is same as -b flag
vrange		    float float	  # same as -v
vmags		    float float	  # set magnitude labeling maximum
				  # and minimum magnitudes
grid		    float float [float float]
				  # same as -x
nogrid		    logical	  # turn off display of all grids
nogrid_ra	    logical	  # turn off display of RA grid
nogrid_dec	    logical	  # turn off display of DEC grid
nomaglbls	    logical	  # turn off display of magnitude labels
drivercontrol	    string	  # same as -a flag
driver_control	    string	  # same as -a flag
additional	    string	  # same as -a flag

For interactive control, the following commands are also active:

exit		    --none--	  # Exit the program
quit		    --none--	  # Exit the program
end_input	    --none--	  # End input, display chart
done_input	    --none--	  # End input, display chart
done		    --none--	  # End input, display chart
draw		    --none--	  # End input, display chart
help		    --none--	  # Give help
?		    --none--	  # Give help
show		    --none--	  # Show value of variables
write_rc_file	    string	  # Write current state to .starrc file
write_mapwin_file   string	  # Write detailed specification of
				  # chart to given filename
read_mapwin_file    string	  # Read detailed specification of
				  # chart from given filename
.fi
.PP
.PP
.PP
.PP
.PP
.NH 3
Interactive\ Controls
.br
.PP
The \f(CW\`-u\'\fR option activates interactive mode, where the same commands
as in the \f(CW\`.starrc\'\fR file may be typed to control the program
options.  In addition, \f(CW\`end_input\'\fR ends interactive input and draws
the chart.  After the chart is drawn, control returns to the keyboard.
The \f(CW\`quit\'\fR command exits the program.  The command \f(CW\`show\'\fR
shows the current state of the control variables.  The command
\&\f(CW\`help\'\fR may be used to get a brief description of the commands and
controls available.
.PP
Some, especially window based, versions will allow the user final
control over each map of the chart.  The chart is specified by an
array of large \fBC\fR structures, one for each map on the page.  The
\&\f(CW\`mapwin\'\fR file contains the values in this structure.  Advanced
interactive versions of starchart, such as \fBstarXaw\fR, allow the
components of the structures to be edited individually, providing full
access to the flexible controls of the program.
.PP
The sequence of control of the program is:
.PP
1) read the \f(CW\`./.starrc\'\fR file.
.PP
2) process the command line: options are processed in order, later
arguments may override earlier arguments, additional \f(CW\`.starrc\'\fR
format files are read as they are encountered in \f(CW\`-e\'\fR options.
However, if the file specified in the \f(CW\`-e\'\fR option is in \f(CW\`mapwin\'\fR
format, it is read later.  After the command line is processed, if the
\&\f(CW\`-u\'\fR option is active, keyboard interaction is performed.  Next,
any \f(CW\`mapwin\'\fR format files are read.  If there is more interaction
available (e.g. as in \fBstarXaw\fR), the details of the chart may now be
controlled.  If a \f(CW\`mapwin\'\fR file is to be written, it is written now.
Then the chart is drawn to specification.  Finally, after the chart is
drawn, the program returns to the first interaction (i.e. returns to
the point after the command line has been processed).
.PP
One more form of interaction is available on some systems:  A mouse or
other pointing device may be used to either select points on a map and
the coordinates of that point are displayed, or the databases may be
searched for objects near the selected point and the database entries
for the objects found are displayed.
.PP
.NH 2
Standard\ Data\ Files
.br
.PP
All are in equinox 2000.0 coordinates, and should also be in epoch
2000.0.
.PP
.IP \f(CW\`yale.star\'\fR\ 
stellar information (mandatory)
.IP
.IP \f(CW\`neb.star\'\fR\ 
nebulae (optional)
.IP
.IP \f(CW\`planet.star\'\fR\ 
planets (optional)
.IP
.IP \f(CW\`boundaries.star\'\fR\ 
constellation boundaries (optional)
.IP
.IP \f(CW\`cnames.star\'\fR\ 
constellation names (optional)
.IP
.IP \f(CW\`patterns.star\'\fR\ 
constellation patterns (optional)
.IP
.IP \f(CW\`index.indx\'\fR\ 
index file for additional star files (optional)
.IP
.IP \f(CW\`con.locs\'\fR\ 
default mnemonic locations
.IP
.PP
.NH 2
Particular\ Starchart\ Programs
.br
.PP
Some starchart programs have coadditional driver-specific controls
which are accessed throught the \f(CW\`-a\'\fR command line option, or the
\&\f(CW\`additional\'\fR (= \f(CW\`driver_control\'\fR) variable.
.PP
These programs and other versions of starchart with special needs are
documented in this section.
.PP
.PP
.NH 3
\&\fBstarX11\fR
.br
.PP
\&\fBstarX11\fR is the X11 version of starchart.  It has the following
special arguments which are accessed through \f(CW\`-a\'\fR, e.g.
.br
\&\f(CW\`-a p\'\fR.
.PP
.IP \f(CW\`p\'\fR\ 
PostScript preview: use screen size and shape to approximate coverage of
\&\fBstarpost\fR chart. 
.IP \f(CW\`l\'\fR\ 
PostScript landscape preview: use screen size and shape to approximate
coverage of \fBstarpost\fR landscape format chart. 
.IP \f(CW\`h\'\fR\ 
hide: display is generated invisibly, which may save time. 
.IP \f(CW\`b\'\fR\ 
backup: use backup pixmap, allows window to be hidden and redrawn. 
.IP \f(CW\`m\'\fR\ 
monochrome: prevents use of color on displays otherwise capable of it. 
.PP
Usage example:
.br
\&\f(CW\`starX11 -c ori -g 5 -a p\'\fR.
.PP
.NH 3
\&\fBstarXaw\fR
.br
.PP
\&\fBstarXaw\fR is the X11 version of starchart with interactive control
using Athena Widgets.  In addition to the controls for \fBstarX11\fR,
it has the following special arguments which are accessed through
\&\f(CW\`-a\'\fR, e.g. 
.br
\&\f(CW\`-a "t -geometry -0-0"\'\fR.
.PP
.IP \f(CW\`t\'\fR\ 
" toolkit_args" for access to the standard X toolkit controls, such as
for geometry, fonts, etc. 
.PP
Usage example:
.br
\&\f(CW\`starXaw -c sgr -a p -a "t -geometry -0-0 -bg red"\'\fR.
.PP
.NH 3
\&\fBstarpost\fR
.br
.PP
\&\fBstarpost\fR is the PostScript version of starchart.  It has the
following special arguments which are accessed through \f(CW\`-a\'\fR, e.g.
.br
\&\f(CW\`-a "m9 8 7 6 5 4 3.5 2.5 2 1.5 1 0.5"\'\fR. 
.PP
.IP \f(CW\`n\'\fR\ 
notebook: The chart is shifted to the right on the page, and the
``thumbnail'' map is replaced with prompts and space for recording
observing conditions.  The rest of the argument string is taken to be a
list of floating point numbers separated by `:' or ` '.  These are taken
to be the radii in degrees of fields of view of the instrument or
finder.  Circles with these radii are drawn, centered on the center of
the chart. 
.IP \f(CW\`m\'\fR\ 
magnitude: 0.1 magnitude quanta are to be used (the default is 1.0
magnitude, e.g. a 4.6 magnitude star is represented by the same symbol
as a star of magnitude 5.4).  The rest of the argument string is taken
to be a list of floating point numbers separated by `:' or ` '.  These
are taken to be the radii of the symbols for stars of magnitude -1, 0,
1, 2, 3 etc. The sizes of symbols for stars with magnitudes between
these magnitudes are determined by linear interpolation.  Note that the
last value given is the value assigned to all magnitudes fainter than
that magnitude.  Current default sizes are "9 8 7 6 5 4 3.25 2.5 2 1.5 1
0.5". 
.IP \f(CW\`s\'\fR\ 
size: this argument provides an alternative way of specifying
magnitude symbol sizes, using a formula proposed by Andrew Young in
Sky and Telescope, March 1990, p. 311, D = D0 * 10^(k*(v0 - v)).
He proposes a k of between 0.07 and 0.16, with 0.11 as probably the
best.  If \f(CW\`s\'\fR uses 0.1 magnitude quanta as does \f(CW\`m\'\fR.  The
default values for \fID0\fR, \fIv0\fR, and \fIk\fR are 0.5 (points),
the limiting magnitude of the first main map, and 0.11, respectively.
These may be altered with the optional arguments to \f(CW\`s\'\fR, which set
\&\fID0\fR, \fIv0\fR and \fIk\fR in that order, e.g. \f(CW\`s 0.7 6.6\'\fR sets
\&\fID0\fR to 0.7 point, and \fIv0\fR to magnitude 6.6 so that a magnitude
6.6 star will be printed as 0.7 point in diameter, and \fIk\fR will
remain 0.11.  \f(CW\`s 0.7 6.6 0.07\'\fR will set \fID0\fR and \fIv0\fR as
above, and also set \fIk\fR to 0.07. 
.IP \f(CW\`f\'\fR\ 
\&\f(CW\`f \fIfile\fR\'\fR may be used to specify an output file, otherwise the
PostScript produced is sent to the standard output. 
.IP \f(CW\`c\'\fR\ 
may be used to produce color postscript output. 
.IP \f(CW\`l\'\fR\ 
may be used to specify landscape mode (as opposed to the default
portrait orientation).  Landscape mode can only used with a
fullpage map.  
.PP
Since the resolution of PostScript is much higher than other common
devices, \f(CW\`mapwin\'\fR files must be edited when transferred between
starpost and other drivers.  The controls for window \f(CW\`width\'\fR,
\&\f(CW\`height\'\fR, \f(CW\`x_offset\'\fR and \f(CW\`y_offset\'\fR must be changed. 
.PP
The \f(CW\`-a p\'\fR or \f(CW\`-a l\'\fR option used with \fBstarX11\fR or
\&\fBstarXaw\fR can be used to produce a \f(CW\`mapwin\'\fR file which can be
converted for use in \fBstarpost\fR through an \fBawk\fR script named
\&\fBpostconv.awk\fR, provided with the source to this program. 
.PP
Usage example:
.br
\&\f(CW\`starpost -a "n 6:1.5:.5" -a "m9 8 7 6 5 4 3.5 2.5 2 1.5 1 0.5" -a "f orion.PS" -c ori -g 5\'\fR.
.PP
.NH 3
\&\fBstartool\fR
.br
.PP
\&\fBstartool\fR uses the \fBtooltool\fR program, not included in the
starchart distribution, to provide an interface to \fBstarsunv\fR.
\&\fBttooltool\fR is available from your local Sun sources archive.
.PP
.NH 2
Writing\ a\ New\ Version\ for\ a\ New\ Device
.br
.NH 3
Organization
.br
.PP
There are several layers of functions in starchart.  The top level is in
\&\f(CW\`starmain.c\'\fR, \f(CW\`starm2.c\'\fR, and \f(CW\`readfile.c\'\fR.  It is (I
hope) general enough to enable any additions to be made by users in the
form of modified drivers, leaving these files stable.  \f(CW\`starmain.c\'\fR
and \f(CW\`starm2.c\'\fR are in charge of the user interface and file
reading, with \f(CW\`readfile.c\'\fR.  Generally, this top level controls
\&\fIwhat\fR is to be drawn, and \fIwhere\fR it appears.  The device
specific section controls \fIhow\fR it appears.
.PP
Information is passed to the drivers via externs and function
parameters.  The externs are for runtime values, the function parameters
are for values taken from the data files.  Information is passed from
the driver to the starchart.c routines via externs.
.PP
There are three layers of functions, in several source files.  The top
level functions are in \f(CW\`starmain.c\'\fR and \f(CW\`starm2.c\'\fR.  You
should not have to change anything in this file, except perhaps #defines
for default file names and file types, and these may also be set in the
Makefile.
.PP
The \f(CW\`starcust.c\'\fR file contains customizing functions, including functions
to define additional command line flags, which may apply to several
drivers.
.PP
The \f(CW\`starXXXX.c\'\fR or \f(CW\`starimages.c\'\fR + \f(CW\`starXXXX.c\'\fR files
define the more device dependent functions.  There is one function,
called \f(CW\`drawobj\'\fR, which is the interface between the top level
functions and the device for drawing the stars, planets, and other
objects read from a database file.  In addition, standard functions to
open and close the device, draw a line, move without drawing, etc. are
required.  These may then call the lowest level device dependent
functions.  The \f(CW\`starimages.c\'\fR file provides a standard definition
for \fBdrawobj\fR and supporting functions for bitmapped devices; the
device dependent code remaining (e.g. line drawing), is in the file
\&\f(CW\`starXXXX.c\'\fR.
.PP
User input functions also may be defined in \f(CW\`starXXXX.c\'\fR.
.PP
The layer currently being drawn and/or the subroutine currently active
is passed to the driver in an extern.  This allows the driver to for
example draw vectors differently for the constellation boundaries.
Additionally, each window has a map type field which is for efficiency
also passed as an extern, which may be used by the driver to for example
draw certain layers differently in certain windows.
.PP
The source to other drivers, especially \fBstarXaw\fR and
\&\fBstarpost\fR, should be a valuable reference to developing new
drivers.
.PP
.NH 3
Customization
.br
.PP
For the drivers distributed, there are many \f(CW\`#defines\'\fR and global
variables whose values may be altered to suit your tastes and your
particular piece of hardware.  Default file names are \f(CW\`#defined\'\fR in
the code, and may also be set in the Makefile.
.PP
It is easy to write a driver for other devices, based on the drivers and
the driver skeleton provided.
.PP
More extensive customization is possible through modifying the driver
functions to take fullest advantage of the device you are using.  It is
also possible to add controls accessed through the \f(CW\`-a\'\fR command
line option and the \f(CW\`additional\'\fR (= \f(CW\`driver_control\'\fR) variables.
.PP
Please try to send me (ccount) a note before writing any new drivers, so
I can let you know if a driver for that device is in the works.  Any
improvements to drivers should be sent to me or the author (if they are
not the same), when you're sure they work and are happy with the
changes.
.PP
Things you define in \f(CW\`starcust.c\'\fR, if you think they are generally
useful, should be sent to me.  If possible, they may be incorporated in
future releases.
.PP
Improvements to top level routines should be made with great caution.
Such changes will make it harder for you to use future refinements.  If
you're sure you're change (a) works and (b) is better/stronger/faster,
please send it to me.
.PP
Do report any bugs/problems.
.PP
.bp
.NH
The\ Data\ Files\ and\ Their\ Formats
.br
.PP
This section discusses file formats used by the programs.
.PP
.PP
.NH 2
Contents\ of\ Data\ Files
.br
.PP
Each line of file contains:
.IP \fIra,\ dec,\ mag\fR\ 
Required.  Right ascension, Declination, and magnitude (V or visual).  6
chars for RA, one for sign of DEC, 4 for DEC, 3 for mag. 
.IP
.IP \fIobject\ type\fR\ 
Defines what the object is: star, galaxy, cluster, nebula, planet,
other, unknown, vector, area, comment, or invisible, with subtypes for
each (Two chars).  Defaults to type star subtype single. 
.IP
.IP \fIspectral\ class\ or\ color\fR\ 
Defines the color of the object, as spectral class for stars or directly
for other objects (Two chars). 
.IP
.IP \fIletter\ or\ flamsteed\ number\ or\ size\fR\ 
Defines either the Bayer letter or Flamsteed number of a star, or the
size of an extended object encoded in two chars. 
.IP
.IP \fIconstellation\fR\ 
The constellation field contains the IAU designation of the
constellation the object is in (three chars). 
.IP
.IP \fIname\fR\ 
Name or other labeling string.  Terminated by comma. 
.IP
.IP \fIcomment\ field\fR\ 
Remainder of line after the comma which terminates the name.  This
comment field may be used for special information about the object, e.g.
the phase of the moon. 
.PP
Magnitude is encoded into three fields as follows: the first character
may be a \f(CW\`-\'\fR, in which case the next two characters are the magnitude
times 10, e.g. \f(CW\`-16\'\fR means \f(CW\`-1.6\'\fR.  The first character is a digit,
the three characters are the magnitude times 100, e.g. \f(CW\`563\'\fR means
\&\f(CW\`5.63\'\fR.  Finally, if the first character is a capital letter, this is
taken as the base-36 value of the integral part of the magnitude, and
the remaining two characters are the fractional part times 100, e.g.
\&\f(CW\`B34\'\fR means \f(CW\`11.34\'\fR.
.PP
The Bayer letter or Flamsteed numbers are applicable only to stars.  The
field is two characters wide.  Greek letters are a single lowercase
letter followed by a space or a number, encoded as in the PostScript
Symbol font:
.PP
.nf
.br
  
alpha	- a	beta	- b	gamma	- g	delta	- d
epsilon	- e	zeta	- z	eta	- h	theta	- q
iota	- i	kappa	- k	lambda	- l	mu	- m
nu	- n	xi	- x	omicron - o	pi	- p
rho	- r	sigma	- s	tau	- t	upsilon	- u
phi	- f	chi	- c	psi	- y	omega	- w
.fi
.PP
Note that theta, xi, phi, chi, and psi have changed from the original
definitions, which were @, E, 0, x, and % respectively.  Note also that
a variant version of phi is mapped to j. 
.PP
An uppercase letter followed by any character or two non-numeric
characters is the roman letter designation for the star, e.g. \f(CW\`CY\'\fR.
Two numbers or a space and a number is the Flamsteed number of the star.
.PP
The size of nonstellar objects is encoded in seconds of arc in the size
field to two significant digits.  The second character is always a
decimal digit.  Sizes from 0 to 99 arc seconds are encoded as two
decimal digits.  For larger objects the first character is a capital
letter, interpreted as follows, with the second character as the next
digit:
.PP
.nf
.br
   
First character		Value
0 - 9			00 - 90
A - I			100 - 900
J - R			1000 - 9000
S - Z			10000 - 80000
.fi
.PP
Examples:
.PP
.nf
.br
   
Size field	Value
 6		6
09		9
73		73
A0		100
C3		330
D5		450
I6		960
J2		1200
R3		9300
S6		16000
Z0		80000
Z9		89000
.fi
.PP
The types and subtypes implemented are:
.PP
.nf
.br
   
Type[0]		main type	type[1]		subtype
C		cluster
				G		globular
				O		open
				(other) 	other

G		galaxy
				a		spiral Sa
				b		spiral Sb
				c		spiral Sc
				d		spiral Sd
				p		spiral Sp
				Z		spiral S0
				s		spiral unspecified
				A		barred spiral SBa
				B		barred spiral SBb
				C		barred spiral SBc
				D		barred spiral SBd
				P		barred spiral SBp
				S		barred spiral unspecified
				0-7		elliptical E0-E7
				E		elliptical
				I		irregular I
				Q		quasar
				U		unknown
				!		peculiar
				 		unspecified
				(other) 	other

N		nebula
				D		diffuse
				P		planetary
				(other) 	other

P		planet
				M		Mercury
				V		Venus
				m		Mars
				J		Jupiter
				s		Saturn
				U		Uranus
				N		Neptune
				P		Pluto
				A		Asteroid
				C		Comet
				S		Sun (Sol)
				L		Moon (Luna)
				(other) 	other

S		star
				S		single
				D		double
				V		variable

O		other
				(any)		other object

U		unknown
				(any)		unknown object

V		vector
				M		moveto (begin, no line drawn)
				S		solid lineto via great circle
				D		dotted lineto via great circle
				H		hyphened (dashed) lineto g. c.
				m		moveto linear
				s		solid lineto linear
				d		dotted lineto linear
				h		hyphened lineto linear

I		invisible
				(any)		for labels

#		data comment
				(any)		for comment in file

A		area
				M		moveto (begin)
				A		add great circle border segment
				F		fill region and end region
				m		moveto (begin)
				a		add linear border segment
				f		fill region and end region
.fi
.PP
.PP
The magnitude of an object determines what information is displayed.
Three magnitude limits are defined for each file in each map window.  If
the magnitude is greater (dimmer) than the first limit, nothing is
drawn.  If the object is brighter than a second limit, the label (Bayer
or Flamsteed) is printed (if present).  If it is brighter than the third
limit, the name is printed (if present; if not, the label is printed if
present).  These text strings generally appear to the right of the
object, but the driver can change this.  Note: the label string only
applies to stars.
.PP
For nonstellar objects, the label field defines the size of the object
in arc seconds.
.PP
For magnitudes within a range defined for the mapwindow, a magnitude
tag, the magnitude to one decimal place as in variable star and asteroid
finding charts, may be displayed.  This will generally be below and to
the right of the object, but this is controlled by the driver.
.PP
The symbol drawn for the object is determined by the type field.
.PP
For devices supporting color, the spectral class or color field defines
the color of the object, and perhaps the associated text.  The spectral
class of stars is mapped to display color, and for other objects, this
field may contain direct color specifications, e.g. 'r3' for an object
colored red level 3.  All this is controlled by the device driver.
Standards for color definitions are yet to be defined.
.PP
The constellation field is not currently used in the display.
.PP
The comment field is also not generally used, but some drivers may use
it to contain special information such as the phase of the moon or
position angle of a comet's tail.
.PP
.NH 2
Data\ file\ formats
.br
.PP
.PP
.NH 3
\&\f(CW\`lineread\'\fR
.br
.PP
This text format, called \fIlineread\fR, is the original data file format.
Data for each object is on a single line, stored in character fields.
Currently, an older variant is also supported, one which has 4
characters for the magnitude and omits the color and letter/number
fields.  Both of these formats are designated as type \fIlineread\fR,
since each line is read individually and these two types are easily
distinguished on a line by line basis.  In all formats, if a value is
unknown, it should be spaces, or a null string in the case of the name
and comment fields.  The RA, DEC and mag. must always be provided for an
object.
.PP
Since it is a text format, it may be mailed safely and be used on many
different computer architectures without change.  However, it is slow,
and uses more file space than it ought.
.PP
.PP
.NH 3
\&\f(CW\`sif\'\fR:\ Starchart\ Interchange\ Format
.br
.PP
The \fIsif\fR format was introduced to allow data to be exchanged as text,
with greater precision in the location and magnitude fields, and greater
readability compared to the \fIlineread\fR format.  The format is too slow
for use as input to the starchart programs.  A data conversion program
is provided to convert between formats, and to precess coordinates
during the conversion.
.PP
Each object is represented by a single line in the file.  This line
contains fields each of which corresponds to a field in the
\&\fIlineread\fR format.  The fields are separated by a single character,
usually \f(CW\`;\'\fR.  Fields may be omitted from the end of the line;
omitted fields are assigned their default values.  Fields may be empty,
and again are assigned their default values.  As with the other formats,
the RA, DEC, and mag. must be present.
.PP
The RA may be given as a decimal hour, hour and decimal minute, or hour
minute and decimal second.  The DEC may be given in the same format.
The magnitude is a floating point number.
.PP
The type, and color, fields are empty or one or two characters, as in
the \fIlineread\fR format.
.PP
The label field is also the same as in the \fIlineread\fR format, and
encodes the size of non-stellar objects in two characters.  There is a
bit of ``magic'' in this field.  Since \f(CW\`X \'\fR or \f(CW\` X\'\fR are both
valid, and whitespace is normally ignored, iff this field is two
characters wide, and both characters are printing characters, it is
taken verbatim as the label field.  That is, \f(CW\`... ; X; ...\'\fR is
\&\f(CW\` X\'\fR, while \f(CW\`... ; X ; ...\'\fR is read as one character and left
justified to become \f(CW\`X \'\fR. 
.PP
The constellation field is the IAU abbreviation for the constellation
and is always blank or three characters.
.PP
The name and comment are two separate fields.  Commas should not be used
in the name field, since when this name is placed in the other formats a
comma is used to separate the name from comments.
.PP
.NH 3
Binary\ Formats
.br
.PP
Three binary input formats are supported.  These formats provide greater
position and magnitude accuracy, with faster input, and varying degrees
of storage space reduction.  The files are inherently unportable,
however, and should not be exchanged between machines and operating
systems, or even different compilers.  The \fIsif\fR format, described
above, provides a general data exchange format.
.PP
The three formats provide different storage sizes.  The most general
format, which contains all the data in the \fIlineread\fR format, is the
\&\fIbinfull\fR format.  A smaller format, \fIbinobj\fR stores only the
RA, DEC, mag. and object type fields, all other fields become their
defaults (usually spaces).  The smallest format, \fIbinstar\fR stores
only the RA, DEC and mag., all other fields become their defaults,
notably the \fIobject type\fR is \f(CW\`SS\'\fR.
.PP
The binary formats are described as \fBC\fR structures.  How the data in
the structure is stored is therefore highly variable between compilers,
operating systems and machines.  However, most machines should be able
to take advantage of these formats for local storage.
.PP
.NH 3
The\ Guide\ Star\ Catalog
.br
.PP
The Hubble Space Telescope Guide Star Catalog, available on 2 CD-ROMs,
may be used as a stellar database.  This format may be read by
\&\fBdataconv\fR and the starchart programs, but now written.  An index
file is used to specify which files on which CD-ROM should be read.
.PP
.NH 3
The\ Index\ File\ index.indx,\ \f(CW\`indextype\'\fR
.br
.PP
The \fIindextype\fR file is a special file format added to support larger
databases of dimmer stars.  It provides an index mapping location to
filenames of files containing star data as above.  Each indexed file
covers a rectangle of sky in RA and DEC.  The areas may be different
sizes for different files.  The area covered is given as the RA and DEC
of the upper left and lower right corners, followed by the filename and
a string indicating the type of the file.  This is all on one line in
the file.  The format is then:
.PP
.nf
.br
   
upper left   upper left   lower right   lower right   filename   filetype
RA	     DEC	  RA		DEC	      filename	 filetype

e.g.
02.0	     -30.0	   00.0	        -90.0	      SAO00-90	 lineread
04.0	     -30.0	   02.0	        -90.0	      SAO02-90	 lineread
06.0	     -30.0	   04.0	        -90.0	      SAO04-90	 lineread
08.0	     -30.0	   06.0	        -90.0	      SAO06-90	 lineread
10.0	     -30.0	   08.0	        -90.0	      SAO08-90	 lineread
.fi
.PP
.PP
.bp
.NH
The\ Support\ Programs
.br
.PP
Several support programs are provided for use with the primary
\&\fBobserve\fR and starchart programs.
.PP
.PP
.NH 2
\&\fBdataconv\fR
.br
.PP
This program is used to convert between file formats, especially to
generate binary format files.  It can precess coordinates as desired.
Remember, input data files for starchart programs should be in 2000.0
coordinates.
.PP
.NH 2
\&\fBsif-to-text\fR
.br
.PP
The \fBsif-to-text\fR or \fBsif_to_text\fR program converts sif format
files to more readable text tables.
.PP
.NH 2
\&\fBtosize\fR
.br
.PP
This program is used to encode the size of objects, for development of
new databases.
.PP
.bp
.NH
Acknowledgements
.br
.PP
The previous versions of starchart were developed by Alan Paeth.
.PP
Steve Kennedy contributed to \f(CW\`starimages.c\'\fR.
.PP
Robert Tidd provided the original \f(CW\`yale.star\'\fR and the format.
.PP
Dave Van't Hof provided most of the other databases (in particular boundaries,
patterns, and names).
.PP
Michael Campanella provided vms support.
.PP
The Atari ST version was developed by Holger Zessel and Markus Kempf.
.PP
Tom Horsley contributed the ibm PC driver.
.PP
Mike Daly helped with the Macintosh version.
.PP
Joe Wang helped with the X11 version.
.PP
Dean Payne provided the original code for \f(CW\`.starrc\'\fR processing.
.PP
Other contributions are from Petri Launiainen, Jyrki Yli-Nokari, Sjoerd
Mullender, David Smith, Tim Pointing, and others.
.PP
.PP
.bp
.SH
Program\ Index
.br
.PP
.PP
.bp
.SH
Concept\ Index
.br
.PP
.PP
.PP
