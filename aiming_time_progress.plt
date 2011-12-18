
set terminal postscript eps enhanced font 14

set output "doc/img/aiming_time_progress.eps"

f(x) = m*x + b
g(x) = n*x + c
fit f(x) 'stats/stats.txt' via m,b
set label 1 sprintf("aim fit:  m = %3.4f",m) at 2,3 font "mono,10"
set label 2 sprintf("          b = %3.4f",b) at 2,2.8 font "mono,10"

#fit g(x) 'stats/stats.median' via n,c
#set label 3 sprintf("mean fit: n = %3.4f",n) at 2,2.6 font "mono,10"
#set label 4 sprintf("          c = %3.4f",c) at 2,2.4 font "mono,10"

set xlabel "Correct clicks"
set ylabel "Interclick delay (aiming time)"


plot    "stats/stats.txt" title 'Aiming', \
        f(x) title 'Linear fit of aiming'

#       "mylog.median" title 'Aiming (median)', \
#        g(x) title 'Linear fit of aiming median', \

pause -1

