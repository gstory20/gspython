#!/bin/sh
#
#  testone
#
#  Glenn Story -- 2/25/5
#
#  Test one program
#
#  This script will create the file $1.tst if a test is successful
#

help="Usage:  testone.sh COMMAND"

# shellcheck disable=SC2209
ECHO=echo

if [ $# -lt 1 ] | [ "$1" = "-h" ] | [ "$1" = "--help" ]
then
    echo "$help"
    exit 1
fi

rm core*
rm "$BINDIR"/core*
rm /core
rm /cores/*

base=$(basename "$1")

#  Handle interactive tests
if [ -f "$base".i ]
then
	echo "Testing $1 (interactive) ..."
	$1
	rc=$?
	if [ $rc -ne 0 ]
	then
		echo "$1" issued return code $rc
		exit 1
	fi

	echo 
	$ECHO "Was the test successful? \c"
	read -r answer
	case $answer in
	Y*|y*)   touch "$base".tst;;
	*)	exit 1;;
	esac

	exit 0
fi

echo "Testing $1..."
if [ -f "$base".in ]
then
	# shellcheck disable=SC2048
	$* < "$base".in > "$1".out
	rc=$?
else
	# shellcheck disable=SC2048
	$* | tee "$1".out
	rc=$?
fi

if ls core* || ls /cores/* 
then
        echo "$1 created a core file"
	exit 1
fi

if [ $rc -ne 0 ]
then
	echo "$1 issued return code $rc"
	exit 1
fi

if [ -f "$base".tst ]
then
	# rerun previous test
#	$BINDIR/qacomp $1.out $base.tst
	diff "$base".tst "$1".out
	rc=$?
else
	more "$1".out
	rc=1
fi

case $rc in
0)      touch "$base".tst;;

1)      while true
        do
	    $ECHO "Is the new output correct? (y n ?)  \c"
	    read -r answer
	    case $answer in
		Y*|y*)  mv "$1".out "$base".tst
                        exit 0
			;;

		N*|n*)  exit 1;;
		?)      more "$1".out;;
	    esac
	done
	;;
2)	exit 2;;
esac

exit 0
