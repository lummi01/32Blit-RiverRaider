![](/assets/image.png)
# RiverRaider
v0.0.6<BR>
a classic shooter for 32Blit.<BR>

![](/assets/screenshot.bmp)<BR>
<BR>

A very loose adaptation of Carol Shaw's successful shooter game "River Raid," released by Atari for the 2600VCS in 1982.<BR>
<BR>
The sprites for this 32Blit version were largely used from the Pico-8 adaptation River Raider (guscosta, https://www.lexaloffle.com/bbs/?tid=47969).<BR>
<BR>
The game is still in a very early beta version and currently ends somewhat abruptly at the end of the four levels.<BR>
<BR>
Try it out and have fun...<BR>
<BR>
<BR>
For local build:
```
mkdir build
cd build
cmake -D32BLIT_DIR=/path/to/32blit-sdk/ ..
make
```

For 32Blit build:
```
mkdir build.stm32
cd build.stm32
cmake .. -D32BLIT_DIR=/path/to/32blit-sdk/ -DCMAKE_TOOLCHAIN_FILE=/path/to/32blit-sdk/32blit.toolchain
make
```

For PicoSystem build:
```
mkdir build.pico
cd build.pico
cmake .. -DCMAKE_TOOLCHAIN_FILE=../../32blit-sdk/pico.toolchain -DPICO_BOARD=pimoroni_picosystem
make
```
