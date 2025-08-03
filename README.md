![](/assets/image.png)
# RiverRaider
a classic shooter for 32Blit.

![](/assets/screenshot.bmp)

A very loose adaptation of Carol Shaw's successful shooter game "River Raid," released by Atari for the 2600VCS in 1982.
Inspired by the excellent adaptation (River Raider, guscosta, 2022) for the Pico-8 virtual console, this version was created for the 32Blit console.
I went the furthest with this version, using the sprites from Guscosta.
The game is still in a very early beta phase and currently ends somewhat abruptly at the end of the four levels.
Trying it out and have fun...

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
