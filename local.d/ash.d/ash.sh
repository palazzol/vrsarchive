: /bin/sh
#
# Configurable stuff.
#
HELP=/usr/local/lib/ash.help
INST=/usr/local/lib/ash.inst
HAS=/usr/local/lib/ash.has
PAGE=/usr/ucb/more
E2="\c"
FMT="pr -t -5"
#
OPATH=$PATH
export PATH

trap 'echo Ouch!' 2 3
cd ${HOME}

# Limbo is where destroyed objects live
LIM=.limbo
mkdir ${LIM} >&- 2>&-

# Make our knapsack.
KNAP=.knapsack
if [ ! -d $KNAP ] ; then
  if mkdir $KNAP >&- 2>&-; then
    echo  "You find a discarded empty knapsack."
  else
    echo "You have no knapsack to carry things in."
    exit 1
  fi
else
  echo "One moment while I peek into your old knapsack."
fi

kn=`echo \`ls -a $KNAP | sed -e '/^\.$/d' -e '/^\.\.$/d'\``

echo ${E1} "Welcome to the Adventure shell!  Do you need instructions?" ${E2}
read X
if [ $X != "no" ] ; then
  $PAGE <$INST
  echo ${E1} "Type a RETURN to continue:" ${E2}
  read X
fi

wiz=false
cha=false
prev=$LIM

while : ; do
  room=`pwd`
  if [ $room != $prev ] ; then
    if [ $room = $HOME ] ; then
      echo ${E1} "You are in your own home.  " ${E2}
    else
      echo ${E1} "You have entered $room.  " ${E2}
    fi

    # Collect contents of room.
    exs= obs= hexs= hobs= f=false
    for I in `ls -a` ; do
      case $I in
	.|..)
	  ;;
	.*)
	  if [ -f $I ] ; then
	    hobs="$hobs $I"
	  elif [ -d $I ] ; then
	    hexs="$hexs $I"
	  else
	    f=true
	  fi
	  ;;
	*)
	  if [ -f $I ] ; then
	    obs="$obs $I"
	  elif [ -d $I ] ; then
	    exs="$exs $I"
	  else
	    f=true
	  fi
	  ;;
      esac
    done

    #  Print report of what we found.
    if [ "$obs" ] ; then
      echo "This room contains:"
      echo $obs | tr ' ' '\012' | ${FMT}
    else
      echo "The room looks empty."
    fi
    if [ "$exs" ] ; then
      echo "There are exits labeled:"
      echo $exs | tr ' ' '\012' | ${FMT}
      echo "as well as a passage overhead."
    else
      echo "There is a passage overhead."
    fi
    if $f ; then
      echo "There are shadowy figures in the corner."
    fi
    prev=$room
  fi

  echo ${E1} "->  " ${E2}
  read verb obj x
  if [ $? != 0 ] ; then
    verb=quit
  fi

  case $verb in
    change)
      if [ "$obj" ] ; then
	if $HAS " $obs $hobs " " $obj " ; then
	  if [ "$x" ] ; then
	    set $x
	  else
	    set ''
	  fi
	  case "$1" in
	    to)
	      if [ "$2" ] ; then
		if [ -f $2 ] ; then
		  echo "You must destroy the $2 first."
		  set ''
		fi
		if [ "$2" ] ; then
		  if mv $obj $2 >&- 2>&- ; then
		    echo "The $obj shimmers and turns i to $2."
		    obs=`echo $2 $obs '' | sed -e "s/ $obj / /" -e 's/ $//'`
		  else
		    echo "You see a cloud of smoke but the $obj is unchanged."
		  fi
		fi
	      else
		echo 'To what?'
	      fi
	      ;;
	    *)
	      echo "Change $obj to what?"
	      ;;
	  esac
	elif $HAS " $kn " " $obj " ; then
	  echo "You must drop it first."
	else
	  echo "I see no $obj here."
	fi
      else
	echo "Change what?"
      fi
      ;;

    clone)
      if [ "$obj" ] ; then
	if $HAS " $obs $hobs " " $obj " ; then
	  if [ ! -r $obj ] ; then
	    echo "The $obj does not wish to be cloned."
	  else
	    if [ "$x" ] ; then
	      set $x
	    else
	      set ''
	    fi
	    case "$1" in
	      as)
		if [ "$2" ] ; then
		  if [ -f $2 ] ; then
		    echo "You must destroy $2 first."
		  elif cp $obj $2 >&- 2>&- ; then
		    echo "Poof!  When the smoke clears, you see the new $2."
		    obs="$obs $2"
		  else
		    echo "You hear a dull thud but no clone appears."
		  fi
		else
		  echo "As what?"
		fi
		;;
	      *)
		echo "Cone $obj as what?"
		;;
	    esac
	  fi
	elif $HAS " $kn " " $obj " ; then
	  echo "You must drop it first."
	else
	  echo "I see no $obj here."
	fi
      else
	echo "Clone what?"
      fi
      ;;

    drop)
      if [ "$obj" ] ; then
	for I in $obj $x ; do
	  if $HAS " $kn " " $I " ; then
	    if [ -w $I ] ; then
	      echo "You must destory $I first."
	    elif mv $HOME/$KNAP/$I $I >&- 2>&- ; then
	      echo "$I: dropped."
	      kn=`echo '' $kn '' | sed -e "s/ $I / /" -e 's/^ //' -e 's/  $//'`
	      obs=`echo $I $obs`
	    else
	      echo "The $I is caught in your knapsack."
	    fi
	  else
	    echo "You're not carrying the $I!"
	  fi
	done
      else
	echo "Drop what?"
      fi
      ;;

    enter|go)
      if [ "$obj" ] ; then
	if [ $obj != up ] ; then
	  if $HAS " $exs $hexs " " $obj " ; then
	    if cd $obj ; then
	      echo "You squeeze through the passage."
	    else
	      echo "An invisible force blocks your way."
	    fi
	  else
	    echo "I see no such pssage."
	  fi
	elif cd .. ; then
	  echo "You struggle upwards."
	else
	  echo "You can't reach that high."
	fi
      else
	echo "Which passage?"
      fi
      ;;

    examine)
      if [ "$obj" ] ; then
	if [ $obj = all ] ; then
	  obj=`echo $obs $exs`
	  x=
	fi
	for I in $obj $x ; do
	  if $HAS " $obs $hobs $exs $hexs " " $I " ; then
	    echo "Upon close inspection of the $I, you see:"
	    if ls -ld $I 2>&- ; then
	      :
	    else
	      echo "-- when you look directly at the $I, it vanishes."
	    fi
	  elif $HAS " $kn " " $I " ; then
	    echo "You must drop it first."
	  else
	    echo "I see no $I here."
	  fi
	done
      else
	echo "Examine what?"
      fi
      ;;

    feed)
      if [ "$obj" ] ; then
	if $HAS " $obs $hobs " " $obj " ; then
	  if [ "$x" ] ; then
	    set $x
	  else
	    set ''
	  fi
	  case "$1" in
	    to)
	      if [ "$2" ] ; then
		shift
		if env PATH=$OPATH $* <$obj 2>&- ; then
		  echo "The $1 monster devours your $obj."
		  if rm -f $obj >&- 2>&1 ; then
		    obs=`echo '' $obs '' |
			      sed -e "s/ $obj / /" -e 's/^ //' -e 's/ $//'`
		  else
		    echo "But he spits it back up."
		  fi
		else
		  echo "The $1 monster holds his nose in disdain."
		fi
	      else
		echo "To what?"
	      fi
	      ;;
	    *)
	      echo "Feed $obj to what?"
	      ;;
	  esac
	elif $HAS " $kn " " $obj " ; then
	  echo "You must drop it first."
	else
	  echo "I see no $obj here."
	fi
      else
	echo "Feed what?"
      fi
      ;;

    get|take)
      if [ "$obj" ] ; then
	if [ $obj = all ] ; then
	  obj="$obs"
	  x=
	fi
	for I in $obj $x ; do
	  if $HAS " $obs $hobs " " $I " ; then
	    if $HAS " $kn " | fgrep " $I " ; then
	      echo "You already have one."
	    elif mv $I $HOME/$KNAP/$I >&- 2>&- ; then
	      echo "$I: taken"
	      kn="$I $kn"
	      obs=`echo '' $obs '' | sed -e "s/ $I / /" -e 's/^ //' -e 's/ $//'`
	    else
	      echo "The $I is too heavy to take."
	    fi
	  else
	    echo "I see no $I here."
	  fi
	done
      else
	echo "Take what?"
      fi
      ;;

    help)
      $PAGE <$HELP
      ;;

    inven|inventory|i)
      if [ "$kn" ] ; then
	echo "Your knapsack contains:"
	echo $kn | tr ' ' '\012' | ${FMT}
      else
	echo "You are poverty-stricken."
      fi
      ;;

    kill|destory)
      if [ "$obj" ] ; then
	if [ $obj = all ] ; then
	  x=
	  obj=
	  echo "To dangerous to $verb them all at once!"
	fi
	for I in $obj $x ; do
	  if $HAS " $obs $hobs " " $I " ; then
	    if mv $I $HOME/$LIM/$I <&- >&- 2>&- ; then
	      if [ $verb = kill ] ; then
		echo "The $I cannot defend itself; it dies."
	      else
		echo "You have destroyed the $I; it vanishes."
	      fi
	    elif [ $verb = kill ] ; then
	      echo "Your feeble blows are no match for the $I."
	    else
	      echo "The $I is indestructible."
	    fi
	  elif $HAS " $kn " " $I " ; then
	    echo "You must drop it first."
	  else
	    echo "I see no $I here."
	  fi
	done
      else
	echo "Kill what?"
      fi
      ;;

    look|l)
      obs=`echo $obs $hobs`
      hobs=
      if [ "$obs" ] ; then
	echo "The room contains:"
	echo $obs | tr ' ' '\012' | ${FMT}
      else
	echo "The room is empty."
      fi
      exs=`echo $exs $hexs`
      hexs=
      if [ "$exs" ] ; then
	echo "There are exits plainly labelled."
	echo $exs | tr ' ' '\012' | ${FMT}
	echo "... and a passage overhead."
      else
	echo "The only exit is directly overhead."
      fi
      ;;

    magic)
      if [ "$obj" = mode ] ; then
	if $cha ; then
	  echo "You had your chance and you blew it."
	else
	  echo ${E1} "Are you a wizard?  " ${E2}
	  read X
	  if [ "$X" = "yes" ] ; then
	    echo "Prove it!  Say the magic word:  "
	    read X
	    if [ "$X" = "armadillo" ] ; then
	      echo "Yes, oh mighty master....  I live to serve."
	      wiz=true
	    else
	      echo "Bah, you are nothing but a charlatan."
	      cha=true
	    fi
	  else
	    echo "I didn't think so."
	  fi
	fi
      else
	echo "Nice try."
      fi
      ;;

    open|read)
      if [ "$obj" ] ; then
	if $HAS " $obs $hobs " " $obj " ; then
	  if [ -r $obj ] ; then
	    if [ -s $obj ] ; then
	      echo "Opening the $obj reveals:"
	      $PAGE <$obj 2>&-
	      if [ $? != 0 ] ; then
		echo "-- oops, you lost the contents!"
	      fi
	    else
	      echo "There is nothing inside the $obj."
	    fi
	  else
	    echo "You do not have the proper tools to open the $obj."
	  fi
	elif $HAS " $kn " " $obj " ; then
	  echo "You must drop it first."
	else
	  echo "I see no $obj here."
	fi
      else
	echo "Open what?"
      fi
      ;;

    quit|exit)
      echo ${E1} "Do you really want to quit now?" ${E2}
      read X
      if [ $X = "yes" ] ; then
	if [ "$kn" ] ; then
	  echo "The contents of your knapsack will still be there next time."
	fi
	rm -rf $HOME/$LIM
	echo "See you later!"
	exit 0
      fi
      ;;

    resurrect)
      if [ "$obj" ] ; then
	for I in $obj $x ; do
	  if $HAS " $obs $hobs " " $I " ; then
	    echo "The $I is already alive and well."
	  elif mv $HOME/$LIM/$I $I <&- >&- 2>&- ; then
	    echo "The $I staggers to its feet"
	    obs=`echo $I $obs`
	  else
	    echo "You see sparks, but no $I appears."
	  fi
	done
      else
	echo "Resurrect what?"
      fi
      ;;

    steal)
      if [ "$obj" ] ; then
	if $HAS " $obs $hobs " " $obj " ; then
	  echo "There is already one here."
	else
	  if [ "$x" ] ; then
	    set $x
	  else
	    set ''
	  fi
	  case "$1" in
	    from)
	      if [ "$2" ] ; then
		if env PATH=$OPATH $* >$obj 2>&- ; then
		  echo "The $1 monster drops the $obj."
		  obs=`echo $obj $obs`
		else
		  echo "The $1 monster runs away as you approach."
		  rm -f $obj >&- 2>&-
		fi
	      else
		echo "From what?"
	      fi
	      ;;
	    *)
	      echo "Steal $obj from what?"
	      ;;
	  esac
	fi
      else
	echo "Steal what?"
      fi
      ;;

    throw|toss)
      if [ "$obj" ] ; then
	if $HAS " $obs $hobs " " $obj " ; then
	  if [ "$x" ] ; then
	    set $x
	  else
	    set ''
	  fi
	  case "$1" in
	    at)
	      case "$2" in
		daemon)
		  if lpr -r $obj ; then
		    echo "The daemon catches the $obj, turns it into paper,"
		    echo "and leaves it in the basket."
		    obs=`echo '' $objs ''
			  | sed -e "s/ $obj / /" -e 's/^ //' -e 's/ $//'`
		  else
		    echo "The daemon is nowhere to be found."
		  fi
		  ;;
		*)
		  echo "At what?"
		  ;;
	      esac
	      ;;
	    *)
	      echo "Throw $obj at what?"
	      ;;
	  esac
	elif $HAS " $kn " " $obj " ; then
	  echo "It is in your knapsack."
	else
	  echo "I see no $obj here."
	fi
      else
	echo "Throw what?"
      fi
      ;;

    u|up)
      if cd .. ; then
	echo "You pull yourself up to a new level."
      else
	echo "You can't reach that high!"
      fi
      ;;

    wake)
      if [ "$obj" ] ; then
	echo "You awaken the $obj monster:"
	env PATH=$OPATH $obj $x
	echo "The monster slithers back into the darkness."
      else
	echo "Wake what?"
      fi
      ;;

    w|where)
      echo "You are in $room."
      ;;

    xyzzy)
      if cd ; then
	echo "A strange feeling comes over you."
      else
	echo "Your spell fizzles out."
      fi
      ;;

    *)
      if [ "$verb" ] ; then
	if $wiz ; then
	  env PATH=$OPATH $verb $obj $x
	else
	  echo "I don't know how to \"$verb\"."
	  echo 'Type "help" for assistance.'
	fi
      fi
      ;;
  esac
done
