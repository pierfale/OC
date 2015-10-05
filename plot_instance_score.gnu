set key outside bmargin left box

set logscale y
set xtics 5
set grid xtics

plot filename using 1:2:3:4 with yerrorbars title 'score divergence'
