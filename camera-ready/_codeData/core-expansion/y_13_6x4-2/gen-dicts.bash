#!/bin/bash


YEAR=$(awk -F '_' '{print $2}' <<< "${PWD##*/}")
THEMDICT=them-dic-$YEAR.txt


cat input-data/dictionary.txt input-data/$THEMDICT | sort | uniq > input-data/combined-dict.txt

# Generate file 3-substr.txt with substrings of length 3 from all dictionary words
cat input-data/combined-dict.txt | awk '{for (i = 1; i <= length($THEMDICT)-2; i++) print (substr($THEMDICT, i, 3))}' | sort | uniq > input-data/3-substr.txt

# Generate file 4-substr.txt with substrings of length 4 from all dictionary words
cat input-data/combined-dict.txt | awk '{for (i = 1; i <= length($THEMDICT)-3; i++) print (substr($THEMDICT, i, 4))}' | sort | uniq > input-data/4-substr.txt

# Generate file 3-suffix.txt with suffixes of length 3 from all dictionary words
cat input-data/combined-dict.txt | awk '{print (substr($THEMDICT, length($i)-2, 3))}' | sort | uniq > input-data/3-suffix.txt

# Generate 5-letter substrings
cat input-data/combined-dict.txt | awk '{for (i = 1; i <= length($THEMDICT)-4; i++) print (substr($THEMDICT, i, 5))}' | sort | uniq > input-data/5-substr.txt

# Generate file 3-prefix.txt with prefixes of length 3 from all dictionary words
cat input-data/combined-dict.txt | awk '{print (substr($THEMDICT, 1, 3))}' | sort | uniq > input-data/3-prefix.txt

# Generate file 4-suffix.txt with suffixes of length 4 from all dictionary words
cat input-data/combined-dict.txt | awk '{print (substr($THEMDICT, length($i)-3, 4))}' | sort | uniq > input-data/4-suffix.txt

# Generate file 4-prefix.txt with prefixes of length 4 from all dictionary words
cat input-data/combined-dict.txt | awk '{print (substr($THEMDICT, 1, 4))}' | sort | uniq > input-data/4-prefix.txt

# Generate file 3-them-suffix.txt, with thematic suffixes of length 3
cat input-data/$THEMDICT | awk '{print (substr($THEMDICT, length($i)-2, 3))}' | sort | uniq > input-data/3-them-suffix.txt

# Generate file 3-them-prefix.txt, with thematic prefixes of length 3
cat input-data/$THEMDICT | awk '{print (substr($THEMDICT, 1, 3))}' | sort | uniq > input-data/3-them-prefix.txt

# Generate thematic substrings of length 3
cat input-data/$THEMDICT | awk '{for (i = 1; i <= length($THEMDICT)-2; i++) print (substr($THEMDICT, i, 3))}' | sort | uniq > input-data/3-them-substr.txt

# Generate thematic substrings of length 4
cat input-data/$THEMDICT | awk '{for (i = 1; i <= length($THEMDICT)-3; i++) print (substr($THEMDICT, i, 4))}' | sort | uniq > input-data/4-them-substr.txt

# Generate thematic substrings of length 5
cat input-data/$THEMDICT | awk '{for (i = 1; i <= length($THEMDICT)-4; i++) print (substr($THEMDICT, i, 5))}' | sort | uniq > input-data/5-them-substr.txt

# Generate thematic substrings of length 6
cat input-data/$THEMDICT | awk '{for (i = 1; i <= length($THEMDICT)-5; i++) print (substr($THEMDICT, i, 6))}' | sort | uniq > input-data/6-them-substr.txt

cat input-data/3-them-substr.txt input-data/4-them-substr.txt input-data/5-them-substr.txt input-data/6-them-substr.txt | sort > input-data/3-6-substr-them.txt
