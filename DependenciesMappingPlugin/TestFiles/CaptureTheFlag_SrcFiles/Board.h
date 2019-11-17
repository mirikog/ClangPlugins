/* Test file for Dependencies Mapping Plugin -
   Board.h */

#pragma once
#include "Tile.h"
#include "Directions.h"
#include "GamePiece.h"
#include "Flag.h"
#include <fstream>
#include <iostream>
#include <vector>
#include "Utils.h"
#include "BoardPosition.h"
#include "BoardFileData.h"
#include <string>
#include "BoardData.h"
using namespace std;

class GamePiece;

class GameBoard : public BoardData {
	// double array of tiles 13*13
	Tile tiles[NUM_OF_ROWS_OR_COLUMNS][NUM_OF_ROWS_OR_COLUMNS];
	GamePiece gamePieces[NUM_OF_PLAYERS][NUM_OF_PIECES_PER_PLAYER];
	int arr[2];
	Directions currDirections[NUM_OF_PLAYERS];
	GamePiece * currGamePieces[NUM_OF_PLAYERS] = { nullptr, nullptr };
	Flag flags[NUM_OF_PLAYERS];
	

	virtual char charAt(int x, int y) const	{ return CHAR_OF_OPPONENTS_GAME_PIECE; }
	
public:
	GameBoard() {}
	~GameBoard() {}
	
	BoardPosition getFlagPosition(const int player)	const			{ return flags[player].getPos(); }
	Tile & getTile(const int row, const int column)					{ return tiles[row][column]; }
	Directions getPlayerCurrDirection(const int playerSide) const	{ return currDirections[playerSide]; }
	GamePiece * getPlayerCurrGamePiece(const int playerSide)		{ return currGamePieces[playerSide]; }


	int getPlayerCurrGamePieceId(const int playerSide) const { 
		if (currGamePieces[playerSide])
			return currGamePieces[playerSide]->getId();
		return 0;
	}

	void setPlayerCurrGamePiece(const int playerSide, const int gamePieceId) {
		if (gamePieceId == 0)
			currGamePieces[playerSide] = nullptr;
		else {
			if (playerSide == 0)
				currGamePieces[0] = &gamePieces[0][gamePieceId - 1];
			else
				currGamePieces[1] = &gamePieces[1][gamePieceId - 7];
		}
	}

	void setPlayerCurrDirection(const int playerSide, Directions dir) {
		currDirections[playerSide] = dir;
	}

	BoardPosition getOpponentsFlagPosition(const int player) const	{ 
		if (player == 0)
			return flags[1].getPos();
		return flags[0].getPos();
	}

	bool isFlagCaptured(int & winningPlayer) const;

	bool isPlayerWithAllGamePiecesDead(int & winningPlayer) const ;

	void initBoardSurfaces_random();

	void initGamePiecesAndFlags_random();

	void resetBoard();

	void drawBoard() ;

	void drawTile(const int row, const int column);

	bool readBoardFromFile(const char * fileName, const string path);

	void setGamePieceDead(int gamePieceId);

private:

	void draw(const int x, const int y, const char c) const;

	void drawFlag(const int row, const int column, char flag);

	void drawTileBackground(const int row, const int column);

	void drawBoardOutline() const;

	void assignTileToFlag(const char ch);

	void assignTileToGP(const int id);

	void updateBoardFromFile(const char ch, const int row, const int col, BoardFileData & boardFileData);

	void updateGamePieceFromFile(const char ch, const int row, const int col, BoardFileData & boardFileData);

	void updateFlagFromFile(const char ch, const int row, const int col, BoardFileData & boardFileData);

	bool errorsInBoardFile(BoardFileData & boardFileData) const;

	void printErrorsFromReadingBoardFile(const char * fileName, const BoardFileData & boardFileData) const;

	void resetGamePiecesAndFlags();
};

class PlayerBoard : public GameBoard {
	GameBoard * gameBoardPtr;
	int playerIndex;

public:
	PlayerBoard() {
		gameBoardPtr = nullptr;
		playerIndex = 0;
	}

	PlayerBoard(GameBoard * boardPtr, int player) { 
		gameBoardPtr = boardPtr;
		playerIndex = player;
	}

	virtual char charAt(int x, int y) const;

private:
    char getGamePieceChar(const int gamePieceId) const;
};

