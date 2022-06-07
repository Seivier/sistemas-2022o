target remote :2345
b nShutdown
cond 1 rc!=0
b nMain
handle SIGUSR1 noprint
handle SIGUSR1 nostop
cont
