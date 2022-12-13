#!/bin/bash
 
DPATH="/sys/kernel/debug/tracing"
PID=$$
## Quick basic checks
[ `id -u` -ne 0  ]  &&  { echo "needs to be root" ; exit 1; }  # check for root permissions
[ -z $1 ] && { echo "needs process name as argument" ; exit 1; } # check for args to this function
mount | grep -i debugfs &> /dev/null
[ $? -ne 0 ] && { echo "debugfs not mounted, mount it first"; exit 1; } #checks for debugfs mount
 
# flush existing trace data
echo nop > $DPATH/current_tracer
 
# set function tracer
echo function_graph > $DPATH/current_tracer
 
# enable the current tracer
echo 1 > $DPATH/tracing_enabled
 
# write current process id to set_ftrace_pid file
echo $PID > $DPATH/set_ftrace_pid
 
# start the tracing
echo 1 > $DPATH/tracing_on

echo $PID

# execute the process
exec $*

