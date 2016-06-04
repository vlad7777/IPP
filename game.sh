#!/bin/bash

rm ai1.input
rm ai2.input

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
intlimit=2147483647

while [ $# -gt 0 ]
do
	case $1 in
		-n) 
			shift
			if ! [[ $1 =~ $numberRegexp ]] || [[ $1 -lt 1 ]] || [[ $1 -gt $intlimit ]]; then
				exit 1
			fi
			boardSize=$1
			;;
		-k)
			shift
			if ! [[ $1 =~ $numberRegexp ]] || [[ $1 -lt 1 ]] || [[ $1 -gt $intlimit ]]; then
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
			echo $1
			if ! [[ -f $1 ]]; then
				exit 1
			fi
			ai1=$1
			;;
		-ai2)
			shift
			echo $1
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

maxTries=100
tries=0

if [[ "$x1" == "" && "$x2" == "" ]]; then
	x1=$(($RANDOM % $(($boardSize - 3)) + 1))
	y1=$(($RANDOM % $boardSize + 1))
	x2=$(($RANDOM % $(($boardSize - 3)) + 1))
	y2=$(($RANDOM % $boardSize + 1))
	while [[ $(($x1-$x2)) -lt 8  && $(($x1-$x2)) -gt -8 ]] && [[ $(($y1-$y2)) -lt 8 && $(($y1-$y2)) -gt -8 ]] && [[ $tries -lt $maxTries ]]; do
		x1=$(($RANDOM % $(($boardSize - 3)) + 1))
		y1=$(($RANDOM % $boardSize + 1))
		x2=$(($RANDOM % $(($boardSize - 3)) + 1))
		y2=$(($RANDOM % $boardSize + 1))
		tries=$(($tries+1))
	done
	if [[ $(($x1-$x2)) -lt 8  && $(($x1-$x2)) -gt -8 ]] && [[ $(($y1-$y2)) -lt 8 && $(($y1-$y2)) -gt -8 ]]; then
		exit 1
	fi

elif [[ "$x1" == "" && "$x2" != "" ]]; then
	x1=$(($RANDOM % $(($boardSize - 3)) + 1))
	y1=$(($RANDOM % $boardSize + 1))
	while [[ $(($x1-$x2)) -lt 8 && $(($y1-$y2)) -lt 8 ]]; do
		x1=$(($RANDOM % $(($boardSize - 3)) + 1))
		y1=$(($RANDOM % $boardSize + 1))
		tries=$(($tries+1))
	done
	if [[ $(($x1-$x2)) -lt 8  && $(($x1-$x2)) -gt -8 ]] && [[ $(($y1-$y2)) -lt 8 && $(($y1-$y2)) -gt -8 ]] && [[ $tries -lt $maxTries ]];  then
		exit 1
	fi

elif [[ "$x1" != "" && "$x2" == "" ]]; then
	x2=$(($RANDOM % $(($boardSize - 3)) + 1))
	y2=$(($RANDOM % $boardSize + 1))
	while [[ $(($x1-$x2)) -lt 8  && $(($x1-$x2)) -gt -8 ]] && [[ $(($y1-$y2)) -lt 8 && $(($y1-$y2)) -gt -8 ]] && [[ $tries -lt $maxTries ]];  do
		x2=$(($RANDOM % $(($boardSize - 3)) + 1))
		y2=$(($RANDOM % $boardSize + 1))
		tries=$(($tries+1))
	done
	if [[ $(($x1-$x2)) -lt 8  && $(($x1-$x2)) -gt -8 ]] && [[ $(($y1-$y2)) -lt 8 && $(($y1-$y2)) -gt -8 ]]; then
		exit 1
	fi

else
	if [[ $(($x1-$x2)) -lt 8 && $(($x1-$x2)) -gt -8 ]] && [[ $(($y1-$y2)) -lt 8 && $(($y1-$y2)) -gt -8 ]]; then
		exit 1
	fi
fi

echo $x1 $y1 $x2 $y2

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

	echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&7
	echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&7

	wait $pid_gui
	gui_ret=$?
	echo gui_ret
	if [[ "$gui_ret" == "" ]]; then
		exit 1
	fi

	if [[ $gui_ret -ne 0 ]]; then
		exit 1
	fi

	exit 0 

elif [[ "$ai1" != "" && "$ai2" != "" ]]; then
	$ai1 <&3 >&4 &
	pid1=$!

	echo pid1 $pid1

	$ai2 <&5 >&6 2> /dev/null &
	pid2=$!

	echo pid2 $pid2

	$GUI_PATH <&7 >&8 &
	pid_gui=$!

	echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&7
	echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&7

	echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&3
	echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&5

	while true; do
		echo 1
		while true; do

			flag=0
			while true; do
				read -t 0.1 s <&4
				st=$?
				if [[ $st -eq 0 ]]; then
					break
				fi
				if [[ $st -ne 0 ]] && !(ps -p $pid1 > /dev/null); then
					flag=1
					break
				fi
			done
			if [[ $flag -eq 1 ]]; then
				break;
			fi

			echo $s
			echo $s >&7
			echo $s >&5
			echo $s >> ai2.input
			sleep $timeLimit
			if [[ "$s" == "END_TURN" ]]; then
				break
			fi
		done
		echo 2
		while true; do
			flag=0
			while true; do
				read -t 0.1 s <&6
				st=$?
				if [[ $st -eq 0 ]]; then
					break
				fi
				if [[ $st -ne 0 ]] && !(ps -p $pid2 > /dev/null); then
					flag=1
					break
				fi
			done
			if [[ $flag -eq 1 ]]; then
				break;
			fi
			echo $s
			echo $s >&3
			echo $s >&7
			echo $s >> ai1.input
			sleep $timeLimit
			if [[ "$s" == "END_TURN" ]]; then
				break
			fi
		done

		if !(ps -p $pid_gui > /dev/null); then
			wait $pid_gui
			retg=$?
			if [[ "$retg" != "" ]] && [[ $retg -gt 0 ]]; then
				break
			fi
		fi

		if !(ps -p $pid1 > /dev/null) && !(ps -p $pid2 > /dev/null); then
			wait $pid1
			ret1=$?

			wait $pid2
			ret2=$?

			echo both dead
			break
		fi 
	done
	
	kill $pid1
	kill $pid2
	kill $pid_gui

	if [[ "$retg" != "" ]] && [[ $retg -gt 0 ]]; then
		exit 1
	fi

	if [[ "$ret1" == "" ]]; then
		exit 1
	fi

	if [[ $ret1 -gt 2 ]]; then
		exit 1
	fi
	
	if [[ "$ret2" == "" ]]; then
		exit 1
	fi

	if [[ $ret2 -gt 2 ]]; then
		exit 1
	fi
	exit 0

elif [[ "$ai1" != "" && "$ai2" == "" ]]; then
	$ai1 <&3 >&4 2> /dev/null &
	pid1=$!

	$GUI_PATH -human2 <&7 >&8 &
	pid_gui=$!

	echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&7
	echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&7

	echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&3

	while true; do
		echo 1
		while true; do
			flag=0
			while true; do
				read -t 0.1 s <&4
				st=$?
				if [[ $st -eq 0 ]]; then
					break
				fi
				if [[ $st -ne 0 ]] && !(ps -p $pid1 > /dev/null); then
					flag=1
					break
				fi
			done
			if [[ $flag -eq 1 ]]; then
				break;
			fi
			echo $s
			echo $s >&7
			echo $s >> ai2.input
			sleep $timeLimit
			if [[ "$s" == "END_TURN" ]]; then
				break
			fi
		done

		while true; do
			flag=0
			while true; do
				read -t 0.1 s <&8
				st=$?
				if [[ $st -eq 0 ]]; then
					break
				fi
				if [[ $st -ne 0 ]] && !(ps -p $pid_gui > /dev/null); then
					flag=1
					break
				fi
			done
			if [[ $flag -eq 1 ]]; then
				break;
			fi
			echo $s >&3

			if [[ "$s" == "END_TURN" ]]; then
				break
			fi
		done

		if !(ps -p $pid1 > /dev/null); then
			wait $pid1
			ret=$?
			break
		fi 
	done

	kill $pid1
	kill $pid_gui

	if [[ "$ret" == "" ]]; then
		exit 1
	fi

	if [[ $ret -gt 2 ]]; then
		exit 1
	fi
	
	exit 0

elif [[ "$ai1" == "" && "$ai2" != "" ]]; then
	$ai2 <&5 >&6 2> /dev/null &
	pid2=$!

	$GUI_PATH -human1 <&7 >&8 &
	pid_gui=$!

	echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&7
	echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&7

	echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&5

	while true; do
		echo human
		while true; do
			flag=0
			while true; do
				read -t 0.1 s <&8
				st=$?
				if [[ $st -eq 0 ]]; then
					break
				fi
				if [[ $st -ne 0 ]] && !(ps -p $pid_gui > /dev/null); then
					flag=1
					break
				fi
			done
			if [[ $flag -eq 1 ]]; then
				break;
			fi
			echo $s >&5
			if [[ "$s" == "END_TURN" ]]; then
				break
			fi
		done

		echo mock
		while true; do
			flag=0
			while true; do
				read -t 0.1 s <&6
				st=$?
				if [[ $st -eq 0 ]]; then
					break
				fi
				if [[ $st -ne 0 ]] && !(ps -p $pid2 > /dev/null); then
					flag=1
					break
				fi
			done
			if [[ $flag -eq 1 ]]; then
				break;
			fi
			echo $s >&7
			if [[ "$s" == "END_TURN" ]]; then
				break
			fi
		done

		if !(ps -p $pid2 > /dev/null); then
			wait $pid2
			ret=$?
			break
		fi 
	done

	kill $pid2	
	kill $pid_gui

	if [[ "$ret" == "" ]]; then
		exit 1
	fi

	if [[ $ret -gt 2 ]]; then
		exit 1
	fi
	
	exit 0
fi
