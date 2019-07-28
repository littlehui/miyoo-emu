PCSX ReARMed porting for miyoo/bittboy handhled

This porting is based on caanoo platform and remove some pollux settings.

How to config:
  $ export PATH=$PATH:/opt/miyoo/bin
  $ CROSS_COMPILE=arm-linux- ./configure --platform=miyoo --disable-neon

How to compile:
  $ make clean && make
  $ make rel

Package file:
  pcsx_rearmed_v1.9_miyoo.zip

Enjoy !

Steward Fu
