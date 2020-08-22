Atari ST support has been provided by Holger Zessel.  He reports that
there is still a problem causing occasional crashing.  I believe this
is a small problem which we will have fixed soon.

You may wish to apply this small patch to starm2.c:
================Cut here================
***************
*** 215,221
  	  {
  	    char tstr[20];
  	    rastr(tstr, ras);
! 	    D_fontsize(10, HELV);
  	    D_text(xloc3+htext_xoff, yloc-htext_yoff, tstr, FALSE);
  	  }
        }

--- 215,221 -----
  	  {
  	    char tstr[20];
  	    rastr(tstr, ras);
! 	    D_fontsize(7, HELV);
  	    D_text(xloc3+htext_xoff, yloc-htext_yoff, tstr, FALSE);
  	  }
        }
***************
*** 246,252
  	    {
  	      char tstr[20];
  	      declstr(tstr, dl);
! 	      D_fontsize(10, HELV);
  	      D_text(xloc3+vtext_xoff, yloc3+vtext_yoff, tstr, FALSE);
  	    }
  	}

--- 246,252 -----
  	    {
  	      char tstr[20];
  	      declstr(tstr, dl);
! 	      D_fontsize(7, HELV);
  	      D_text(xloc3+vtext_xoff, yloc3+vtext_yoff, tstr, FALSE);
  	    }
  	}
================Cut here================

Holger's comments follow:

Some informations to the atari st driver:
-----------------------------------------
The atari driver uses gdos, supports monochrome only at this time
and is *very *unstable. This has something to do with the last changes
to starsupp.c and I had not the time to find the bugs yet.
I'll supply fixes as soon as I am able to do this. observe and dataconv
are ok in the atari version.

The atari special feature to commandline options is the driver option
-a d<num>   .
This tells the driver to open physical workstation <num>.
<num> must appear in assign.sys corresponding to an appropriate 
gdos driver. 31 has to be the handle for the metadevice driver.
starst writes metafiles but an option for the metafile name 
is still missing. For now it is a constant name "gemfile.gem".
If I read the metafile into easydraw the whole chart is upside down.
I don't know why this happens but it may depend on the NDC coordinates
(see below). An easy workaround is to select all, group all, and to mirror it
horizontally. After an ungroup everything can be edited as usual. This is
a big feature I think. Most other drivers are not able to offer an afterward
editing of charts.
  I use amcgdos and NDC coordinates (virtual [0..32767,0..32767]) for all
devices. There may be gdos versions which do not support NDC for screen
but I cannot prove this. I added a little assembler file with a function
telling whether gdos is loaded or not. This may be interesting for other
applications too. But it is MWCas dialect.
The hole stuff is compilable with MWC3.06. I didn't try other versions.

  I regret that it is so unstable but it was not one version before. I think
the use of gdos is a very flexible solution as even people with
laser printer are directly supported if they have a gdosdriver for their
printer. Some time ago an atari version was posted already but it supported
only color screens and furthermore screenhardcopies are not what I call
a finder chart. There are still many things to do with respect to the symbols
used to represent objects in sky. Object size is not yet optimal and
for the font size this the case too. Fonts are a drawback of gdos as
one is not able to be absolutely device independent.
The release of the atari st stuff is thought as expandable in future and if
people have interest in making extensions they should feel free to do this.
I suggest a second screen with an GEM interface...........

--Holger
----------------------------------------------------------------
Holger Zessel, uucp: ...!uunet!unido!uklirb!incas!zessel
	or	zessel@informatik.uni-kl.de
	or	zessel%informatik.uni-kl.de@relay.cs.net (from usa)
Fachbereich Informatik, SFB124-D1
Universitaet Kaiserslautern, West-Germany
