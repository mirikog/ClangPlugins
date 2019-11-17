/* Test file for Dependencies Mapping Plugin -
 GameMove.h */

#pragma once
#include <iostream>
#include "Directions.h"

struct GameMove {
	const int from_x, from_y;
	const int to_x, to_y;
	GameMove(int x1, int y1, int x2, int y2) : from_x(x1), from_y(y1), to_x(x2), to_y(y2) { }
};

class GameMove_Parsed
{
	Directions dir;
	int gpiece;
	long iteration;

public:
	GameMove_Parsed()
	{
		dir = Directions::STOP;
		gpiece = -1;
		iteration = -1;
	}

	void setDir(Directions d) { dir = d; }
	Directions getDir() { return dir; }
	void setGPiece(int gp) { gpiece = gp; }
	int getGPiece() { return gpiece; }
	void setIteration(long iter) { iteration = iter; }
	long getGIteration() { return iteration; }

	bool operator==(const GameMove_Parsed & move) {
		return (move.dir == dir && move.gpiece == gpiece);
	}

	bool operator!=(const GameMove_Parsed & move) {
		return (move.dir != dir || move.gpiece != gpiece);
	}

	GameMove_Parsed & operator=(const GameMove_Parsed & assignGameMove) {
		if (this != &assignGameMove) {
			dir = assignGameMove.dir;
			gpiece = assignGameMove.gpiece;
			iteration = assignGameMove.iteration;
		}
		return *this;
	}
};
