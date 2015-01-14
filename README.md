School project
===========

Analyzing continuity of curves generated by subdivision surface algorithm.

* Chaikin
* Corner cutting
* Four-point
* Uniform cubic splines

Compilation & Execution
---------------------

    gcc -o main CurveSubdivision.c -lm

    # main can be called with {0,1, ...}
    # the argument defines what subdivision scheme to use
    ./main 1

    # exports all plots of each scheme for latex
    ./exportAllPlot.sh
