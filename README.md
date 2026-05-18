# Conway's Game of Life

## Description

The Game of Life, also known simply as Life, is a cellular automaton devised by the British mathematician John Horton Conway in 1970. It is a zero-player game, meaning that its evolution is determined by its initial state, requiring no further input. One interacts with the Game of Life by creating an initial configuration and observing how it evolves. It is Turing complete and can simulate a universal constructor or any other Turing machine.
[Wikipedia](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)

## Implementation

This implementation is written in C99 using the standard library, OpenMP and the SDL2 library.

## Controls

- `Esc` - exit
- `R` - randomize field
- `Space` - play/pause
- `LMB` - pan around the board
- `MWHEELUP`/`MWHEELDOWN` - zoom

## Build

```shell
git clone https://github.com/ilya-nikolaev/ConwaysGameOfLife.git
cd ConwaysGameOfLife
mkdir build && cd build
cmake ..
cmake --build .
./game
```

## TO-DO

- [ ] **Symbolic Positioning**
      Support definitions like `x == 5`, `y == x`, `(x + y) % 2 == 0`

- [X] **Camera Control**
      Pan around the board via LMB

- [X] **Zoom Functionality**
      Allow zooming in/out with `MWHEELUP` and `MWHEELDOWN` keys

- [ ] **Improve performace**
      The goal is 60 FPS on a Ryzen 7 7700 with 4k resolution and a 4096x4096 field.

- [ ] **CLI**
      Allow to use CLI arguments without getopt dependency
