i=0

while [ $i -le 6 ]; do
    main $i
    gnuplot latexSubdExport.gp
    mv latex/subdPlot.eps latex/subdPlot${i}.eps
    gnuplot latexAngleExport.gp
    mv latex/anglePlot.eps latex/anglePlot${i}.eps
    i=$((${i} + 1))
done;
