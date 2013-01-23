#!/bin/bash
media=0
media_total=0
rm *.csv
rm *.txt
#for i in $(seq 1 2)
for ((j=$1; j<=$2; j=j+$3))
do
	media=0;
	media_total=0;
	for i in $(seq 1 $4)
	do
		ns cenario3a.tcl cenario3.$j.$i.csv $j
		trace-filter "(Iv=32)" > cenario3.$j.filtrado.$i.csv < cenario3.$j.$i.csv
		trace-filter "(Nw=COL)" > cenario3.$j.dropped1.$i.csv < cenario3.$j.filtrado.$i.csv
		trace-filter "(Nw=END)" > cenario3.$j.dropped2.$i.csv < cenario3.$j.filtrado.$i.csv
                trace-filter "(Nl=AGT)" > cenario3.$j.normal.$i.csv < cenario3.$j.filtrado.$i.csv
		rm cenario3.$j.$i.csv
		linhas=$(cat cenario3.$j.dropped1.$i.csv | wc -l)
		linhas=$linhas+$(cat cenario3.$j.dropped2.$i.csv | wc -l)
		linhas_total=$(cat cenario3.$j.normal.$i.csv | wc -l)
		media=$((media + linhas))
		media_total=$((media_total+linhas_total))
	done
	#touch plot.txt
	media=$((media/2))
	echo $media
	media_total=$((media_total/2))
	echo $media_total
	x=$(echo "scale=5; ($media / $media_total)" | bc -l)
	echo $j 0$x >> plot.txt
done
#rm *.csv
#touch plot.txt
#media=$((media/2))
#echo $media
#media_total=$((media_total/2))
#echo $media_total
#echo "scale=5; ($media / $media_total)*10" | bc -l >> plot.txt
