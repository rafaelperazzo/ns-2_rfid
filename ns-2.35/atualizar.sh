cp atualizar.sh ns-2.35/
now=$(date '+%d/%m/%Y %H:%M')
git add ns-2.35
git commit -a -m "Version $now"
git push origin master
