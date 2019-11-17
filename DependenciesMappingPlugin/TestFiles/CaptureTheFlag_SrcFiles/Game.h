/* Test file for Dependencies Mapping Plugin -
 Game.h */

#pragma once
#include "Player.h"
#include "PlayerData.h"
#include "Board.h"
#include <stdlib.h> //exit()
#include "GameOptions.h"
#include "GamePiece.h"
#include "Directions.h"
#include <typeinfo>
#include <iostream>
#include <fstream>
#include "Utils.h"
#include <queue>
#include <cstdio>
#include <string>
#include "CmdConfig.h"
#include "GameMove.h"
using namespace std;



#define BOARD_FILE_TYPE "gboard"
#define RANDOM_BOARD_FILE_PREFIX "random_"

class Game {
	enum { ESC = 27 };
	static int counter;
	GameBoard board;
	PlayerData players[NUM_OF_PLAYERS];
	string boardFileName;
	CmdConfig cmdConfig;
	queue<string> boardFiles;

public:
	Game(int argc, char ** argv) : players { PlayerData(&board, 0), PlayerData(&board, 1) } {
		boardFileName = (string)RANDOM_BOARD_FILE_PREFIX + "0";
		cmdConfig.parseCmdConfig(argc, argv);
		createPlayers();
	};

	~Game() { 
		delete players[0].player;
		delete players[1].player;
	}

	void gameManager();

private:

	void createPlayers() {
		char ch = cmdConfig.getMovesSource();
		if (ch == 'k') {
			players[0].player = new KeyboardPlayer(0);
			players[1].player = new KeyboardPlayer(1);
		}
		else if (ch == 'f') {
			players[0].player = new FilePlayer(0);
			players[1].player = new FilePlayer(1);
			initFilePlayers();
		}
		else {
			players[0].player = new AlgoPlayer(0);
			players[1].player = new AlgoPlayer(1);
		}

		players[0].player->init(players[0].playerBoard);
		players[1].player->init(players[1].playerBoard);
	}

	void initFilePlayers() {
		if (cmdConfig.isBoardFileInUse()) {
			((FilePlayer*)(players[0].player))->setBoardFile(cmdConfig.getPath(), boardFileName, true);
			((FilePlayer*)(players[1].player))->setBoardFile(cmdConfig.getPath(), boardFileName, true);
		}
		else {
			((FilePlayer*)(players[0].player))->setBoardFile(cmdConfig.getPath(), "", false);
			((FilePlayer*)(players[1].player))->setBoardFile(cmdConfig.getPath(), "", false);
		}
	}

    GameOptions mainMenu(bool & playersSwapped);

	void receiveNamesFromPlayers();

	void swapPlayers();

	bool areAvailableMovesToContinueGame() const;

	bool isBoardAvailable() const ;

	bool noAvailableMoves() const;

	void printMatchEndBanner(GeneralPlayer * const winner, const int numSteps) const;

	bool setBoardForNewGame_file();

	GameOptions escMenu();

	void setBoardForNewGame_random();

	void displayBoard();

	bool loadBoard();

	GameMove * turn(const int side, bool & moveOccurred, GameMove * opponentsMove);

	bool move(GameMove & gameMove);

	void twoGamePiecesFace2Face(GamePiece * const movingGamePiece, const GameMove & gameMove, GamePiece * const gamePieceInTile, bool & moveOccurred);

	void printScoreBelowBoard() const;

	void getFilesInDir();

	string generateNewRandomFileName() const;

	bool writeBoardToFile();

	void writeMovesToFiles(queue<GameMove_Parsed> & moves, ofstream & outMovesFile_A, ofstream & outMovesFile_B) const;

	void writeSingleMoveToFile(GameMove_Parsed & move, ofstream & outMovesFile_A, ofstream & outMovesFile_B) const;

	bool recordMovesToFiles(queue<GameMove_Parsed> & moves) const;

	int getNumberOfRandomFile(const string fileName) const;

	GameOptions playSingleRound();

	bool isGameOver(GeneralPlayer ** const winningPlayer) const;

	bool isLegalGameMove(const int side, const GameMove gameMove);

	GameMove_Parsed parseCurrGameMove(const int side) const;

	Directions getDirectionOfLegalGameMove(const GameMove & gameMove) const;

	GamePiece * getLosingGamePiece(GamePiece * const gp1, GamePiece * const gp2, GamePiece * winningGamePiece);

	void updateGamePieceMove(GamePiece * const movingGamePiece, const GameMove & gameMove);

	void removeGamePieceFromTile(GamePiece * const movingGamePiece, const GameMove & gameMove);

	void putGamePieceOnTile(GamePiece * const movingGamePiece, const GameMove & gameMove);

	void updateLosingPlayerWithFightResults(GamePiece * const losingGamePiece);

	bool areAvailableMovesForNextRound() const;

	void destSurfaceIsRegular(GamePiece * const movingGamePiece, const GameMove & gameMove, GamePiece * const gamePieceInDest, bool & isMovePossible);

	void destSurfaceIsSea(GamePiece * const movingGamePiece, const GameMove & gameMove, GamePiece * const gamePieceInDest, bool & isMovePossible);

	void destSurfaceIsForest(GamePiece * const movingGamePiece, const GameMove & gameMove, GamePiece * const gamePieceInDest, bool & isMovePossible);

	void destSurfaceIsFlagA(GamePiece * const movingGamePiece, const GameMove & gameMove, bool & isMovePossible);
	
	void destSurfaceIsFlagB(GamePiece * const movingGamePiece, const GameMove & gameMove, bool & isMovePossible);
	
	void printGameEndBanner() const;

	void updateBoardWithFightResults(GamePiece * const movingGamePiece, GamePiece * const winningGamePiece, GamePiece * const losingGamePiece, const GameMove & gameMove);
	
	bool isGameMoveChanged(GameMove_Parsed & currGameMove, GameMove_Parsed & prevGameMoveA, GameMove_Parsed & prevGameMoveB, const int side) const;
	
	void resetGame();

	GameOptions stopGameConditon(const long iteration) ;

	bool setNewGame(GameOptions & gameState);

	void resetPlayerData();
};
