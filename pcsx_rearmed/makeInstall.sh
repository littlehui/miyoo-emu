export PATH=$PATH:/opt/miyoo/bin
CROSS_COMPILE=arm-linux- ./configure --platform=miyoo --disable-neon
make clean
make
cp pcsx /media/littlehui/ROMS/emus/pcsx_rearmed/