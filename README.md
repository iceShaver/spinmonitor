# SpinMonitor

## Description
Linux HDD activity monitoring application.

## Architecture
client - server (daemon) architecture. 

daemon:
* runs in background
* monitors HDD activity and logs activity info with specified interval
* responds to client queries
* to run daemon: # spinmonitor -d [options]

client:
* displays info acquired from daemon
* controls daemon 
* to run client: $ spinmonitor [options]


## Dependencies
* smartmontools
* sdparm
* hdparm
* lsof

## Other
* saves logs to working directory
