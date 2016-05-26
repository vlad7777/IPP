boardSize=10
turnsNumber=100
timeLimit=1
x1=1
y1=1
x2=1
y2=10
ai1=./sredniowiecze_gui_linux64_v1/sredniowiecze_gui
ai2=./sredniowiecze_gui_linux64_v1/sredniowiecze_gui

while [ $# -gt 0 ]
do
	case $1 in
		-n) 
			echo "setting board size to "
			shift
			echo $1
			boardSize=$1
			;;
		-k)
			echo "setting turns number to "
			shift
			echo $1
			turnsNumber=$1
			;;
		-s)
			echo "setting time limit to "
			shift
			echo $1
			timeLimit=$1
			;;
		-p1)
			echo "setting first player position to "
			shift
			echo $1
			;;
		-p2)
			echo "setting second player position to "
			shift
			echo $1
			;;
		-ai1)
			echo "setting ai1 to "
			shift
			echo $1
			ai1=$1
			;;
		-ai2)
			echo "setting ai2 to "
			shift
			echo $1
			ai2=$1
			;;
		-*)
			echo "wrong argument"
			exit
			;;
	esac
	shift
done

PIPE=$(mktemp -u)
mkfifo $PIPE
exec 3<>$PIPE

PIPE=$(mktemp -u)
mkfifo $PIPE
exec 4<>$PIPE

PIPE=$(mktemp -u)
mkfifo $PIPE
exec 5<>$PIPE

PIPE=$(mktemp -u)
mkfifo $PIPE
exec 6<>$PIPE

./$ai1 <&3 >&4 &
./$ai2 <&5 >&6 &

echo "INIT $boardSize $turnsNumber 1 $x1 $y1 $x2 $y2">&3
echo "INIT $boardSize $turnsNumber 2 $x1 $y1 $x2 $y2">&5
s=""
while true; do
	while read s <&4; do
		echo $s >&5;
	done
	while read s <&6; do
		echo $s >&3
	done
done
