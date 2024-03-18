##make -f Makefile.rg350
export PATH=$PATH:/opt/miyoo/bin
make -f Makefile.miyoo
cp fceux /media/littlehui/ROMS/emus/fceux-dev/
sleep 2
sudo umount /media/littlehui/ROMS
sudo umount /media/littlehui/OD
