/* Test file for Dependencies Mapping Plugin -
 PlayerData.h */

#pragma once
#include "Player.h"
#include "Board.h"

struct PlayerData {
	GeneralPlayer * player;
	PlayerBoard playerBoard;
	friend class Game;
public:
	PlayerData(GameBoard * boardPtr, int player) : playerBoard(boardPtr, player), player(nullptr) { }
};
