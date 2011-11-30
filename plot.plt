

f(x) = m*x + b
g(x) = n*x + c
fit f(x) 'mylog.diffs' via m,b
set label 1 sprintf("aim fit:  m = %3.4f",m) at 2,3 font "mono,10"
set label 2 sprintf("          b = %3.4f",b) at 2,2.8 font "mono,10"

fit g(x) 'mylog.median' via n,c
set label 3 sprintf("mean fit: n = %3.4f",n) at 2,2.6 font "mono,10"
set label 4 sprintf("          c = %3.4f",c) at 2,2.4 font "mono,10"

set xlabel "Correct clicks"
set ylabel "Interclick delay (aiming time)"


plot    "mylog.diffs" title 'Aiming', \
        "mylog.median" title 'Aiming (median)', \
        f(x) title 'Linear fit of aiming', \
        g(x) title 'Linear fir of aiming median'
pause -1
