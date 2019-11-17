/* Test file for Dependencies Mapping Plugin -
 ReadBoardFromFile.cpp */

#include "Board.h"

bool GameBoard::readBoardFromFile(const char * fileName, const string path) {
	char ch;
	string buffer, fullFileName;
	BoardFileData boardFileData;

	fullFileName = path + "\\" + fileName;

	fstream boardFile;
	boardFile.open(fullFileName.c_str(), ios_base::in);

	if (boardFile.is_open()) {
		for (int row = 0; row < NUM_OF_ROWS_OR_COLUMNS && !boardFile.eof(); row++) {
			for (int col = 0; col < NUM_OF_ROWS_OR_COLUMNS && !boardFile.eof(); col++) {
				if (!boardFile.eof()) {
					boardFile.get(ch);
					updateBoardFromFile(ch, row, col, boardFileData);
				}
			}
			getline(boardFile, buffer);
		}

		if (errorsInBoardFile(boardFileData)) {
			printErrorsFromReadingBoardFile(fileName, boardFileData);
			return false;
		}
		boardFile.close();
	}

	return true;
}

void GameBoard::updateBoardFromFile(char ch, int row, int col, BoardFileData & tileData) {
	if (A_GP_1 == ch || A_GP_2 == ch || A_GP_3 == ch || B_GP_7 == ch || B_GP_8 == ch || B_GP_9 == ch)
		updateGamePieceFromFile(ch, row, col, tileData);
	else {
		switch (ch) {
		case ('A'):
			updateFlagFromFile(ch, row, col, tileData);
			break;
		case ('B'):
			updateFlagFromFile(ch, row, col, tileData);
			break;
		case ('S'):
			tiles[row][col].setTileType(Surface::SEA);
			break;
		case ('T'):
			tiles[row][col].setTileType(Surface::FR);
			break;
		case (' '):
			tiles[row][col].setTileType(Surface::REGULAR);
			break;
		default:
			bool charAppeared = false;
			for (unsigned int i = 0; i < tileData.illegalChars.size(); i++)
				if (tileData.illegalChars[i] == ch)
					charAppeared = true;
			if (!charAppeared)
				tileData.illegalChars.push_back(ch);
			break;
		}
	}
}

void GameBoard::updateGamePieceFromFile(char ch, int row, int col, BoardFileData & tileData) {
	int id = ch - '0';
	int side = id < 4 ? 0 : 1;
	int i = id < 4 ? ch - '0' - 1 : ch - '0' - 7;

	if (tileData.gamePieces[side][i] != 0)
		tileData.isError_player[side] = true;
	else {
		tileData.gamePieces[side][i] = ch - '0';

		gamePieces[side][i].setRowColumn(row, col);
		tiles[row][col].setTileType(Surface::REGULAR);
		tiles[row][col].setGamePiece(&gamePieces[side][i]);
		switch (id) {
		case (1):
			gamePieces[side][i].setGamePieceProperties(1, false, false);
			break;
		case (2):
			gamePieces[side][i].setGamePieceProperties(2, true, true);
			break;
		case (3):
			gamePieces[side][i].setGamePieceProperties(3, true, false);
			break;
		case (7):
			gamePieces[side][i].setGamePieceProperties(7, true, true);
			break;
		case (8):
			gamePieces[side][i].setGamePieceProperties(8, false, true);
			break;
		case (9):
			gamePieces[side][i].setGamePieceProperties(9, false, false);
			break;
		default:
			break;
		}
	}
}

void GameBoard::updateFlagFromFile(char ch, int row, int col, BoardFileData & boardFileData) {
	int side = ch - 'A';
	Surface flag = side == 0 ? Surface::FLAGA : Surface::FLAGB;

	if (boardFileData.flags[side] == 0) {
		tiles[row][col].setTileType(flag);
		flags[side].setRowCol(row, col);
		boardFileData.flags[side] = NUM_OF_PLAYERS;
	}
	else
		boardFileData.isError_player[side] = true;
}

bool GameBoard::errorsInBoardFile(BoardFileData & boardFileData) const {
	if (!boardFileData.isError_player[0]) {
		if (boardFileData.gamePieces[0][0] == 0 || boardFileData.gamePieces[0][1] == 0 || boardFileData.gamePieces[0][2] == 0 || boardFileData.flags[0] == 0)
			boardFileData.isError_player[0] = true;
	}
	if (!boardFileData.isError_player[1]) {
		if (boardFileData.gamePieces[1][0] == 0 || boardFileData.gamePieces[1][1] == 0 || boardFileData.gamePieces[1][2] == 0 || boardFileData.flags[1] == 0)
			boardFileData.isError_player[1] = true;
	}
	if (boardFileData.isError_player[0] || boardFileData.isError_player[1] || boardFileData.illegalChars.size() > 0)
		return true;
	return false;
}

void GameBoard::printErrorsFromReadingBoardFile(const char * fileName, const BoardFileData & boardFileData) const {
	clearScreen();
	if (boardFileData.isError_player[0])
		cout << "Wrong settings for player A tools in file " << fileName << endl;
	if (boardFileData.isError_player[1])
		cout << "Wrong settings for player B tools in file " << fileName << endl;
	for (unsigned int i = 0; i < boardFileData.illegalChars.size(); i++)
		cout << "Wrong character on board : " << boardFileData.illegalChars[i] << " in file " << fileName << endl;
	system("pause");
	return;
}
