![](/assets/image.png)
# RiverRaider
v0.0.9<BR>
a classic shooter for 32Blit.<BR>

![](/assets/Screenshot/River Raider0.bmp)![](/assets/Screenshot/River Raider1.bmp)![](/assets/Screenshot/River Raider2.bmp)<BR>
<BR>

A very loose adaptation of Carol Shaw's successful shooter game "River Raid," released by Atari for the 2600VCS in 1982.<BR>
<BR>
The sprites for this 32Blit version were largely used from the Pico-8 adaptation River Raider (guscosta, https://www.lexaloffle.com/bbs/?tid=47969).<BR>
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
