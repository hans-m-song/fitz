# fitz
A simple tile based game

use `make` to compile

use `make BUILD=test` to compile with debugging information

use `make BUILD=verbose` to compile with LOTS of debugging information

run with `./fitz tilefile [p1type p2type [height width | savegame]]`

`tilefile` is a file containing tiles,

`p1type` and `p2type` should be either 1, 2, or h

`height` and `width` should be 1 <= x <= 999

`savegame` is a file containing a saved game state
