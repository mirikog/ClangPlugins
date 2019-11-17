/* Test file for Dependencies Mapping Plugin -
 Player.h */

#pragma once
#include <iostream>
#include <string>
#include <queue>
#include "Board.h"
#include "GameMove.h"
#include "GameMove.h"
#include "Directions.h"
#include "GameOptions.h"
#include "BoardPosition.h"
#include <typeinfo>
#include "GameOptions.h"
#include "Orbit.h"


using namespace std;

#define A_UP	'w'
#define A_DOWN	'x'
#define A_LEFT	'a'
#define A_RIGHT 'd'
#define B_UP	'i'
#define B_DOWN	'm'
#define B_LEFT	'j'
#define B_RIGHT	'l'
#define MOVES_A_FILE_TYPE "moves-a_small"
#define MOVES_B_FILE_TYPE "moves-b_small"
#define NUM_OF_PIECES_PER_PLAYER 3

class AbstractPlayer {
public:
	virtual ~AbstractPlayer() { }
	/* player: 1 for 1-2-3 player, 2 for 7-8-9 */
	virtual void setPlayer(int player) = 0;
	virtual void init(const BoardData& board) = 0;
	virtual GameMove play(const GameMove& opponentsMove) = 0;
	virtual string getName() const = 0;
};

class GeneralPlayer : public AbstractPlayer {
	string _name;
	unsigned int score;
	int playerIndex;
	GamePiece gamePieces[NUM_OF_PIECES_PER_PLAYER];
	GamePiece * currGamePiece;
	Directions currDirection;
	const BoardData * _board;
	Flag flag_self;
	Flag flag_opponent;

public:
	
	GeneralPlayer(const int player) {
		playerIndex = player;
		_name = player == 0 ? "A" : "B";
		score = 0;
		initGamePiecesProperties();
		currGamePiece = nullptr;
		_board = nullptr;
		currDirection = Directions::STOP;
	}

	GeneralPlayer() {
		playerIndex = 0;
		_name = "";
		score = 0;
		currGamePiece = nullptr;
		_board = nullptr;
		currDirection = Directions::STOP;
	}

	~GeneralPlayer() {};

	void clearScore()										{ score = 0; }
	BoardPosition getOpponentsFlagPos() const				{ return flag_opponent.getPos(); }
	GamePiece * getGamePieceByInternalIndex(int index)		{ return &gamePieces[index]; }
	Directions getCurrDirection() const						{ return currDirection; }
	GamePiece * getCurrGamePiece() const					{ return currGamePiece; }
	int getPlayerIndex() const								{ return playerIndex; }
	void setScore(const int _score)							{ score = _score; }
	void setCurrGamePiece(GamePiece * const gamePiece)		{ currGamePiece = gamePiece; }
	void setCurrDirection(const Directions dir)				{ currDirection = dir; }
	void resetScore()										{ score = 0; }
	unsigned int getScore() const							{ return score; }
	void addPointToScore()									{ score++; }
	void setName(const string name)							{ _name = name; }
	Flag & getFlag_self()									{ return flag_self; }
	Flag & getFlagOpponent()								{ return flag_opponent; }

	GamePiece * getGamePieceById_self(const int id);

	void resetPlayer();

	virtual void setPlayerForNewGame(int playerIndex);

	void updatePlayerPrevMove();

	char getCharFromBoard(int x, int y) const;
	
	virtual void init(const BoardData& board)	{ _board = &board; }
	virtual string getName() const				{ return _name; }
	virtual void setPlayer(int player)			{ playerIndex = player; }
	virtual GameMove play(const GameMove& opponentsMove);
	virtual void setPlayerNextMove(const GameMove& opponentsMove) = 0;

private:

	void resetGamePiecesLives();

	void resetGamePiecesAndFlagsLocation_self();

	virtual void initGamePiecesProperties();

	void initGamePiecesAndFlagsLocation();

	BoardPosition getPositionOfDestMove() const;

	void currGamePieceMovedOnGameBoard();

	void selfGamePieceOnDestTile(char gamePieceOnDestTile, BoardPosition destTile);

	void opponentGamePieceOnDestTile();

	BoardPosition getBoardPositionInDirection(BoardPosition from, Directions direction) const;

	bool isLegalBoardPos(const BoardPosition pos) const;
};

class KeyboardPlayer : public GeneralPlayer {
	queue<char> inputStream;
	GamePiece * nextGamePiece;

public:
	KeyboardPlayer(const int player) : GeneralPlayer(player) { 
		nextGamePiece = nullptr;
	}

	void setNextGamePiece(GamePiece * gp) { nextGamePiece = gp; }

	virtual void setPlayerNextMove(const GameMove& opponentsMove);

private:

	void getInputFromConsole();
};

class FilePlayer : public GeneralPlayer {
	queue<string> files;
	queue<GameMove_Parsed> moves;
	bool isBoardFile = false;
	string currBoardFileName = "";
	string path = "";
	ifstream currMovesFile;
	long int iteration;

public:
	FilePlayer(int player) : GeneralPlayer(player) {
		iteration = getPlayerIndex() == 0 ? 1 : 2;
	}

	void addFileNameToQueue(const string fileName)			{ files.push(fileName); }
	bool isFilesQueueEmpty() const							{ return files.empty(); }
	void setCurrBoardFileName(const string boardFileName)	{ currBoardFileName = boardFileName; }
	bool noAvailableMoves() const							{ return moves.empty(); }
	
	void setBoardFile(const string _path, const string boardFileName, const bool isBoardFileInUse) {
		isBoardFile = isBoardFileInUse;
		currBoardFileName = boardFileName;
		path = _path;
	}

	void resetIteration() {
		iteration = getPlayerIndex() == 0 ? 1 : 2;
	}

	bool areAvailableMoves() {
		bool res = loadMovesFromFile();
		return res;
	}

	virtual void setPlayerNextMove(const GameMove& opponentsMove);

private:

	bool loadMovesFromFile();

	bool openMovesFile();

	bool readMovesFromFile();

	bool getIterationFromFile(int & iteration);

	bool getGamePieceFromFile(int & gamePiece);

	bool getDirectionFromFile(Directions & direction);

};

class AlgoPlayer : public GeneralPlayer {
	GamePiece gamePieces_opponent[NUM_OF_PIECES_PER_PLAYER];
	char cBoard[NUM_OF_ROWS_OR_COLUMNS][NUM_OF_ROWS_OR_COLUMNS];
	int distanceMap_flagSelf[NUM_OF_PIECES_PER_PLAYER][NUM_OF_ROWS_OR_COLUMNS][NUM_OF_ROWS_OR_COLUMNS];
	int distanceMap_flagOpponent[NUM_OF_PIECES_PER_PLAYER][NUM_OF_ROWS_OR_COLUMNS][NUM_OF_ROWS_OR_COLUMNS];

public:
	AlgoPlayer() { }
	
	AlgoPlayer(int player) : GeneralPlayer(player) { }

	void setPlayerForNewGame(int playerIndex) override;
	void initGamePiecesProperties() override;
	void initGameLocations();
	void initDistanceMaps();


	void updateOpponentsGamePiece(const GameMove & opponentsMove);
	void setSeaOnDistanceMap(int permissionArr_self[NUM_OF_PIECES_PER_PLAYER], int permissionArr_opponent[NUM_OF_PIECES_PER_PLAYER]);
	void setForestOnDistanceMap(int permissionArr_self[NUM_OF_PIECES_PER_PLAYER], int permissionArr_opponent[NUM_OF_PIECES_PER_PLAYER]);
	void setOpponentFlagOnDistanceMap(int permissionArr_self[NUM_OF_PIECES_PER_PLAYER], int permissionArr_opponent[NUM_OF_PIECES_PER_PLAYER]);
	void setSelfFlagOnDistanceMap(int permissionArr_self[NUM_OF_PIECES_PER_PLAYER], int permissionArr_opponent[NUM_OF_PIECES_PER_PLAYER]);
	void distanceMapCalcLauncher();
	bool isLegalCalcStep(int * map, BoardPosition dest);
	void clacDistanceMap(int * map, BoardPosition dest, int step);
	void gpClosestToEnemyFlag();
	void advanceOffence();


	virtual void setPlayerNextMove(const GameMove& opponentsMove) override;
	
};

