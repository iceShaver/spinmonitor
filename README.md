# SpinMonitor

## description
Application which allows to track HDD spinning status and generate statistics. Useful for controlling disks consumed
 energy and spin up count.

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
* lsof
* boost
