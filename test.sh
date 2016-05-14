#         Uladzislau Sobal
#         ws374078
#         97022413196
#         zadanie Szpital 

if [ "$#" -lt 2 ]; then
	echo "Error: please supply at least two parameters (program tested and directory with tests";
	exit;
fi;

if [ "$#" -gt 3 ]; then
	echo "Too many arguments";
	exit;
fi;

if [ "$#" -eq 2 ]; then
	flag=0;
	file=$1;
	directory=$2;
fi;

if [ "$#" -eq 3 ]; then
	if [ "$1" == "-v" ]; then 
		flag=1;
		file=$2;
		directory=$3;
	else
		echo "Unknown first parameter, only -v is supported";
	fi;
fi;

if [ ! -f $file ]; then
	echo "$file not found";
	exit;
fi;

for f in $directory/*.in; do
	echo "$f: "; 
	#output and errlog are temporary files that hold the output of the tested program
	#these files we later compare to .out and .err files correspondingly
	rm output > /dev/null 2> /dev/null;
	rm errlog > /dev/null 2> /dev/null;

	if [ "$flag" -eq 0 ]; then
		./$file <$f >output;	
	else
		./$file -v <$f >output 2>errlog;
	fi;
		
	echo -n "	Output:    ";
	if diff ${f%in}out output > /dev/null; then
	       	echo -e "\e[32mOK\e[0m";
       	else
		echo -e "\e[31mWrong Answer\e[0m";
		# used for debugging 
		cat output > ${f%in}myout;
	fi;

	if [ "$flag" -eq 1 ]; then
		echo -n "	Error log: ";
		if diff ${f%in}err errlog > /dev/null; then
			echo -e "\e[32mOK\e[0m";
		else
			echo -e "\e[31mWrong Answer\e[0m";
			# used for debugging 
			cat errlog > ${f%in}myerr;
		fi;
	fi;
done

# clean up
rm output > /dev/null 2> /dev/null;
rm errlog > /dev/null 2> /dev/null;
