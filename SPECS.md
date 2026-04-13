## Omni-2048 specifications

### Display
- Board should be horizontally centered and vertically slightly above the center.
- Above the board the total score is displayed to the left, and the value of the highest tile is displayed to the right.
- Below the board a message is displayed.
- This message indicates how to slide tiles if the highest tile is less than 2048 and the game isn't over.
- If the highest tile is at least 2048, display a message congradulating the player.
- If the game ends, the message should let the player know.
- Display should continually remain centered if player resizes the game window (if aplicable).

### Internal state representation
- Maximum possible tile value: 131072, Maximum possible score: Approximately 3 million.
- Every tile can be represented as n, where 2 << n is the tile's value.
- The game board, a 4x4 array of tiles where each tile is an unsigned 8 bit integer.
- The score, a 32 bit unsigned integer.
- The next input, an unsigned 16 or 32 bit integer, used to indicate input type and movement direction.
- A bitvector of at least 16 bits where each bit is used to indicate whether a tile moves or not.  Bits are grouped into 4 each with rightmost groups referring to higher rows.
