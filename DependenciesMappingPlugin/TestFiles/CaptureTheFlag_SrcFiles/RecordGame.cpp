/* Test file for Dependencies Mapping Plugin -
 RecordGame.cpp */

#include "Game.h"

bool Game::writeBoardToFile() {
	string recordBoardFileName_full, recordBoardFileName_short;
	ofstream newBoardFile;
	Surface surface;
	GamePiece * gamePiece;

	boardFileName = generateNewRandomFileName();
	recordBoardFileName_full = cmdConfig.getPath() + "\\" + boardFileName + "." + BOARD_FILE_TYPE;
	newBoardFile.open(recordBoardFileName_full.c_str(), ios_base::out);

	if (newBoardFile.is_open()) {
		for (int row = 0; row < NUM_OF_ROWS_OR_COLUMNS; ++row) {
			for (int col = 0; col < NUM_OF_ROWS_OR_COLUMNS; ++col) {
				surface = board.getTile(row, col).getSurfaceType();
				gamePiece = board.getTile(row, col).getGamePieceOnTile();
				if (gamePiece)
					newBoardFile << gamePiece->getId();
				else {
					switch (surface) {
					case Surface::FLAGA:
						newBoardFile.put('A');
						break;
					case Surface::FLAGB:
						newBoardFile.put('B');
						break;
					case Surface::FR:
						newBoardFile.put('T');
						break;
					case Surface::SEA:
						newBoardFile.put('S');
						break;
					case Surface::REGULAR:
						newBoardFile.put(' ');
						break;
					default:
						newBoardFile.put(' ');
						break;
					}
				}
			}
			newBoardFile.put('\n');
		}
		newBoardFile.close();
		return true;
	}
	cout << "Failed to record board to file" << endl;
	system("pause");
	return false;
}


bool Game::recordMovesToFiles(queue<GameMove_Parsed> & moves) const {
	string fileName_A, fileName_B;
	ofstream movesFile_A, movesFile_B;
	bool res = true;

	fileName_A = cmdConfig.getPath() + "\\" + boardFileName + "." + MOVES_A_FILE_TYPE;
	fileName_B = cmdConfig.getPath() + "\\" + boardFileName + "." + MOVES_B_FILE_TYPE;

	movesFile_A.open(fileName_A.c_str(), ios_base::out);
	movesFile_B.open(fileName_B.c_str(), ios_base::out);

	if (movesFile_A.is_open() && movesFile_B.is_open())
		writeMovesToFiles(moves, movesFile_A, movesFile_B);
	else {
		cout << "Failed to record moves to file" << endl;
		system("pause");
		res = false;
	}

	if (movesFile_A.is_open())
		movesFile_A.close();
	if (movesFile_B.is_open())
		movesFile_B.close();

	return res;
}

void Game::writeMovesToFiles(queue<GameMove_Parsed> & moves, ofstream & outMovesFile_A, ofstream & outMovesFile_B) const {
	while (!moves.empty()) {
		GameMove_Parsed currMove = moves.front();
		writeSingleMoveToFile(currMove, outMovesFile_A, outMovesFile_B);
		moves.pop();
	}
}

void Game::writeSingleMoveToFile(GameMove_Parsed & move, ofstream & outMovesFile_A, ofstream & outMovesFile_B) const {
	string moveLine;
	long iteration;
	int gamePiece;
	char direction;

	iteration = move.getGIteration();
	gamePiece = move.getGPiece();
	switch (move.getDir()) {
	case Directions::UP:
		direction = 'U';
		break;
	case Directions::DOWN:
		direction = 'D';
		break;
	case Directions::LEFT:
		direction = 'L';
		break;
	case Directions::RIGHT:
		direction = 'R';
		break;
	case Directions::STOP:
		break;
	}

	if (iteration % 2 == 1)
		outMovesFile_A << iteration << "," << gamePiece << "," << direction << "\n";
	else
		outMovesFile_B << iteration << "," << gamePiece << "," << direction << "\n";
}
