#!/usr/bin/env bash

track=$1
tty=$(tty)

t0=`head -n 1 $track | awk '{print $1}'`
t=`tail -n 1 $track | awk '{print $1}'`
date -d @$t0
date -d @$t

awk '{print $8}' $track | sort | uniq -c | sort --numeric-sort --reverse

awk '($5 == "R3" && $8 == "getattr") {total += "0x"$21; cnt += 1} END {print total/cnt/1000000"MB"}' $track

awk '($5 == "R3" && $8 == "getattr") {print $3}' $track | sort | uniq -c | sort --numeric-sort --reverse | tee $tty | wc -l

