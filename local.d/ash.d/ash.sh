:
:	'ash -- "Adventure shell"'
:	'Edit Hitory:	83/11/14	D A Gwyn'
:	'           	84/03/17	V. Slyngstad'
:	'SCCS ID:	@(#)ash.sh	1.2'
:
PAGE=more
HELP=/usr/local/lib/ash.help
INST=/usr/local/lib/ash.inst
MAINT=root

trap 'echo Ouch!' 2 3

cd
LIM=.limbo			: $HOME/$LIM contains "destroyed" objects
mkdir $LIM >&- 2>&-
KNAP=.knapsack			: $HOME/$KNAP contains objects being "carried"
if [ ! -d $KNAP ]; then
  mkdir $KNAP >&- 2>&-
  if [ $? = 0 ]; then
    echo 'You found a discarded empty knapsack.'
  else
    echo 'You have no knapsack to carry things in.'
    exit 1
  fi
else
  echo 'One moment while I peek in your old knapsack...'
fi

kn=`echo \`ls -a $KNAP | sed -e '/^\.$/d' -e '/^\.\.$/d'\``

echo -n 'Welcome to the Adventure shell!  Do you need instructions?'
read reply
case "$reply" in
  y*|Y*)
    $PAGE < $INST ;;
esac

wiz=false
cha=false
prev=$LIM
while true; do
  room=`pwd`
  if [ $room != $prev ]; then
    if [ $room = $HOME ]; then
      echo 'You are in your own home.'
    else
      echo "You have entered $room."
    fi
    exs=
    obs=
    hexs=
    hobs=
    f=false
    for i in `ls -a`; do
      case $i in
        .|..) ;;
        .*)   if [ -f $i ]; then
                hobs="$hobs $i"
              elif [ -d $i ]; then
                hexs="$hexs $i"
              else
                f=true
              fi ;;
        *)    if [ -f $i ]; then
                obs="$obs $i"
              elif [ -d $i ]; then
                exs="$exs $i"
              else
                f=true
              fi ;;
      esac
    done
    if [ "$obs" ]; then
      echo 'This room contains:'
      echo $obs | tr ' ' '\012' | pr -5 -t -l1
    else
      echo 'The room looks empty.'
    fi
    if [ "$exs" ]; then
      echo 'There are exits labeled:'
      echo $exs | tr ' ' '\012' | pr -5 -t -l1
      echo 'as well as a passage overhead.'
    else
      echo 'There is a passage overhead.'
    fi
    if sh -c $f; then
      echo 'There are shadowy figures in the corner.'
    fi
    prev=$room
  fi

  echo -n '->';			: prompt
  read verb obj x
  if [ $? != 0 ]; then
    verb=quit		: EOF
  fi
      
  case $verb in
    change)
      if [ "$obj" ]; then
        echo " $obs $hobs " | fgrep " $obj " >&- 2>&-
        if [ $? = 0 ]; then
          set $x
          case "$1" in
            to) if [ "$2" ]; then
                  if [ -f $2 ]; then
                    echo "You must destroy $2 first."
                  else
                    if mv $obj $2 >&- 2>&-; then
                      echo "The $obj shimmers and turns into $2."
                      obs=`echo $2 $obs '' | sed -e "s/ $obj / /" -e 's/ $//'`
                    else
                      echo There is a cloud of smoke but the $obj is unchanged.
                    fi
                  fi
                else
                  echo 'To what?'
                fi ;;
            *)  echo "Change $obj to what?" ;;
          esac
        else
          echo " $kn " | fgrep " $obj " >&- 2>&-
          if [ $? = 0 ]; then
            echo 'You must drop it first.'
          else
            echo "I see no $obj here."
          fi
        fi
      else
        echo 'Change what?'
      fi ;;
    clone)
      if [ "$obj" ]; then
        echo " $obs $hobs " | fgrep " $obj " >&- 2>&-
	if [ $? = 0 ]; then
	  if [ ! -r $obj ]; then
	    echo "The $obj does not wish to be cloned."
	  else
	    set $x
	    case "$1" in
	      as) if [ "$2" ]; then
	            if [ -f $2 ]; then
	              echo "You must destroy $2 first."
	            else
	              if cp $obj $2 >&- 2>&-; then
	                echo "Poof!  When the smoke clears, you see the new $2."
	                obs="$obs $2"
	              else
	                echo 'You hear a dull thud but no clone appears.'
	              fi
	            fi
	          else
	            echo 'As what?'
	          fi ;;
              *)  echo "Clone $obj as what?" ;;
            esac
          fi
        else
          echo " $kn " | fgrep " $obj " >&- 2>&-
          if [ $? = 0 ]; then
            echo 'You must drop it first.'
          else
            echo "I see no $obj here."
          fi
        fi
      else
        echo 'Clone what?'
      fi ;;
    drop)
      if [ "$obj" ]; then
        for it in $obj $x; do
          echo " $kn " | fgrep " $it " >&- 2>&-
          if [ $? = 0 ]; then
            if [ -w $it ]; then
              echo "You must destroy $it first."
            else
              if mv $HOME/$KNAP/$it $it >&- 2>&-; then
                echo "$it: dropped."
                kn=`echo '' $kn '' |sed -e "s/ $it / /" -e 's/^ //' -e 's/ $//'`
                obs=`echo $it $obs`
              else
                echo "The $it is caught in your knapsack."
              fi
            fi
          else
            echo "You're not carrying the $it!"
          fi
        done
      else
        echo 'Drop what?'
      fi ;;
    enter|go)
      if [ "$obj" ]; then
        if [ $obj != up ]; then
          echo " $exs $hexs " | fgrep " $obj " >&- 2>&-
          if [ $? = 0 ]; then
            if [ -r $obj ]; then
              if cd $obj; then
                echo 'You squeeze through the passage.'
              else
                echo "You can't go that direction."
              fi
            else
              echo 'An invisible force blocks your way.'
            fi
          else
            echo 'I see no such passage.'
          fi
        elif cd ..; then
          echo 'You struggle upwards.'
        else
          echo "You can't reach that high."
        fi
      else
        echo 'Which passage?'
      fi ;;
    examine)
      if [ "$obj" ]; then
        if [ $obj = all ]; then
          $obj=`echo $obs $exs`
          x=
        fi
        for it in $obj $x; do
          echo " $obs $hobs $exs $hexs " | fgrep " $it " >&- 2>&-
          if [ $? = 0 ]; then
            echo "Upon close inspection of the $it, you see:"
            ls -ld $it 2>&-
            if [ $? != 0 ]; then
              echo "-- when you look directly at the $it, it vanishes."
            fi
          else
            echo " $kn " | fgrep " $it " >&- 2>&-
            if [ $? = 0 ]; then
              echo 'You must drop it first.'
            else
              echo "I see no $it here."
            fi
          fi
        done
      else
        echo 'Examine what?'
      fi ;;
    feed)
      if [ "$obj" ]; then
        echo " $obs $hobs " | fgrep " $obj " >&- 2>&-
        if [ $? = 0 ]; then
          set $x
          case "$1" in
            to) if [ "$2" ]; then
                  shift
                  if $* <$obj 2>&-; then
                    echo "The $1 monster devours your $obj."
                    if rm -f $obj >&- 2>&-; then
                      obs=`echo '' $obs '' |\
                           sed -e "s/ $obj / /" -e 's/^ //' -e 's/ $//'`
                    else
                      echo 'But he spits it back up.'
                    fi
                  else
                    echo "The $1 monster holds his nose in disdain."
                  fi
                else
                  echo 'To what?'
                fi ;;
            *)  echo "Feed $obj to what?" ;;
          esac
        else
          echo " $kn " | fgrep " $obj " >&- 2>&-
          if [ $? = 0 ]; then
            echo 'You must drop it first.'
          else
            echo "I see no $obj here."
          fi
        fi
      else
        echo 'Feed what?'
      fi ;;
    get|take)
      if [ "$obj" ]; then
        if [ $obj = all ]; then
          obj="$obs"
          x=
        fi
        for it in $obj $x; do
          echo " $obs $hobs " | fgrep " $it " >&- 2>&-
          if [ $? = 0 ]; then
            echo " $kn " | fgrep " $it " >&- 2>&-
            if [ $? = 0 ]; then
              echo 'You already have one.'
            else
              if mv $it $HOME/$KNAP/$it >&- 2>&-; then
                echo "$it: taken."
                kn="$it $kn"
                obs=`echo '' $obs '' |\
                     sed -e "s/ $it / /" -e 's/^ //' -e 's/ $//'`
              else
                echo "The $it is too heavy."
              fi
            fi
          else
            echo "I see no $it here."
          fi
        done
      else
        echo 'Get what?'
      fi ;;
    gripe|bug)
      echo 'Please describe the problem and your situation at the time.'
      echo 'End the bug report with a line containing just a Ctrl-D.'
      (echo 'Subject: Adventure Shell';cat) | mail $MAINT
      echo 'Thank you!' ;;
    help)
      $PAGE < $HELP ;;
    inventory|i)
      if [ "$kn" ]; then
        echo 'Your knapsack contains:'
        echo $kn | tr ' ' '\012' | pr -5 -t -l1
      else
        echo 'You are poverty-stricken.'
      fi ;;
    kill|destroy)
      if [ "$obj" ]; then
        if [ $obj = all ]; then
          x=
          echo -n "Do you really want to attempt to $verb them all?"
          read reply
          case "$reply" in
            y*Y*)
              obj=`echo $obs` ;;
            *)
              echo 'Chicken!'
              obj= ;;
          esac
        fi
        for it in $obj $x; do
          echo " $obs $hobs " | fgrep " $it " >&- 2>&-
          if [ $? = 0 ]; then
            if mv $it $HOME/$LIM <&- >&- 2>&-; then
              if [ $verb = kill ]; then
                echo "The $it cannot defend himself; he dies."
              else
                echo "You have destroyed the $it; it vanishes."
              fi
              obs=`echo '' $obs '' |sed -e "s/ $it / /" -e 's/^ //' -e 's/ $//'`
            else
              if [ $verb = kill ]; then
                echo "Your feeble blows are no match for the $it."
              else
                echo "The $it is indestructible."
              fi
            fi
          else
            echo " $kn " | fgrep " $it " >&- 2>&-
            if [ $? = 0 ]; then
              echo "You must drop the $it first."
              found=false
            else
              echo "I see no $it here."
            fi
          fi
        done
      else
        echo 'Kill what?'
      fi ;;
    look|l)
      obs="$hobs $obs"
      hobs=
      if [ "$obs" ]; then
        echo 'The room contains:'
        echo $obs | tr ' ' '\012' | pr -5 -t -l1
      else
        echo 'The room is empty.'
      fi
      exs="$hexs $exs"
      hexs=
      if [ "$exs" ]; then
        echo 'There are exits plainly labeled:'
        echo $exs | tr ' ' '\012' | pr -5 -t -l1
        echo 'and a passage directly overhead.'
      else
        echo 'The only exit is directly overhead.'
      fi ;;
    magic)
      if [ "$obj" = mode ]; then
        if sh -c $cha; then
          echo 'You had your chance and you blew it.'
        else
          echo -n 'Are you a wizard?'
          read reply
          case "$reply" in
            y*|Y*)
              echo -n 'Prove it!  Say the magic word:'
              read reply
              if [ "$reply" = armadillo ]; then
                echo 'Yes, master!!'
                wiz=true
              else
                echo "Foo, you're nothing but a charlatan!"
                cha=true
              fi ;;
            *)
              echo "I didn't think so." ;;
          esac
        fi
      else
        echo 'Nice try.'
      fi ;;
    open|read)
      if [ "$obj" ]; then
        echo " $obs $hobs " | fgrep " $obj " >&- 2>&-
        if [ $? = 0 ]; then
          if [ -r $obj ]; then
            if [ -s $obj ]; then
              echo "Opening the $obj reveals:"
              $PAGE < $obj 2>&-
              if [ $? != 0 ]; then
                echo '-- oops, you lost the contents!'
              fi
            else
              echo "There is nothing inside the $obj."
            fi
          else
            echo "You do not have the proper tools to open the $obj."
          fi
        else
          echo " $kn " | fgrep " $obj " >&- 2>&-
          if [ $? = 0 ]; then
            echo 'You must drop it first.'
            found=false
          else
            echo "I see no $obj here."
          fi
        fi
      else
        echo 'Open what?'
      fi ;;
    quit|exit)
      echo -n 'Do you really want to quit now?'
      read reply
      case "$reply" in
        y*|Y*)
          if [ "$kn" ]; then
            echo 'The contents of your knapsack will still be there next time.'
          fi
          rm -rf $HOME/$LIM
          echo 'See you later!'
          exit 0 ;;
      esac ;;
    resurrect)
      if [ "$obj" ]; then
        for it in $obj $x; do
          echo " $obs $hobs " | fgrep " $it " >&- 2>&-
          if [ $? = 0 ]; then
            echo "The $it is already alive and well."
          else
            if mv $HOME/$LIM/$it $it <&- >&- 2>&-; then
              echo "The $it staggers to his feet."
              obs=`echo $it $obs`
            else
              echo "There are sparks but no $it appears."
            fi
          fi
        done
      else
        echo 'Resurrect what?'
      fi ;;
    steal)
      if [ "$obj" ]; then
        echo " $obs $hobs " | fgrep " $obj " >&- 2>&-
        if [ $? = 0 ]; then
          echo 'There is already one here.'
        else
          set $x
          case "$1" in
            from)
              if [ "$2" ]; then
                shift
                if $* >$obj 2>&-; then
                  echo "The $1 monster drops the $obj."
                  obs=`echo $obj $obs`
                else
                  echo "The $1 monster runs away as you approach."
                  rm -f $obj >&- 2>&-
                fi
              else
                echo 'From what?'
              fi ;;
            *)
              echo "Steal $obj from what?" ;;
          esac
        fi
      else
        echo 'Steal what?'
      fi ;;
    throw)
      if [ "$obj" ]; then
        echo " $obs $hobs " | fgrep " $obj " >&- 2>&-
        if [ $? = 0 ]; then
          set $x
          case "$1" in
            at)
              case "$2" in
                daemon)
                  if sh -c "lpr -r $obj"; then
                    echo "The daemon catches the $obj, turns it into paper,"
                    echo "and leaves it in the basket."
                  obs=`echo '' $obs '' |\
                       sed -e "s/ $obj / /" -e 's/^ //' -e 's/ $//'`
                  else
                    echo "The daemon is nowhere to be found."
                  fi ;;
                *)
                  echo 'At what?' ;;
              esac ;;
            *)
              echo "Throw $obj at what?" ;;
          esac
        else
          echo " $kn " | fgrep " $obj " >&- 2>&-
          if [ $? = 0 ]; then
            echo 'It is in your knapsack.'
            found=false
          else
            echo "I see no $obj here."
          fi
        fi
      else
        echo 'Throw what?'
      fi ;;
    u|up)
      if cd ..; then
        echo 'You pull yourself up a level.'
      else
        echo "You can't reach that high."
      fi ;;
    wake)
      if [ "$obj" ]; then
        echo "You awaken the $obj monster:"
        $obj $x
        echo 'The monster slithers back into the darkness.'
      else
        echo 'Wake what?'
      fi ;;
    w|where)
      echo "You are in $room." ;;
    xyzzy)
      if cd; then
        echo 'A strange feeling comes over you.'
      else
        echo 'Your spell fizzles out.'
      fi ;;
    *)
      if [ "$verb" ]; then
        if sh -c $wiz; then
          ${SHELL-sh} -c $verb $obj $x
        else
          echo "I don't know how to \"$verb\"."
          echo 'Type "help" for assistance.'
        fi
      else
        echo 'Say something!'
      fi ;;
  esac
done
