#!/bin/bash

OUTPUT_FILE_NAME="./ocena-cz2.txt"
TESTING_SCRIPT=$(realpath "`dirname ${BASH_SOURCE[0]}`/tests2/run_all_tests.sh")

if [[ $# != 1 ]]; then
	echo "Usage: ./check_middle2.sh <path to solution root dir>" >&2
	echo "Can be called from any working directory." >&2
	echo "Might not work if there will be spaces in paths. Download the correct repo contents yourself." >&2
	exit 1
fi

cd "$1"

if ! cmake .; then
	echo "CMake failed."
	exit 1
fi

if ! make; then
	echo "make failed."
	exit 1
fi

"$TESTING_SCRIPT" "$PWD" 2>&1 | tee "$OUTPUT_FILE_NAME"
