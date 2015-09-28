set ylabel "Time (micro seconds)"

set style data histogram
set xtics rotate
set bmargin 8

plot filename using 5:xticlabels(1) title 'average', filename using 6:xticlabels(1) title 'min', filename using 7:xticlabels(1) title 'max'
