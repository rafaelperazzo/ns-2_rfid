rm -f mean.1.5.dat
rm -f slots.1.5.dat
arquivo="mean.1.5.dat"
arquivo2="slots.1.5.dat"
caminho="."
perl ic_q.pl $caminho| sort -g > $arquivo
perl ic_slots.pl $caminho| sort -g > $arquivo2
