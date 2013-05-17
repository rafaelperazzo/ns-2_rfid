rm -f mean.02.5.dat
rm -f slots.02.5.dat
arquivo="mean.02.5.dat"
arquivo2="slots.02.5.dat"
caminho="."
perl ic_q.pl $caminho| sort -g > $arquivo
perl ic_slots.pl $caminho| sort -g > $arquivo2
