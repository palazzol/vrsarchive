.nr VS 14
.nr PO 1.25i
.nr LL 6.0i
.nr HM 1.25i
.nr FM 1.25i
.TL
OFF - A 3D Object File Format
.AU
Randi J. Rost
6-November-1986
.AI
Digital Equipment Corporation
Workstation Systems Engineering
100 Hamilton Ave.
Palo Alto, Ca. 94301
.DA
.AB no
This document describes the data format developed by WSE for the
interchange and archiving of three-dimensional objects.
This format, called OFF (for Object File Format), is general,
flexible, and extensible.  It supports ASCII text versions of
objects for the purpose of interchange, and binary versions for
efficiency of reading and writing.  It is assumed that applications
will develop their own, more efficient format for internal storage
and operation on three-dimensional objects.
.AE
.PP
.NH 1
Introduction
.PP
One of the most time-consuming tasks in computer animation projects
is designing the 3D models that will be used.  Many computer animation
houses have found that owning a large number of databases makes it
easier for them to take on new projects at a lower cost (time and $$$).
The cost of initially creating an object can be amortized over the
number of times it can be re-used.
It is our intention to promote the use of OFF files within (and perhaps
even outside of) Digital in an effort to build up our collection of useful
3D models.
.PP
The file format itself is not limiting:  OFF files can be used for
a wide variety of object types.  None of the "policy decisions" are
hard-wired in the design of the file format, or in the support library
routines that allow reading and writing of OFF files.
Rather, the policy decisions have been left
up to the designers since the format supports "generic" object definitions.
We have developed specific conventions for objects that are defined
by polygons for use within WSE, and we'd encourage others to adopt
these conventions as
well in order to promote the interchange of useful object data bases.
The XModel application (also
developed by WSE) is an example of an application that permits
reading and writing of OFF files.
.PP
This paper describes the Object File Format itself, the conventions
we've adopted at WSE, and the library of support routines that can
be used to read and write OFF files.
.PP
.NH 1
Design Goals and Non-Goals
.LP
Design goals for the Object File Format include:
.IP 1)
\fBSimple.\fP  Simple cases should be simple.  It should be possible
to type in all the data required for a simple object (such as a cube)
by hand.
.IP 2)
\fBPowerful.\fP  The Object File Format should be capable of
accomodating complicated objects (many vertices, polygons, and
a wide range of attributes).
.IP 3)
\fBPortable.\fP  ASCII text file representation required for portability
across operating systems and hardware.  This also allows
operations on OFF files by familiar system utilities
(text editors and the like).
.IP 4)
\fBEfficient.\fP  Binary text file representation required to allow
efficient reading and writing of OFF files.  It is assumed that reading/writing
an object is a costly operation, but reading and writing ASCII data is just
\fItoo\fP slow.
.IP 5)
\fBGeneral.\fP  The format should address the general case of the
three-dimensional object, not a single particular case.
.IP 6)
\fBExtensibile.\fP  Make sure the format can be easily extended to
eventually support other primitives such as bezier patches.
.IP 7)
\fBNo Favors.\fP  Avoid hard-wiring policy decisions.  Rather, provide
generic building blocks capable of supporting several styles and document
a set of strongly encouraged conventions that we have adopted.
.LP
There are also things that were specifically non-goals in the design
of the Object File Format.
.IP 1)
\fBInternal Format.\fP
The Object File Format is not intended to be forced upon applications
as an internal format that must be used.  Rather, it should be considered
a means for inputting and outputting object descriptions in a 
device-, language-, and operating system-independent format.
Applications should feel free to develop and maintain the most useful/efficient
data format they can, and only convert to/from OFF when input or output
of a standardized object is desired.
.IP 2)
\fBObject Conventions.\fP
OFF conventions are documented only for objects in polygonal form at
this point.  It is anticipated
that the Object File Format can be easily extended to handle bezier
surface patches and other primitives in the future.
.NH 1
Objects
.PP
For the purposes of the Object File Format, we'll adopt a very general
definition of an \fIobject\fP.
An \fIobject\fP is simply a list of properties
(name, description, author, copyright information, geometry data, colors,
etc.)
.PP
The most important information about the object can be found in the
\fIheader file\fP for the object.  The header file is always an ASCII text file
that, by convention, is named \fIname\fP.off where \fIname\fP is the
object name.  See Appendix B for an example of an OFF object header file.
.PP
A few of these properties (name, description, author, copyright, type)
are common to every type of 3D object and are considered standard
properties.  The standard properties are built into the routines
that manipulate 3D objects.  The rest of the properties may vary
with the type of object, and so are defined by convention only.
.PP
The \fIname\fP of an object is used to concisely describe the object
itself.  For example, we have objects named "x29", "banana" and "vw".
By convention, this name also becomes the prefix for OFF data filenames
when an object is read or written, so it is best to keep it fairly short.
.PP
The \fIdescription\fP is used to more fully describe the object itself.
It may contain the time and date of creation or more prose describing
the object.
.PP
The \fIauthor\fP should be the name of the person (or company, or utility) that
created the object.  We should always try to give credit where credit is due.
This field tells you who to thank for spiffy objects or whose cage to rattle
when a problem with an OFF file is discovered.
.PP
The \fIcopyright\fP field contains information dealing with the distribution
of the object data.  Some object databases will be regarded
by a company as proprietary.  These objects should not be copied or
distributed without consent.  Other objects (vw, x29) were developed
by companies or individuals and can be copied or used as long as the
copyright notice appears and proper credit is given.  Still other objects
(cube, sphere, etc.) have been placed in the public domain.  We have
tried to be as careful as possible in preserving copyright and author
information for the objects we have collected, but sometimes the information
was lost or unavailable.  Be sure and honor copyright notices.  If you don't,
you (or your company) could end up in big trouble.
.PP
The \fItype\fP field contains the type of the object.  For now, only
two types of objects are supported: polyline objects and polygon objects.
It is anticipated that surface patch-type objects will be supported in
the future as well.
.PP
Also contained in the object header file are lines that describe
the various properties of the object.  Each line in the object header
file that describes an attribute of the object other than a standard
attribute must contain the
property name, the property type, the data format
and either a file name or a string containing default data,
depending on the property type.  Each of these four items is
an ASCII string, separated by white space.
.PP
The \fIproperty name\fP uniquely describes the property.  Property
names for which we have defined conventions (see Appendix A) include
geometry, polygon_colors, vertex_colors, back_faces, vertex_order,
normals, diffuse_coef, specular_coef, and specular_power.
.PP
OFF currently supports three \fIproperty types\fP:
\fIdefault\fP, \fIgeneric\fP,
and \fIindexed_poly\fP.  If a property is indicated to be of type
\fIdefault\fP, the part of the line after the data format is assumed
to contain some default data that will be applied to the entire object.
For instance, it may make sense to give the entire object a default color
and default diffuse and specular coefficients.
.PP
If the property type is either generic or indexed_poly (described more
fully below), the remainder of the line is taken to be a file name that
can be opened and read to obtain the information for the property.
.PP
The data format indicates what type of data will be found on the remainder
of the line if the property type is default, otherwise what kind of
data will be found in the specified file.  The data format is a
string of characters (no spaces) that indicate the order and type of
the data.  Supported primitive data types are:
.IP "f -"
A number stored internally as a 32-bit floating point number
.IP "d -"
A number stored internally as a 64-bit double-precision floating point number
.IP "i -"
A number stored internally as a 32-bit integer value
.IP "h -"
A number stored internally as a 16-bit integer value
.IP "b -"
A number stored internally as an 8-bit integer value
.IP "s -"
A 32-bit pointer to a null-terminated string of characters
.PP
If, for instance, you were interested in using 32-bit floating values
for r, g, and b default color values, you might have a line in the
object header file that reads
.DS
polygon_colors   default   fff   1.0   0.8   0.0
.DE
.PP
It is important to understand that in all cases, the "string" (s) data
primitive will indicate a pointer to a string
that is stored internally in the data block for an object and not
the string itself.
.NH 1
ASCII Property Files
.PP
OFF supports ASCII text files as a way of providing for language-, hardware-,
and operating system-independent object data files.  The two types of
data files currently supported by OFF are generic and indexed_poly.
.NH 2
Generic Files
.PP
Generic files contain only a \fIcount\fP value followed by \fIcount\fP
data items of the
type specified by the data format.
Each data item can be comprised of some combination of the primitive data
types described in Section 3.
Generic data files are useful for storing attributes
which are unique at every vertex or polygon (such as color).
.PP
String data items in ASCII generic files may not contain spaces or
other white space.  8-bit integers must be listed in the range 0-255.
.PP
See Appendix D for an example of an ASCII generic data file.
.PP
.NH 2
Indexed_Poly Files
.PP
Indexed_poly files take advantage of a connectivity list to reduce
the amount of information needed
to store a list of polylines, polygons, or normals.
The unique geometry items (vertices or normals) are listed in the first
part of the file.  Following this list is a connectivity list.  Each line
in the connectivity list contains a \fIcount\fP value
followed by \fIcount\fP indices (pointers)
to information in the geometry list.  (Items in the geometry list are
indexed starting from 1, not 0.)
.PP
The first line of an indexed_poly data file contains three integers,
separated by white space.  The first number on this line indicates
the number of data items (vertices/normals) that follow, the second number
indicates the number of polylines/polygons that follow the
data list, and the
third indicates the total number of edges that are contained
in the polyline/polygon connectivity list.
.PP
String data items in ASCII indexed_poly files may not contain spaces or
other white space.  8-bit integers must be listed in the range 0-255.
.PP
See Appendix C for an example of an ASCII indexed_poly file.
.PP
.NH 1
Binary OFF Files
.PP
The same two types of data files described above are also supported
in binary format.  There are a few minor differences.
.PP
.NH 2
Generic Files
.PP
Binary generic files begin with the first 32-bit word equal to 
OFF_GENERIC_MAGIC as defined in the include file \fIoff.h\fP.
The second word in the file is the \fIcount\fP (number of data items in
the file).  Following the \fIcount\fP is the data itself.
.PP
The data format in the header file describes the primitives that
make up each data item in the list.
Floats, doubles,
32-bit integers, and string pointers will all begin on a word boundary.
16-bit integers will all begin on a half-word boundary.  Strings begin
with a 32-bit \fIcount\fP followed by \fIcount\fP characters followed by a null
character.  The string is null-padded so it will end on a word boundary.
.PP
(It is assumed that for strings, the length will be read and then
the necessary memory will be allocated and the string read in.  This eliminates
a problem with having variable-length data in the data files.  Anyway,
strings in files are really only there for symmetry with default values,
where strings are really useful.  The performance implications for files
containing strings will probably be enough to prevent people from using
them.)
.PP
In addition, each data item will begin on a word boundary.  Thus, if
your data format for the data items in a generic data file is "bbb"
(three byte values), each data item will be stored as three bytes followed
by a null byte so that each data item wil begin on a word boundary.
.PP
.NH 2
Indexed_Poly Files
.PP
Binary indexed_poly files begin with the first 32-bit word equal to 
OFF_INDEXED_POLY_MAGIC as defined in the include file off.h.
The second word is the number of data items
in the vertex/normal list (\fInpts\fP),
the third word is the number of polylines/polygons
in the list (\fInpolys\fP), and
the fourth word is the number of edges contained in
the connectivity list (\fInconnects\fP).
.PP
Starting at the fifth word in the file
is a list of \fInpts\fP data items, followed by \fInpolys\fP short integers
containing polyline/polygon vertex counts, followed by \fInconnects\fP
short integers which are indices into the array of data items.  (This
arrangement is slightly different than that used for indexed_poly files
in ASCII format for efficiency reasons.)
.PP
The same restrictions that
apply to the data types for generic binary files apply to indexed_poly
binary files as well.  In addition, the vertex count array which
follows the geometry data in an indexed_poly file will always begin
on a word boundary.  The connectivity array that follows the vertex
count array will not necessarily start on a word boundary, but will
always begin \fInpolys * sizeof(short)\fP bytes after the start of the
vertex count array.
.PP
.NH 1
Liboff.a and Objects.h
.PP
An include file and a library of routines has been provided for UNIX/C
programmers to more easily manipulate OFF files.  The basic concepts
of "reading" and "writing" OFF files are supported in this library
of routines.  The library is a software layer on top of
the operating system file I/O interface, with special knowledge of
OFF files.  This subroutine library provides a mechanism for accessing
the syntactical elements of an object file, but makes no attempt to understand
the semantics.  Higher level interfaces can be layered on top.
.PP
The subroutine library refers to an object as a pointer to an
\fIOFFObjDesc\fP.  This structure contains
a pointer to the first property in the
property list.  It is defined as follows:
.DS
typedef struct
    { 
    OFFProperty	*FirstProp;	/* Pointer to first property in list */
    } OFFObjDesc;
.DE
.PP 
The information that describes the object is contained in a linked
list of property structures.  The first such structure in the list
is pointed at by an \fIOFFObjDesc\fP structure.  The property structures
have the form:
.DS
typedef struct _OFFProp
    {
    char	PropName[40];
    int		PropType;
    char	PropFileName[256];
    int		PropCount;
    char	DataFormat[40];
    char	*PropData;
    struct _OFFProp *NextProp;
    } OFFProperty;
.DE
.PP
\fIPropName\fP contains a string defining one of the property types
for which a convention has been defined.
This includes the property names
"name", "author", "description", "copyright", "comment",
"geometry", "polygon_color", "normals", etc.  For a complete
list of property names, see Appendix A.
(The special attribute type "comment" is 
supported so that blank lines and comment lines can be preserved if 
an object file is read and then written.)
.PP
The \fIPropType\fP field contains a value equal to \fIOFF_DEFAULT_DATA\fP,
\fIOFF_GENERIC_DATA\fP, or \fIOFF_INDEXED_POLY_DATA\fP which defines
the basic type for the property.
.PP
The \fIPropFileName\fP is required if \fIPropType\fP is something
other than \fIOFF_DEFAULT_DATA\fP.  It contains a string representing the name
of the file to be read/written for this attribute.  This file name
should \fInot\fP contain a path leading up to the file itself, only
the actual file name.  The object search path mechanism (see Section
7) should be used instead.
.PP
The \fIPropCount\fP indicates the actual number of data items associated
with this particular attribute.  After reading in an object, properties
of type \fIOFF_DEFAULT_DATA\fP will have a \fIPropCount\fP of one, properties
of type \fIOFF_GENERIC_DATA\fP will have a \fIPropCount\fP equal to the number
of generic data items in the list, and properties of type
\fIOFF_INDEXED_POLY_DATA\fP
will have a \fIPropCount\fP equal to the number of data items in the
geometry list.
.PP
The \fIDataFormat\fP field contains a string of characters corresponding
to primitive data items.  The composite type of the data for this property
can then be deduced by looking at this field and applying the rules for
padding to word and half-word boundaries.
.PP
The \fIPropData\fP field contains a pointer to a block of memory containing
the actual data for this property.  This data will have the same
data alignment restrictions as a binary file has, with the exception
of strings.  As strings are read in, memory is malloc'ed to hold them
and a pointer to the string is stored in the appropriate field in the
data list.  This means that all primitive data types will have a fixed
size and lengths and alignments can be computed more easily.
.PP
The \fINextProp\fP field contains a pointer to the next property structure
in the property list.
.PP
The routines contained in the subroutine library are defined below.
.sp 1
.LP
.nf
\fB#include "off.h"\fP
.sp 1
int \fBOFFReadObj\fP(Obj, FileName)
	OFFObjDesc *Obj;
	char *FileName;
.sp 1
int \fBOFFWriteObj\fP(Obj, FileName, Directory, FileType);
	OFFObjDesc *Obj;
	char *FileName;
	char *Directory;
	int FileType;
.sp 1
int \fBOFFReadGeneric\fP(Property, FileName)
	OFFProperty *Property;
	char *FileName;
.sp 1
int \fBOFFWriteGeneric\fP(Property, FileName, FileType)
	OFFProperty *Property;
	char *FileName;
	int FileType;
.sp 1
int \fBOFFReadIndexedPoly\fP(Property, FileName)
	OFFProperty *Property;
	char *FileName;
.sp 1
int \fBOFFWriteIndexedPoly\fP(Property, FileName, FileType)
	OFFProperty *Property;
	char *FileName;
	int FileType;
.sp 1
OFFObjDesc *\fBOFFCreateObj\fP()
.sp 1
int \fBOFFDestroyObj\fP(Obj)
	OFFObjDesc *Obj;
.sp 1
OFFProperty *\fBOFFAddProperty\fP(Obj)
	OFFObjDesc *Obj;
.sp 1
int \fBOFFRemoveProperty\fP(Obj, PropertyName)
	OFFObjDesc *Obj;
	char *PropertyName;
.sp 1
int \fBOFFFreeProperty\fP(Property)
	OFFProperty *Property;
.sp 1
.PP
\fIOFFReadObj\fP will attempt to open the object header file named
\fIFileName\fP
and read the object data it contains.  A pointer to the constructed
object structure will be returned in \fIObj\fP when the object has been read.
An attempt will be made to open the specified file first as given,
then concatenated in turn with each of
the directories specified by the environment
search path variable \fIOBJ_PATH\fP.
The property list for the object is built as the file is
read.  Upon return, the client need only traverse the
property list and select the data it needs.  This routine
calls \fIOFFReadGeneric\fP and \fIOFFReadIndexedPoly\fP 
in order to read associated data files.
\fIOFFReadObj\fP will return 0 if the read operation was successful,
-1 otherwise.
.PP
\fIOFFWriteObj\fP will attempt to write the object pointed at by
\fIObj\fP using the filename specified by \fIFileName\fP.  The
file will be written in the directory indicated by \fIDirectory\fP.
If \fIFileType\fP is \fIOFF_ASCII\fP, the file will be written as an ASCII
text OFF file.  If \fIFileType\fP is \fIOFF_BINARY\fP, the file will
be written as a binary OFF file.
The property list for the object is traversed and each
property of the object is written out in turn.
This routine
calls \fIOFFWriteGeneric\fP and \fIOFFWriteIndexedPoly\fP 
in order to write associated data files.
\fIOFFWriteObj\fP will return 0 if the write operation was successful,
-1 otherwise.
.PP
\fIOFFReadGeneric\fP will read the generic data file named \fIFileName\fP
(here \fIFileName\fP contains the full path name) into the
property structure pointed at by \fIProperty\fP.
This routine will allocate the space it needs in order
to read in the data.  A pointer to this allocated data space
will be stored in the \fIPropData\fP field of the specified
\fIproperty\fP as described earlier.
The entire object, including all
allocated memory resources
can later be deallocated by calling \fIOFFDestroyObj\fP.
This routine will not typically be called directly by applications.
\fIOFFReadGeneric\fP will return 0 if the read operation was successful,
-1 otherwise.
.PP
\fIOFFWriteGeneric\fP will write the generic data associated with
\fIProperty\fP into the file \fIFileName\fP (here \fIFileName\fP
contains the full path name of the file to be written).  
If \fIFileType\fP is \fIOFF_ASCII\fP, the file will be written as an ASCII
text generic data file.  If \fIFileType\fP is \fIOFF_BINARY\fP, the file will
be written as a binary generic data file.
This routine will not typically be called directly by applications.
\fIOFFWriteGeneric\fP will return 0 if the write operation was successful,
-1 otherwise.
.PP
\fIOFFReadIndexedPoly\fP will read the indexed_poly data file named
\fIFileName\fP (here \fIFileName\fP contains the full path name) into the
property structure pointed at by \fIProperty\fP.
This routine will allocate the space it needs in order
to read in the data.  A pointer to this allocated data space
will be stored in the \fIPropData\fP field of the specified
\fIproperty\fP as described earlier.
The entire object, including all
allocated memory resources
can later be deallocated by calling \fIOFFDestroyObj\fP.
This routine will not typically be called directly by applications.
\fIOFFReadIndexedPoly\fP will return 0 if the read operation was successful,
-1 otherwise.
.PP
\fIOFFWriteIndexedPoly\fP will write the indexed_poly data associated with
\fIProperty\fP into the file \fIFileName\fP (here \fIFileName\fP contains
the full path name of the file to be written).  
If \fIFileType\fP is \fIOFF_ASCII\fP, the file will be written as an ASCII
text indexed_poly data file.
If \fIFileType\fP is \fIOFF_BINARY\fP, the file will
be written as a binary indexed_poly data file.
This routine will not typically be called directly by applications.
\fIOFFWriteIndexedPoly\fP will return 0 if the write operation was successful,
-1 otherwise.
.PP
\fIOFFCreateObj\fP allocates and initializes an \fIOFFObjDesc\fP structure
A pointer to the newly-created structure is returned.  The null pointer is
returned if the operation was unsuccessful.
.PP
\fIOFFDestroyObj\fP deallocates all memory resources associated with
the object pointed at by \fIObj\fP.  It works by calling
\fIOFFFreeProperty\fP for each property in the property list for
the specified object.
.PP
\fIOFFAddProperty\fP adds a property structure to the property list
associated with the object pointed at by \fIObj\fP, initializes it,
and returns a pointer to it.
The null pointer is returned if the operation was unsuccessful.
.PP
\fIOFFRemoveProperty\fP deletes the named property 
from the object pointed at by \fIObj\fP.  This routine returns
-1 if the named property is not found in the property list for the
specified object.
.PP
\fIOFFFreeProperty\fP frees all the memory resources allocated to the
property structure specified by \fIProperty\fP as well as the property
structure itself.  This routine will not typically be called directly
by applications.
.NH 1
Object Search Path
.PP
It is important to avoid embedding path names in object files.
When an object is transported to another system, chances are slim that the same
directory structure will exist.  The \fIOFFReadObj\fP routine in
libobj.a knows about an environment variable named \fIOBJ_PATH\fP
that is used to overcome this problem.
.PP
When an object is read, an attempt is first made to open it in the
current working directory.  If that attempt fails, the directories
specified in the \fIOBJ_PATH\fP variable are tried in turn until
the file is successfully opened or the directory list is exhausted.
.PP
The name of the directory where a successful open operation occurred
is used for opening associated data files as well.  This means that
all of the data files for a particular object must reside in the same
directory.
.PP
It is hoped that in this way, users will be able to draw on one or
more collections of "standard" objects in addition to their own private
collections of objects.
.bp
.sp 6
.NH 1
Appendix A: Conventions for Polygonal Objects
.PP
This list contains the conventions we have adopted for describing
3D polygonal objects which are defined in some three-dimensional
model coordinate system.  Items in regular type are string
literal, printed as they would appear in an OFF file, and
item in italics are indicate data values that will vary from
object to object.
.PP
.TS
box;
c|c|c|c|c|c
l|l|l|l|l|l.
Property	Type	Format	Defaults	ASCII filename	Binary Filename
=
name	*****	*****	\fIobjname\fP	*****	*****
author	*****	*****	\fIauthor\fP	*****	*****
description	*****	*****	\fIdescription\fP	*****	*****
copyright	*****	*****	\fIcopyright\fP	*****	*****
type	*****	*****	polyline	*****	*****
	*****	*****	polygon	*****	*****
geometry	indexed_poly	fff	*****	\fIname\fP.geom	\fIname\fP.bgeom
polygon_colors	generic	fff	*****	\fIname\fP.pcol	\fIname\fP.bpcol
vertex_colors	generic	fff	*****	\fIname\fP.vcol	\fIname\fP.bvcol
back_faces	default	s	cull	*****	*****	
			display	*****	*****	
			reverse	*****	*****	
vertex_order	default	s	clockwise	*****	*****	
			counter-clockwise	*****	*****	
normals	indexed_poly	fff	*****	\fIname\fP.norm	\fIname\fP.bnorm
diffuse_coef	default	f	\fIvalue\fP	*****	*****
specular_coef	default	f	\fIvalue\fP	*****	*****
specular_power	default	f	\fIvalue\fP	*****	*****
bounding_box	default	ffffff	\fIvalue\fP	*****	*****
.TE
.bp
.NH 1
Appendix B: OFF Header File For a Cube (cube.off)
.PP
.sp 6
.TS
;
l l.
name	cube
author	Randi J. Rost
description	cube with sides of red, green, blue, cyan, yellow, magenta
copyright	public domain
type	polygon
.TE
.TS
;
l c c c
l c c c
l l l l.
# Prop.	data type	format	filename or default data
#_______	_________	______	________________________
.sp 1
geometry	indexed_poly	fff	cube.geom
vertex_order	default	s	clockwise
polygon_colors	generic	fff	cube.pcol
back_faces	default	s	cull
.TE
.bp
.sp 6
.NH 1
Appendix C: Listing of cube.geom
.PP
.sp 6
.TS
;
nw(0.5i) nw(0.5i) nw(0.5i) nw(0.5i) nw(0.5i).
8	6	24
-1.0	-1.0	1.0
-1.0	1.0	1.0
1.0	1.0	1.0
1.0	-1.0	1.0
-1.0	-1.0	-1.0
-1.0	1.0	-1.0
1.0	1.0	-1.0
1.0	-1.0	-1.0
4	1	2	3	4
4	5	6	2	1
4	3	2	6	7
4	3	7	8	4
4	1	4	8	5
4	8	7	6	5
.TE
.bp
.sp 6
.NH 1
Appendix D: Listing of cube.pcol
.PP
.sp 6
.TS
;
l s s
nw(0.5i) nw(0.5i) nw(0.5i).
6		
1.0        0.0        0.0
0.0        1.0        0.0
0.0        0.0        1.0
0.0        1.0        1.0
1.0        1.0        0.0
1.0        0.0        1.0
.TE
.bp
.sp 6
.NH 1
Appendix E: Listing of off.h
.LP
.sp 2
.nf
\f8#define OFF_INDEXED_POLY_MAGIC  0xFEEDFEED
#define OFF_GENERIC_MAGIC       0xBEEFBEEF

#define OFF_BIGSTR              256
#define OFF_SMSTR               40

#define	OFF_ASCII               0
#define	OFF_BINARY              1


/* Types of data for object properties  */

#define OFF_UNKNOWN_TYPE_DATA   0
#define OFF_STANDARD_DATA       1
#define OFF_COMMENT_DATA        2
#define OFF_DEFAULT_DATA        3
#define OFF_GENERIC_DATA        4
#define OFF_INDEXED_POLY_DATA   5


typedef struct _OFFProp
    {
    char        PropName[OFF_SMSTR];     /* Name of property (or attribute)  */
    int         PropType;                /* Type of data for property        */
    char        PropFileName[OFF_BIGSTR];/* Name of file that has prop data  */
    char        DataFormat[OFF_SMSTR];   /* Pointer to property data format  */
    int         PropCount;               /* Number of data items for property*/
    char        *PropData;               /* Pointer to property data         */
    struct _OFFProp *NextProp;           /* Pointer to next property in list */
    } OFFProperty;

typedef struct
    { 
    OFFProperty *FirstProp;              /* Pointer to first property in list*/
    } OFFObjDesc;\fP

.fi
.bp
.sp 6
.NH 1
Appendix F: Data Structure Format
.PP
The following diagram depicts some of the data structures for the
object \fIcube.off\fP after being read by \fIOFFReadObj()\fP (or just prior
to being written by \fIOFFWriteObj()\fP).
.PS
.ps 8
move to 0.0, 14.5
Object:
	[
	boxht = 0.3i; boxwid = 2.0i
	moveht = 0.3i; movewid = 0.0i
	   down
	A: box "FirstProp"
	   move up 0.3i; move right 2.0i
	   down
	B: box
	]

move to 1.0, 13.0
Prop1:
	[
	boxht = 0.3i; boxwid = 1.5i
	moveht = 0.3i; movewid = 0.0i
	   down
	A: box "PropName"
	   down
	B: box "PropType"
	   down
	C: box "PropFileName"
	   down
	D: box "DataFormat"
	   down
	E: box "PropCount"
	   down
	F: box "PropData"
	   down
	H: box "NextProp"
	   move up 2.1i;
	   move right 2.25i;
	   boxht = 0.3i; boxwid = 3.0i
	   moveht = 0.3i; movewid = 0.0i
	   down
	I: box "name"
	   down
	J: box "OFF_STANDARD_DATA"
	   down
	K: box "\fInull string\fP"
	   down
	L: box "\fInull string\fP"
	   down
	M: box "0"
	   down
	N: box
	   down
	O: box
	]

move to 1.0, 10.0
Prop2:
	[
	boxht = 0.3i; boxwid = 1.5i
	moveht = 0.3i; movewid = 0.0i
	   down
	A: box "PropName"
	   down
	B: box "PropType"
	   down
	C: box "PropFileName"
	   down
	D: box "DataFormat"
	   down
	E: box "PropCount"
	   down
	F: box "PropData"
	   down
	H: box "NextProp"
	   move up 2.1i;
	   move right 2.25i;
	   boxht = 0.3i; boxwid = 3.0i
	   moveht = 0.3i; movewid = 0.0i
	   down
	I: box "author"
	   down
	J: box "OFF_STANDARD_DATA"
	   down
	K: box "\fInull string\fP"
	   down
	L: box "\fInull string\fP"
	   down
	M: box "0"
	   down
	N: box
	   down
	O: box
	]

move to 1.0, 7.0
Prop3:
	[
	boxht = 0.3i; boxwid = 1.5i
	moveht = 0.3i; movewid = 0.0i
	   down
	A: box "PropName"
	   down
	B: box "PropType"
	   down
	C: box "PropFileName"
	   down
	D: box "DataFormat"
	   down
	E: box "PropCount"
	   down
	F: box "PropData"
	   down
	H: box "NextProp"
	   move up 2.1i;
	   move right 2.25i;
	   boxht = 0.3i; boxwid = 3.0i
	   moveht = 0.3i; movewid = 0.0i
	   down
	I: box "geometry"
	   down
	J: box "OFF_INDEXED_POLY_DATA"
	   down
	K: box "cube.geom"
	   down
	L: box "fff"
	   down
	M: box "6"
	   down
	N: box
	   down
	O: box
	]

move to 1.0, 4.0
Prop4:
	[
	boxht = 0.3i; boxwid = 1.5i
	moveht = 0.3i; movewid = 0.0i
	   down
	A: box "PropName"
	   down
	B: box "PropType"
	   down
	C: box "PropFileName"
	   down
	D: box "DataFormat"
	   down
	E: box "PropCount"
	   down
	F: box "PropData"
	   down
	H: box "NextProp"
	   move up 2.1i;
	   move right 2.25i;
	   boxht = 0.3i; boxwid = 3.0i
	   moveht = 0.3i; movewid = 0.0i
	   down
	I: box "polygon_colors"
	   down
	J: box "OFF_GENERIC_DATA"
	   down
	K: box "cube.pcol"
	   down
	L: box "fff"
	   down
	M: box "6"
	   down
	N: box
	   down
	O: box
	]

move to 1.0, 1.0
Prop5:
	[
	boxht = 0.3i; boxwid = 1.5i
	moveht = 0.3i; movewid = 0.0i
	   down
	A: box "PropName"
	   down
	B: box "PropType"
	   down
	C: box "PropFileName"
	   down
	D: box "DataFormat"
	   down
	E: box "PropCount"
	   down
	F: box "PropData"
	   down
	H: box "NextProp"
	   move up 2.1i;
	   move right 2.25i;
	   boxht = 0.3i; boxwid = 3.0i
	   moveht = 0.3i; movewid = 0.0i
	   down
	I: box "back_faces"
	   down
	J: box "OFF_DEFAULT_DATA"
	   down
	K: box "\fInull string\fP"
	   down
	L: box "s"
	   down
	M: box "0"
	   down
	N: box
	   down
	O: box "\fInull pointer\fP"
	]

line from Prop1.O.c to Prop1.O.c.x, (Prop1.s.y + Prop2.n.y) / 2.0
line to 0.5, (Prop1.s.y + Prop2.n.y) / 2.0
line to 0.5, Prop2.A.c.y
arrow to Prop2.A.w

line from Prop2.O.c to Prop2.O.c.x, (Prop2.s.y + Prop3.n.y) / 2.0
line to 0.5, (Prop2.s.y + Prop3.n.y) / 2.0
line dashed to 0.5, Prop3.A.c.y
arrow to Prop3.A.w

line from Prop3.O.c to Prop3.O.c.x, (Prop3.s.y + Prop4.n.y) / 2.0
line to 0.5, (Prop3.s.y + Prop4.n.y) / 2.0
line dashed to 0.5, Prop4.A.c.y
arrow to Prop4.A.w

line from Prop4.O.c to Prop4.O.c.x, (Prop4.s.y + Prop5.n.y) / 2.0
line to 0.5, (Prop4.s.y + Prop5.n.y) / 2.0
line to 0.5, Prop5.A.c.y
arrow to Prop5.A.w

line from Object.B.c to Object.B.c.x, (Object.s.y + Prop1.n.y) / 2.0
line to 0.5, (Object.s.y + Prop1.n.y) / 2.0
line to 0.5, Prop1.A.c.y
arrow to Prop1.A.w

boxht = 0.3i; boxwid = 2.0i
P1: box "cube" at Prop1.N.e.x + 1.5i, Prop1.N.e.y
arrow from Prop1.N.c to P1.w

P2: box "Randi J. Rost" at Prop2.N.e.x + 1.5i, Prop2.N.e.y
arrow from Prop2.N.c to P2.w

P3: box "  8    6    24 -1.0 -1.0" at Prop3.N.e.x + 1.5i, Prop3.N.e.y
P4: box " 1.0 -1.0  1.0  1.0  1.0" with .nw at P3.sw
P5: box " 1.0  1.0  1.0 -1.0  1.0" with .nw at P4.sw
P6: box "-1.0 -1.0 -1.0 -1.0  1.0" with .nw at P5.sw
P7: box "-1.0  1.0  1.0 -1.0  1.0" with .nw at P6.sw
P8: box "-1.0 -1.0 4  4  4 4  4  4" with .nw at P7.sw
P9: box "1  2  3  4  5  6  2  1" with .nw at P8.sw
Pa: box "3  2  6  7  3  7  8  4" with .nw at P9.sw
Pb: box "1  4  8  5  8  7  6  5" with .nw at Pa.sw
arrow from Prop3.N.c to P3.w

Pc: box "6    1.0  0.0  0.0  0.0" at Prop4.N.e.x + 1.5i, Prop4.N.e.y
Pd: box "1.0  0.0  0.0  0.0  1.0" with .nw at Pc.sw
Pe: box "0.0  1.0  1.0  1.0  1.0" with .nw at Pd.sw
Pf: box "0.0  1.0  0.0  1.0     " with .nw at Pe.sw
arrow from Prop4.N.c to Pc.w

boxht = 0.3i; boxwid = 0.5i
Pg: box at Prop5.N.e.x + 1.0i, Prop5.N.e.y
Ph: box "cull" at Pg.e.x + 1.0i, Prop5.N.e.y
arrow from Pg.c to Ph.w
arrow from Prop5.N.c to Pg.w

"Object" at Object.A.n above
.ps 12
.PE
.bp
.NH 1
Acknowledgements
.PP
Special thanks to Allen Akin of WSE for helpful ideas and suggestions.  
Thanks also to Jeff Friedberg of Digital's High-Performance Workstation (HPWS)
group and Shaun Ho of WSE who also contributed to the design.
