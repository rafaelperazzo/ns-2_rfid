rm -f *.tr
rm -f *.csv
rm -f *.nam
ns rfid.tcl
awk -f nodes.awk rfid.tr
