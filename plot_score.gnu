set ylabel "Score divergence"

set style data histogram
set xtics rotate
set bmargin 8

plot filename using 2:xticlabels(1) title 'average', filename using 3:xticlabels(1) title 'min', filename using 4:xticlabels(1) title 'max'
