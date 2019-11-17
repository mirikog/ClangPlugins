/* Test file for Dependencies Mapping Plugin -
 GamePiece.cpp */

#include "GamePiece.h"

void GamePiece::setGamePiecePropertiesById(int _id) {

	switch (_id) {
	case 1:
		setGamePieceProperties(1, false, false);
		break;
	case 2:
		setGamePieceProperties(2, true, true);
		break;
	case 3:
		setGamePieceProperties(3, true, false);
		break;
	case 7:
		setGamePieceProperties(7, true, true);
		break;
	case 8:
		setGamePieceProperties(8, false, true);
		break;
	case 9:
		setGamePieceProperties(9, false, false);
		break;
	default:
		break;
	}
}

GamePiece * fight(GamePiece * gPiece1, GamePiece * gPiece2, int row, int column) {
	int id1, id2;
	id1 = gPiece1->getId();
	id2 = gPiece2->getId();

	//determine which game piece is from left side of game pieces array and leads the fight logic
	if (id1 < id2)
		return fightAux(gPiece1, gPiece2, row, column);
	return fightAux(gPiece2, gPiece1, row, column);
}

GamePiece * fightAux(GamePiece * gPieceA, GamePiece * gPieceB, int row, int column) {
	int idA, idB;
	idA = gPieceA->getId();
	idB = gPieceB->getId();

	// game logic is determined by the player A's id
	switch (idA) {
	case 1:
		if ((9 < row && row < 13) || 3 == column) {
			gPieceA->setDead();
			return gPieceB;
		}
		else {
			gPieceB->setDead();
			return gPieceA;
		}
		break;
	case 2:
		if (9 == idB) {
			gPieceA->setDead();
			return gPieceB;
		}
		else if (2 == row || 3 == row || 10 == column) {
			gPieceB->setDead();
			return gPieceA;
		}
		else {
			gPieceA->setDead();
			return gPieceB;
		}
		break;
	case 3:
		if (8 == row || 6 == column) {
			gPieceB->setDead();
			return gPieceA;
		}
		else {
			gPieceA->setDead();
			return gPieceB;
		}
		break;
	default:
		return nullptr;
	}
}
