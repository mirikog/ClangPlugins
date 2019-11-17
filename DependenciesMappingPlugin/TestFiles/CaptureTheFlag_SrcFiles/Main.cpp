/* Test file for Dependencies Mapping Plugin -
 Main.cpp */

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <time.h>
#include <string>
#include <vector>
#include "Board.h"
#include "GamePiece.h"
#include "Tile.h"
#include "Game.h"
#include "Utils.h"
#include "Player.h"
using namespace std;

int main(int argc, char ** argv) {
	srand((unsigned int)time(NULL));
	Game game(argc, argv);
	game.gameManager();
}


