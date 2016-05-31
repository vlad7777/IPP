#/bin/bash -x

GUI_PATH=./sredniowiecze_gui_with_libs.sh

boardSize=10
turnsNumber=100
timeLimit=1
x1=
y1=
x2=
y2=
ai1=""
ai2=""

numberRegexp='^[0-9]+$'
twoNumbersRegexp='^[0-9]+,[0-9]+$'

while [ $# -gt 0 ]
do
	case $1 in
		-n) 
			shift
			if ! [[ $1 =~ $numberRegexp ]] || [[ $1 -lt 1 ]]; then
				exit 1
			fi
			boardSize=$1
			;;
		-k)
			shift
			if ! [[ $1 =~ $numberRegexp ]] || [[ $1 -lt 1 ]]; then
				exit 1
			fi
			turnsNumber=$1
			;;
		-s)
			shift
			if ! [[ $1 =~ $numberRegexp ]]; then
				exit 1
			fi
			timeLimit=$1
			;;
		-p1)
			shift
			if ! [[ $1 =~ $twoNumbersRegexp ]] ; then
				exit 1
			fi

			x1=$(echo $1 | cut -f1 -d,)
			y1=$(echo $1 | cut -f2 -d,)
			if [[ $x1 -lt 1 ]] || [[ $x1 -gt $boardSize ]] || [[ $y1 -lt 1 ]] || [[ $y1 -gt $boardSize ]]; then
				exit 1
			fi
			;;
		-p2)
			shift
			if ! [[ $1 =~ $twoNumbersRegexp ]] ; then
				exit 1
			fi

			x2=$(echo $1 | cut -f1 -d,)
			y2=$(echo $1 | cut -f2 -d,)
			if [[ $x2 -lt 1 ]] || [[ $x2 -gt $boardSize ]] || [[ $y2 -lt 1 ]] || [[ $y2 -gt $boardSize ]]; then
				exit 1
			fi
			;;
		-ai1)
			shift
			if ! [[ -f $1 ]]; then
				exit 1
			fi
			ai1=$1
			;;
		-ai2)
			shift
			if ! [[ -f $1 ]]; then
				exit 1
			fi
			ai2=$1
			;;
		-*)
			exit 1
			;;
	esac
	shift
done

if [[ "$x1" == "" && "$x2" == "" ]]; then
	x1=$(($RANDOM % $(($boardSize - 3)) + 1))
	y1=$(($RANDOM % $boardSize + 1))
	x2=$(($RANDOM % $(($boardSize - 3)) + 1))
	y2=$(($RANDOM % $boardSize + 1))
	while [[ $(($x1-$x2)) -lt 8 && $(($y1-$y2)) -lt 8 ]]; do
		x1=$(($RANDOM % $(($boardSize - 3)) + 1))
		y1=$(($RANDOM % $boardSize + 1))
		x2=$(($RANDOM % $(($boardSize - 3)) + 1))
		y2=$(($RANDOM % $boardSize + 1))
	done

elif [[ "$x1" == "" && "$x2" != "" ]]; then
	x1=$(($RANDOM % $(($boardSize - 3)) + 1))
	y1=$(($RANDOM % $boardSize + 1))
	while [[ $(($x1-$x2)) -lt 8 && $(($y1-$y2)) -lt 8 ]]; do
		x1=$(($RANDOM % $(($boardSize - 3)) + 1))
		y1=$(($RANDOM % $boardSize + 1))
	done

elif [[ "$x1" != "" && "$x2" == "" ]]; then
	x2=$(($RANDOM % $(($boardSize - 3)) + 1))
	y2=$(($RANDOM % $boardSize + 1))
	while [[ $(($x1-$x2)) -lt 8  && $(($x1-$x2)) -gt -8 ]] && [[ $(($y1-$y2)) -lt 8 && $(($y1-$y2)) -gt -8 ]]; do
		x2=$(($RANDOM % $(($boardSize - 3)) + 1))
		y2=$(($RANDOM % $boardSize + 1))
	done
else
	if [[ $(($x1-$x2)) -lt 8 && $(($x1-$x2)) -gt -8 ]] && [[ $(($y1-$y2)) -lt 8 && $(($y1-$y2)) -gt -8 ]]; then
		exit 1
	fi
fi

PIPE=$(mktemp -u)
mkfifo $PIPE
exec 3<>$PIPE
rm $PIPE

PIPE=$(mktemp -u)
mkfifo $PIPE
exec 4<>$PIPE
rm $PIPE

PIPE=$(mktemp -u)
mkfifo $PIPE
exec 5<>$PIPE
rm $PIPE

PIPE=$(mktemp -u)
mkfifo $PIPE
exec 6<>$PIPE
rm $PIPE

PIPE=$(mktemp -u)
mkfifo $PIPE
exec 7<>$PIPE
rm $PIPE

PIPE=$(mktemp -u)
mkfifo $PIPE
exec 8<>$PIPE
rm $PIPE

if [[ "$ai1" == "" && "$ai2" == "" ]]; then
	$GUI_PATH -human1 -human2 <&7 >&8 &
	pid_gui=$!

	echo "-human1 -human2" >> gui.args
	echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&7
	echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&7

	wait $pid_gui
	gui_ret=$?
	if [[ "$gui_ret" == "" ]]; then
		exit 1
	fi

	if [[ $gui_ret -ne 0 ]]; then
		exit 1
	fi

	exit 0 

elif [[ "$ai1" != "" && "$ai2" != "" ]]; then
	./$ai1 <&3 >&4 &
	pid1=$!

	echo pid1 $pid1

	./$ai2 <&5 >&6 2> /dev/null &
	pid2=$!

	echo pid2 $pid2

	$GUI_PATH <&7 >&8 &
	pid_gui=$!

	echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&7
	echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&7

	echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&3

	echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&5

	while true; do
		while (ps -p $pid1 > /dev/null) && (ps -p $pid2 > /dev/null) && read s <&4; do
			echo $s >&7
			echo $s >&5
			sleep $timeLimit
			if [[ "$s" == "END_TURN" ]]; then
				break
			fi
		done
		while (ps -p $pid1 > /dev/null) && (ps -p $pid2 > /dev/null) && read s <&6; do
			echo $s >&3
			echo $s >&7
			sleep $timeLimit
			if [[ "$s" == "END_TURN" ]]; then
				break
			fi
		done

		if !(ps -p $pid1 > /dev/null); then
			break
		fi 

		if !(ps -p $pid2 > /dev/null); then
			break
		fi 
	done

	wait $pid1
	ai1_ret=$?
	wait $pid2
	ai2_ret=$?
	wait $pid_gui
	gui_ret=$?

	if [[ "$ai1_ret" == "" ]]; then
		exit 1
	fi

	if [[ $ai1_ret -gt 2 ]]; then
		exit 1
	fi

	if [[ "$ai2_ret" == "" ]]; then
		exit 1
	fi

	if [[ $ai2_ret -gt 2 ]]; then
		exit 1
	fi
		
	if [[ "$gui_ret" == "" ]]; then
		exit 1
	fi
	
	if [[ $gui_ret -ne 0 ]]; then
		exit 1
	fi
	
	exit 0

elif [[ "$ai1" != "" && "$ai2" == "" ]]; then
	./$ai1 <&3 >&4 2> /dev/null &
	pid1=$!

	$GUI_PATH -human2 <&7 >&8 &
	pid_gui=$!

	echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&7
	echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&7

	echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&3

	while true; do
		while (ps -p $pid1 > /dev/null) && read s <&4; do
			echo $s >&7
			if [[ "$s" == "END_TURN" ]]; then
				break
			fi
		done
		while read s <&8; do
			echo $s >&3
			if [[ "$s" == "END_TURN" ]]; then
				break
			fi
		done

		if !(ps -p $pid1 > /dev/null); then
			break
		fi 
	done

	wait $pid1
	ai1_ret=$?
	wait $pid_gui
	gui_ret=$?
	if [[ "$ai_ret" == "" ]]; then
		exit 1
	fi

	if [[ $ai_ret -gt 2 ]]; then
		exit 1
	fi

	if [[ "$gui_ret" == "" ]]; then
		exit 1
	fi
	
	if [[ $gui_ret -ne 0 ]]; then
		exit 1
	fi
	
	exit 0

elif [[ "$ai1" == "" && "$ai2" != "" ]]; then
	./$ai2 <&5 >&6 2> /dev/null &
	pid2=$?

	$GUI_PATH -human1 <&7 >&8 &
	pid_gui=$?

	echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&7
	echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&7

	echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&5

	while true; do
		while (ps -p $pid2 > /dev/null) && read s <&8; do
			echo $s >&5
			if [[ "$s" == "END_TURN" ]]; then
				break
			fi
		done

		while read s <&6; do
			echo $s >&7
			if [[ "$s" == "END_TURN" ]]; then
				break
			fi
		done

		if !(ps -p $pid2 > /dev/null); then
			break
		fi 
	done

	wait $pid2
	ai2_ret=$?

	wait $pid_gui
	gui_ret=$?

	if [[ "$ai2_ret" == "" ]]; then
		exit 1
	fi

	if [[ $ai2_ret -gt 2 ]]; then
		exit 1
	fi

	if [[ "$gui_ret" == "" ]]; then
		exit 1
	fi
	
	if [[ $gui_ret -ne 0 ]]; then
		exit 1
	fi
	
	exit 0
fi
