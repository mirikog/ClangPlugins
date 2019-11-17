/* Test file for Dependencies Mapping Plugin -
 GamePiece.h */

#pragma once
#include <iostream>
#include <stdlib.h>
#include "BoardPosition.h"
using namespace std;

#define A_GP_1	'1'
#define A_GP_2	'2'
#define A_GP_3	'3'
#define B_GP_7	'7'
#define B_GP_8	'8'
#define B_GP_9	'9'

class GamePiece {
	int id = 0; // same as the number of the piece
	bool isAmphibian;
	bool isForestRanger;
	BoardPosition pos;
	bool isAlive;

public:
	GamePiece() {
		pos.row = 0;
		pos.col = 0;
		id = 0;
		isAmphibian = true;
		isForestRanger = true;
		isAlive = true;
	}

	GamePiece(int _id) {
		pos.row = 0;
		pos.col = 0;
		setProperies(_id);
		isAlive = true;
	}

	void setProperies(int id) {
		switch (id) {
		case 1:
			(*this).setGamePieceProperties(1, false, false);
			break;
		case 2:
			(*this).setGamePieceProperties(2, true, true);
			break;
		case 3:
			(*this).setGamePieceProperties(3, true, false);
			break;
		case 7:
			(*this).setGamePieceProperties(7, true, true);
			break;
		case 8:
			(*this).setGamePieceProperties(8, false, true);
			break;
		case 9:
			(*this).setGamePieceProperties(9, false, false);
			break;
		default:
			break;
		}
	}

	int  getId() const							{ return id; }
	bool getAmphibian() const					{ return isAmphibian; }
	bool getForestRanger() const				{ return isForestRanger; }
	int  getRow() const							{ return pos.row; }
	int	 getColumn() const						{ return pos.col; }
	BoardPosition getPos() const				{ return pos; }
	bool getIsAlive() const						{ return isAlive; }
	void setId(int _id)							{ id = _id; }
	void setAmphibian(bool _isAmphibian)		{ isAmphibian = _isAmphibian; }
	void setForestRanger(bool _isForestRanger)	{ isForestRanger = _isForestRanger; }
	void setDead()								{ isAlive = false; }
	void setAlive()								{ isAlive = true; }
	void setBoardPos(BoardPosition boardPos)	{ pos = boardPos; }
	BoardPosition getBoardPos() const			{ return pos; }

	int	 getOpponentSide() const {
		if (this->getSide() == 0)
			return 1;
		return 0;
	}

	//return the side (=player) to whom the game piece belongs
	int getSide() const {
		if (id < 4)
			return 0;
		return 1;
	}

	//returns -1 if input is incorrect and 1 if row and column were updated successfully 
	int setRowColumn(int _row, int _column) {
		if (12 < _row || 12 < _column)
			return -1;
		pos.row = _row;
		pos.col = _column;
		return 1;
	}

	//returns -1 if input is incorrect and 1 if game piece was updated successfully 
	int setGamePieceProperties(int _id, bool _isAmphibian, bool _isForestRanger) {
		if ((4 > _id && 0 < _id) || (10 > _id && 6 < _id)) {
			id = _id;
			isAmphibian = _isAmphibian;
			isForestRanger = _isForestRanger;
			return 1;
		}
		return -1;
	}

	void setGamePiecePropertiesById(int id);

	friend GamePiece * fight(GamePiece * gPiece1, GamePiece * gPiece2, int row, int column);

	friend GamePiece * fightAux(GamePiece * gPieceA, GamePiece * gPieceB, int row, int column);
};
