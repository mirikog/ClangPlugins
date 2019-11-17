/* Test file for Dependencies Mapping Plugin -
 Flag.h */

#pragma once
#include "BoardPosition.h"

class Flag {
	BoardPosition pos;
public:

	Flag() {
		pos.row = pos.col = 0;
	}

	Flag(int row, int col) {
		pos.row = row;
		pos.col = col;
	}

	~Flag() {}

	void setRowCol(int row, int col) {
		pos.row = row;
		pos.col = col;
	}

	void setBoardPos(BoardPosition _pos) {
		pos = _pos;
	}

	BoardPosition getPos() const		{ return pos; }
	int getRow() const					{ return pos.row; }
	int getCol() const					{ return pos.col; }
};
