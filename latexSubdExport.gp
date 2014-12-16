set terminal epslatex color solid size 10,7
set output 'latex/subdPlot.eps'
set key left
unset ytics
unset xtics
set style line 1 lt 1 lw 5 lc rgb "navy"
set style line 2 lt 1 lw 5 lc rgb "purple"
set style line 3 lt 1 lw 5 lc rgb "green"
set style line 4 lt 1 lw 5 lc rgb "chartreuse"
set style line 5 lt 1 lw 5 lc rgb "yellow"
set style line 6 lt 1 lw 5 lc rgb "orange"
set style line 7 lt 1 lw 5 lc rgb "red"
plot 'results/Subdivision0.txt' with lines ls 1 title '\footnotesize level 0',\
     'results/Subdivision1.txt' with lines ls 2 title '\footnotesize level 1',\
     'results/Subdivision2.txt' with lines ls 3 title '\footnotesize level 2',\
     'results/Subdivision3.txt' with lines ls 4 title '\footnotesize level 3',\
     'results/Subdivision4.txt' with lines ls 5 title '\footnotesize level 4',\
     'results/Subdivision5.txt' with lines ls 6 title '\footnotesize level 5',\
     'results/Subdivision6.txt' with lines ls 7 title '\footnotesize level 6'
