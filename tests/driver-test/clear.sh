#!/bin/bash

DPATH="/sys/kernel/debug/tracing"

echo 0 > $DPATH/tracing_on

cp $DPATH/trace /home/david
mv /home/david/trace tracing2.txt

echo nop > $DPATH/trace
echo 0 > $DPATH/set_ftrace_pid
