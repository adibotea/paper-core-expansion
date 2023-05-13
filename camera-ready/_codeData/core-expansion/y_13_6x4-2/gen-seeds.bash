#!/bin/bash

YEAR=$(awk -F '_' '{print $2}' <<< "${PWD##*/}")
PATTERN=$(awk -F '_' '{print $3}' <<< "${PWD##*/}")


process_pattern () {
    echo "Generating cores with Wombat run on the substring core puzzle"
    time ./wombat -v 1 -ntw3456 20 20 20 20 -mnw 100 -s 1000000 -i input-data/$PATTERN.pzl -mts 20 -t -1 -start_slot -1 -search 5 -heur 13 -weight .8 -o cores.txt > wombat.log 2>&-
    mv cores.txt input-data
    echo "Finished generating cores with Wombat run on the substring core puzzle"

    echo "Generating seeds with Corex 1.0"
    time python2 ../../src/main.py input-data/cores.txt input-data/them-dic-$YEAR.txt input-data/3-prefix.txt input-data/3-substr.txt input-data/3-suffix.txt input-data/4-prefix.txt input-data/4-substr.txt input-data/4-suffix.txt input-data/5-substr.txt
    echo "Finished generating seeds with Corex 1.0"

    rm -rf y-$YEAR-$PATTERN-alive
    mkdir y-$YEAR-$PATTERN-alive
    echo "Seed static processing with Wombat-DBC"
    time for i in puzzle-* ; do 
    	sed -i "s/dic-21/dic-$YEAR/g" $i ; 
	./dyn-bp-wombat -v 0 -s 1 -n 1 -i $i > /dev/null 2>&1 ; 
	sed -i "s/dic-21/dic-$YEAR/g" alive-0-$i ; 
	mv alive-0-$i y-$YEAR-$PATTERN-alive/ 2>/dev/null ;
	rm -f $i ;
    done 
    echo "Finished seed static processing with Wombat-DBC"

    tar cfz y-$YEAR-$PATTERN-alive.tgz y-$YEAR-$PATTERN-alive
    rm -rf y-$YEAR-$PATTERN-alive
    mv corex.log $YEAR-$PATTERN-corex.log
    mv wombat-dbc.log $YEAR-$PATTERN-wombat-dbc.log
}


process_pattern > summary-$YEAR-$PATTERN.txt 2>&1
