/* Test file for Dependencies Mapping Plugin -
 BoardFileData.h */

#pragma once
#include <vector>

#define NUM_OF_PLAYERS 2
#define NUM_OF_PIECES_PER_PLAYER 3

struct BoardFileData {
	int gamePieces[NUM_OF_PLAYERS][NUM_OF_PIECES_PER_PLAYER] = { { 0, 0, 0 },{ 0, 0, 0 } };
	int flags[NUM_OF_PLAYERS] = { 0 };
	bool isError_player[NUM_OF_PLAYERS] = { false, false };
	vector<char> illegalChars;
	friend class GameBoard;
};
