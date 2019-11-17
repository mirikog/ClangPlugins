/* Test file for Dependencies Mapping Plugin -
 Board.cpp */

#include "Board.h"

void GameBoard::drawTile(const int row, const int column) {
	Tile & tile = tiles[row][column];

	drawTileBackground(row, column);

	if (nullptr != tile.getGamePieceOnTile())
	{
		draw(column * 3 + 4, row + 2, '1' - 1 + tile.getGamePieceOnTile()->getId());
		return;
	}
	else if (tile.getSurfaceType() == Surface::FLAGA)
	{
		drawFlag(row, column, 'A');
		return;
	}
	else if (tile.getSurfaceType() == Surface::FLAGB)
	{
		drawFlag(row, column, 'B');
		return;
	}

}

void GameBoard::draw(const int x, const int y, const char c) const {
	cout << c;
	// make sure it gets to screen on time
	cout.flush();
}

void GameBoard::drawFlag(const int row, const int column, char flag)
{

	draw(column * 3 + 4, row + 2, (char)247); // ascii #247 = '?'

	if ('A' == flag)
	{
		draw(column * 3 + 5, row + 2, 'A');
	}
	else
	{
		draw(column * 3 + 5, row + 2, 'B');
	}
}

void GameBoard::drawTileBackground(const int row, const int column)
{
	for (int i = 0; i < 3; i++)
	{
		draw((column + 1) * 3 + i, row + 2, ' ');
	}
}

void GameBoard::drawBoardOutline() const {
	clearScreen();

	for (int i = 0; i < 13; i++)
	{
		draw((3 * i) + 4, 0, 'A' + i);		// Draw A,B,C,D,...,M in row 0

		if (i < 9)
		{
			draw(0, 2 + i, '1' + i);	// Draw 1,2,3,4,...,9 in column 0
		}
		else
		{
			draw(0, 2 + i, '1');
			draw(1, 2 + i, '1' + i - 10);	// Draw 10,....,13
		}
	}

	for (int i = 0; i < 3 * 13; i++)
	{
		draw(3 + i, 1, '_');
		draw(3 + i, 15, (char)196);
	}

	for (int i = 0; i < NUM_OF_ROWS_OR_COLUMNS; i++)
	{
		draw(3 * 14, i + 2, (char)179);
		draw(2, i + 2, (char)179);
	}
}

void GameBoard::drawBoard()
{
	clearScreen();
	drawBoardOutline();
	for (int i = 0; i < NUM_OF_ROWS_OR_COLUMNS; i++)
	{
		for (int j = 0; j < NUM_OF_ROWS_OR_COLUMNS; j++)
		{
			drawTile(i, j);
		}
	}
}

void GameBoard::resetBoard() {
	resetGamePiecesAndFlags();

	for (int row = 0; row < NUM_OF_ROWS_OR_COLUMNS; ++row)
		for (int col = 0; col < NUM_OF_ROWS_OR_COLUMNS; ++col) {
			tiles[row][col].setTileType(Surface::REGULAR);
			tiles[row][col].setGamePiece(nullptr);
		}
}

void GameBoard::resetGamePiecesAndFlags() {
	
	for (int i = 0; i < 3; i++)
	{
		gamePieces[0][i].setId(0);
		gamePieces[0][i].setRowColumn(0, 0);
		gamePieces[0][i].setAlive();
		gamePieces[1][i].setId(0);
		gamePieces[1][i].setRowColumn(0, 0);
		gamePieces[1][i].setAlive();
	}
	flags[0].setRowCol(0, 0);
	flags[1].setRowCol(0, 0);
}

void GameBoard::initBoardSurfaces_random()
{
	// tiles[row][column]
	// set fr tiles	
	tiles[6]['B' - 'A'].setTileType(Surface::FR);
	for (int i = 6; i<8; i++) { tiles[i]['A' - 'A'].setTileType(Surface::FR); }
	for (int i = 3; i<7; i++) { tiles[i]['C' - 'A'].setTileType(Surface::FR); }
	for (int i = 5; i<9; i++) { tiles[i]['D' - 'A'].setTileType(Surface::FR); }

	// set SEA tiles
	tiles[5]['H' - 'A'].setTileType(Surface::SEA);
	tiles[7]['L' - 'A'].setTileType(Surface::SEA);
	for (int i = 4; i<6; i++) { tiles[i]['I' - 'A'].setTileType(Surface::SEA); }
	for (int i = 3; i<10; i++) { tiles[i]['J' - 'A'].setTileType(Surface::SEA); }
	for (int i = 6; i<9; i++) { tiles[i]['K' - 'A'].setTileType(Surface::SEA); }

	return;
}

void GameBoard::initGamePiecesAndFlags_random() {

	for (int i = 0; i < NUM_OF_PLAYERS; ++i) {
		assignTileToFlag('A' + i);
		int startIndexOfGamePieces = i == 0 ? 1 : 7;
		for (int j = 0; j < NUM_OF_PIECES_PER_PLAYER; ++j) {
			assignTileToGP(startIndexOfGamePieces + j);
			gamePieces[i][j].setProperies(startIndexOfGamePieces + j);
		}
	}

}

void GameBoard::assignTileToFlag(char ch) {
	int colFlag = 0, rowFlag = 0, rowsRangeStart = (ch - 'A') * 10;

	// Generate random row and column
	rowFlag = (rand() % 3) + rowsRangeStart;
	colFlag = rand() % NUM_OF_ROWS_OR_COLUMNS;
	if ('A' == ch) {
		// Set flagA
		tiles[rowFlag][colFlag].setTileType(Surface::FLAGA);
		flags[0].setRowCol(rowFlag, colFlag);
	}
	else {
		// Set flagB
		tiles[rowFlag][colFlag].setTileType(Surface::FLAGB);
		flags[1].setRowCol(rowFlag, colFlag);
	}
	return;
}

void GameBoard::assignTileToGP(int id)
{
	bool isTileEmpty = false;
	int colOption = 0, rowOption = 0, rowsRangeStart = id < 4 ? 0 : 8; 
	Tile currTile;

	// Randomly select a tile until an empty one is found
	while (!isTileEmpty)
	{
		// Generate random row and column
		rowOption = (rand() % 5) + rowsRangeStart;
		colOption = rand() % NUM_OF_ROWS_OR_COLUMNS;

		// Check if tile is empty
		if (nullptr == tiles[rowOption][colOption].getGamePieceOnTile() &&
			Surface::REGULAR == tiles[rowOption][colOption].getSurfaceType())
		{
			// Exit loop - an empty tile was found
			isTileEmpty = true;
		}
	}

	// Yay! tile is empty
	// Assign GamePiece 1,2,3 to Tile
		if (id < 4)
		{
			gamePieces[0][id - 1].setRowColumn(rowOption, colOption);
			tiles[rowOption][colOption].setGamePiece(&gamePieces[0][id - 1]);
		}
	// Or assign GamePiece 7,8,9 to Tile	
		else
		{
			gamePieces[1][id - 7].setRowColumn(rowOption, colOption);
			tiles[rowOption][colOption].setGamePiece(&gamePieces[1][id - 7]);
		}
}

bool GameBoard::isFlagCaptured(int & winningPlayer) const {

	for (int i = 0; i < NUM_OF_PLAYERS; ++i) {
		BoardPosition flagOpponent = getOpponentsFlagPosition(i);
		for (int j = 0; j < NUM_OF_PIECES_PER_PLAYER; ++j) {
			if (gamePieces[i][j].getRow() == flagOpponent.row && gamePieces[i][j].getColumn() == flagOpponent.col) {
				winningPlayer = i == 0 ? 0 : 1;
				return true;
			}
		}
	}

	winningPlayer = NUM_OF_PLAYERS;
	return false;
}

bool GameBoard::isPlayerWithAllGamePiecesDead(int & winningPlayer) const {

	for (int i = 0; i < NUM_OF_PLAYERS; ++i) {
		if (!gamePieces[i][0].getIsAlive() && !gamePieces[i][1].getIsAlive() && !gamePieces[i][2].getIsAlive()) {
			winningPlayer = i == 0 ? 1 : 0;
			return true;
		}
	}

	winningPlayer = NUM_OF_PLAYERS;
	return false;
}

void GameBoard::setGamePieceDead(int gamePieceId) {
	if (gamePieceId < 4)
		gamePieces[0][gamePieceId - 1].setDead();
	else
		gamePieces[1][gamePieceId - 7].setDead();
}

//assuming x,y start from 0: y satands for row number, x stands for col number
char PlayerBoard::charAt(int x, int y) const {
	Surface surfaceOnTile;
	GamePiece * gamePieceOnTile = nullptr;

	if (x < 1 || x > NUM_OF_ROWS_OR_COLUMNS || y < 1 || y > NUM_OF_ROWS_OR_COLUMNS)
		return POS_OUT_OF_BOARD_BOUNDS;
	else {
		if (gameBoardPtr) {
			gamePieceOnTile = gameBoardPtr->getTile(y - 1, x - 1).getGamePieceOnTile();
			surfaceOnTile = gameBoardPtr->getTile(y - 1, x - 1).getSurfaceType();
		}

		if (gamePieceOnTile) {
			int gamePieceId = gamePieceOnTile->getId();
			return getGamePieceChar(gamePieceId);
		}

		switch (surfaceOnTile) {
		case Surface::SEA:
			return 'S';
			break;
		case Surface::FR:
			return 'T';
			break;
		case Surface::FLAGA:
			return 'A';
			break;
		case Surface::FLAGB:
			return 'B';
			break;
		case Surface::REGULAR:
			return ' ';
			break;
		default:
			break;
		}
	}
	return POS_OUT_OF_BOARD_BOUNDS;
}

char PlayerBoard::getGamePieceChar(int gamePieceId) const {
	if (gamePieceId < 4) {
		if (playerIndex == 0)
			return '0' + gamePieceId;
		else
			return CHAR_OF_OPPONENTS_GAME_PIECE;
	}
	else {
		if (playerIndex == 0)
			return CHAR_OF_OPPONENTS_GAME_PIECE;
		else
			return '0' + gamePieceId;
	}
}
