/* Test file for Dependencies Mapping Plugin -
 BoardData.h */
#pragma once

#define NUM_OF_ROWS_OR_COLUMNS 13
#define NUM_OF_PLAYERS 2
#define NUM_OF_PIECES_PER_PLAYER 3
#define POS_OUT_OF_BOARD_BOUNDS '*'
#define CHAR_OF_OPPONENTS_GAME_PIECE '#'

class BoardData {
public:
	enum { rows = 13, cols = 13 };
	virtual ~BoardData() { }
	virtual char charAt(int x, int y) const = 0;
};
