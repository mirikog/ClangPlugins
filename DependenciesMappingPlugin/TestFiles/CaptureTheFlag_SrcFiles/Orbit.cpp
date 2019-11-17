/* Test file for Dependencies Mapping Plugin -
 Orbit.cpp */

#include "Orbit.h"

Orbit::Orbit(int col, int row)
{
	orbit[0].col = col;	// up
	orbit[0].row = row - 1;
	orbit[1].col = col;	// down
	orbit[1].row = row + 1;
	orbit[2].col = col - 1;// left
	orbit[2].row = row;
	orbit[3].col = col + 1;// right
	orbit[3].row = row;
}

BoardPosition Orbit::operator[](int dir)
{
	return orbit[dir];
}
