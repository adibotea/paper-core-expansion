#!/bin/bash

# Generate 6x4 cores with wombat
time ./wombat -v 1 -ntw3456 20 20 20 20 -mnw 100 -s 1000000 -i input-data/6x4-puzzle.pzl -mts 44 -t -1 -start_slot -1 -search 5 -heur 13 -weight .8 -o cores-6x4.txt
#mv core-mini-grids.txt input-data




# Generate initial .pzl files
time python ../src/main.py input-data/cores-6x4.txt input-data/them-dic-19.txt input-data/3-prefix.txt input-data/3-substr.txt input-data/3-suffix.txt input-data/4-prefix.txt input-data/4-substr.txt input-data/4-suffix.txt input-data/5-substr.txt

# Further improve the results this way:
# -- detect additional deadlocked puzzles
# -- place additional forced black points
# -- place additional forced letters
#mv pzl-results/v1
#for i in puzzle-* ; do time ./dyn-bp-wombat -v 1  -s 1 -n 3 -i $i ; done
# This will generate two folders, dead/ and legal/, and will populate them
# accordingly. Some easy file renaming and folder movements might
# be manually required at the end. 

