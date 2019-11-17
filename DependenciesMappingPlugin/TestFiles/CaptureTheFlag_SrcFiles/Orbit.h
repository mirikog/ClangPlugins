/* Test file for Dependencies Mapping Plugin -
 Orbit.h */

#pragma once
#include "BoardPosition.h"
enum O_DIR { up = 0, down = 1, left = 2, right = 3 };
struct Point { int x = -1, y = -1; };

class Orbit
{
	BoardPosition orbit[4];

public:
	Orbit(int row, int col);
	BoardPosition operator[](int dir);
};
