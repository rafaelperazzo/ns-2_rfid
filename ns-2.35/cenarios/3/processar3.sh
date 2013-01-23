#!/bin/bash
media=0
media_total=0
rm -f *.csv
rm -f *.txt
rm -f *.plt
rm -f *.png
rm -f INICIO
rm -f FIM
echo $(date) >> INICIO
for ((j=$1; j<=$2; j=j+$3))
do
	media=0
	media_total=0
	linhas=0
	linhas_total=0
	touch $j.txt
	for i in $(seq 1 $4)
	do
		ns cenario3.tcl cenario3.$j.$i.csv $j
		trace-filter "(Id=0)" > cenario3.$j.filtrado.$i.csv < cenario3.$j.$i.csv
		#trace-filter "(Iv=32)" > cenario3.$j.filtrado.$i.csv < cenario3.$j.$i.csv
		#trace-filter "(Nw=COL)" > cenario3.$j.dropped1.$i.csv < cenario3.$j.filtrado.$i.csv
		#trace-filter "(Nw=END)" > cenario3.$j.dropped2.$i.csv < cenario3.$j.filtrado.$i.csv
                #trace-filter "(Nl=AGT)" > cenario3.$j.normal.$i.csv < cenario3.$j.filtrado.$i.csv
		rm cenario3.$j.$i.csv
		linhas=$(awk -f processar_lidas.awk cenario3.$j.filtrado.$i.csv | uniq -u | wc -l)
		echo $linhas
		#linhas=$linhas+$(awk -f processar_lidas.awk cenario3.$j.normal.$i.csv | uniq -d | wc -l)
		linhas=$(echo "scale=10; ($linhas+$(awk -f processar_lidas.awk cenario3.$j.filtrado.$i.csv))"| uniq -d| wc -l)
		#linhas_total=$(($j-1))
		linhas_total=$(echo "scale=10; ($j-1)" | bc -l)
		#media=$((media + linhas))
		media=$(echo "scale=10; ($media+$linhas)" | bc -l)
		#media_total=$((media_total+linhas_total))
		media_total=$(echo "scale=10; ($media_total+$linhas_total)" | bc -l)
		echo $linhas_total
		echo $media
		echo $media_total
		#linhas=$(echo "scale=10; ($linhas)" | bc -l)
		echo $linhas
		temp=$(echo "scale=10; (($linhas_total - $linhas) / $linhas_total)" | bc -l)
		echo $temp
		echo 0$temp >> $j.txt
	done
	#media=$((media/$4))
	#media=$(echo "scale=10; ( $media / $4)" | bc -l)
	#media_total=$((media_total/$4))
	#media_total=$(echo "scale=10; ($media_total / $4)" | bc -l)
	#y=$((media_total-media | bc -l))
	#x=$(echo "scale=10; ($y / $media_total)"| bc -l )
	#echo $j 0$x >> plot3.dat
	rm -f *.csv
done
echo $(date) >> FIM
perl ic.pl | sort -g > plot3.dat
