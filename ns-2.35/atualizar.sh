cd /home/rafael/Downloads/ns-allinone-2.35/
cp atualizar.sh /home/rafael/Downloads/ns-allinone-2.35/ns-2.35/
now=$(date '+%d/%m/%Y %H:%M')
git add /home/rafael/Downloads/ns-allinone-2.35/ns-2.35
git commit -a -m "Version $now"
git push origin master --force
