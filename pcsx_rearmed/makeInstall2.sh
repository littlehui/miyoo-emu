export PATH=$PATH:/opt/miyoo/bin
export TARGETMACH=arm-linux
export BUILDMACH=i686-pc-linux-gnu
export CROSS=arm-linux
export CC=${CROSS}-gcc
export LD=${CROSS}-ld
export AS=${CROSS}-as
export CXX=${CROSS}-g++

CROSS_COMPILE=arm-linux- ./configure --platform=miyoo --disable-neon
make clean
make