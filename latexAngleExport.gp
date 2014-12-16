set terminal epslatex color solid size 10,7
set output 'latex/anglePlot.eps'
unset ytics
unset xtics
set key off
plot 'results/AngleSubdivision6.txt' with points ps 5 pt 7