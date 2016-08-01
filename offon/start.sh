bin/offon_manager -Hhei -D0
bin/offon_fordb -Hhei
sleep 1
#bin/offon_entry -Hhei

cd scripts
python emu_hss.py 8080 >/dev/null &
python emu_tas.py 8081 >/dev/null &

