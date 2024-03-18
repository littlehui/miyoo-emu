##make -f Makefile.rg350 clean
make -f Makefile

scp -r ./pocketsnes/PocketSNES.dge root@10.1.1.2:/media/sdcard/apps/snestmp
##scp -r ./opk/PocketSNES.opk root@10.1.1.2:/media/sdcard/simplemenu/emulators/PocketSNES_littlehui.opk