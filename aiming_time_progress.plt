
# set terminal postscript eps color enhanced font 14
set term postscript eps enhanced color font 16

set output "doc/img/aiming_time_progress.eps"

f(x) = m*x + b
g(x) = n*x + c
fit f(x) 'stats/stats.txt' via m,b
set label 1 sprintf("aim fit:  m = %3.4f",m) at 15,3.5 font "mono,12"
set label 2 sprintf("          b = %3.4f",b) at 15,3.4 font "mono,12"

#fit g(x) 'stats/stats.median' via n,c
#set label 3 sprintf("mean fit: n = %3.4f",n) at 2,2.6 font "mono,12"
#set label 4 sprintf("          c = %3.4f",c) at 2,2.4 font "mono,12"

set xlabel "Correct clicks"
set ylabel "Interclick delay (aiming time)"


plot    "stats/stats.txt" title 'Aiming', \
        f(x) title 'Linear fit of aiming', \
        "stats/stats.median" title 'Aiming (median filter, win=5)'
#        g(x) title 'Linear fit of aiming median'

# pause -1

