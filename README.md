# Conway's Game of Life

## Description

The Game of Life, also known simply as Life, is a cellular automaton devised by the British mathematician John Horton Conway in 1970. It is a zero-player game, meaning that its evolution is determined by its initial state, requiring no further input. One interacts with the Game of Life by creating an initial configuration and observing how it evolves. It is Turing complete and can simulate a universal constructor or any other Turing machine.
[Wikipedia](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)

## Implementation

This implementation is written in C99 using the standard library, unistd.h, and the SDL2 library.

## Console line parameters

- `-r {string}` - game rules (e.g., `-r B368/S245`, default `B3/S23`)
- `-f {int}` - maximum FPS (also determines ticks per second, default `24`)
- `-c {hex}` - primary (cells) color (hex, default `00FF00`)
- `-b {hex}` - background color (hex, default `000000`)

## Controls

- `Esc` - exit
- `Space` - pause
- `R` - reset field
- `C` - clear field
- `LMB` - draw cells
- `RMB` - erase cells

## Build

```shell
git clone https://github.com/ilya-nikolaev/ConwaysGameOfLife.git
cd ConwaysGameOfLife
mkdir build && cd build
cmake .. && make
./game
```

## TO-DO

- [ ] **Symbolic Positioning**
      Support definitions like `x == 5`, `y == x`, `(x + y) % 2 == 0`

- [X] **Camera Control**
      Pan around the board via MMB

- [ ] **Zoom Functionality**
      Allow zooming in/out with `-` and `=` keys

- [ ] **Grid**
      Allow to enable grid with zoom >= 3
