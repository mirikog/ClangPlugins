/* Test file for Dependencies Mapping Plugin -
 BoardPosition.h */

#pragma once
#include "BoardData.h"


struct BoardPosition {
	int row;
	int col;
public:
	BoardPosition() : row(NUM_OF_ROWS_OR_COLUMNS), col(NUM_OF_ROWS_OR_COLUMNS) {}

	BoardPosition(int _row, int _col) {
		row = _row;
		col = _col;
	}

	bool operator==(const BoardPosition & pos) {
		return pos.row == row && pos.col == col;
	}
};

