/* Test file for Dependencies Mapping Plugin -
 Player.cpp */

#include "Player.h"

void GeneralPlayer :: resetPlayer() {
	resetGamePiecesAndFlagsLocation_self();
	resetGamePiecesLives();
	currGamePiece = nullptr;
	currDirection = Directions::STOP;
	if (typeid(*this) == typeid(KeyboardPlayer))
		(*(KeyboardPlayer*)(this)).setNextGamePiece(nullptr);
	if (typeid(*this) == typeid(FilePlayer))
		((FilePlayer*)(this))->resetIteration();
}

void GeneralPlayer::resetGamePiecesLives() {
	for (int i = 0; i < NUM_OF_PIECES_PER_PLAYER; ++i)
		gamePieces[i].setAlive();
}

void GeneralPlayer::setPlayerForNewGame(int playerIndex) {
	_name = playerIndex == 0 ? "A" : "B";
	setPlayer(playerIndex);
	initGamePiecesProperties();
	initGamePiecesAndFlagsLocation();
	return;
}

void GeneralPlayer::resetGamePiecesAndFlagsLocation_self() {
	for (int i = 0; i < 3; i++)
		gamePieces[i].setRowColumn(0, 0);
	flag_self.setRowCol(NUM_OF_ROWS_OR_COLUMNS, NUM_OF_ROWS_OR_COLUMNS);
	flag_opponent.setRowCol(NUM_OF_ROWS_OR_COLUMNS, NUM_OF_ROWS_OR_COLUMNS);
}

char GeneralPlayer::getCharFromBoard(int x, int y) const {
	return _board->charAt(x, y);
}

GamePiece * GeneralPlayer::getGamePieceById_self(const int id) {
	if (id < 4 && playerIndex == 0)
		return &gamePieces[id - 1];
	if (id > 4 && playerIndex == 1)
		return &gamePieces[id - 7];
	return nullptr;
}

void GeneralPlayer::initGamePiecesProperties() {
	if (playerIndex == 0) {
		gamePieces[0].setGamePieceProperties(1, false, false);
		gamePieces[1].setGamePieceProperties(2, true, true);
		gamePieces[2].setGamePieceProperties(3, true, false);
	}
	else {
		gamePieces[0].setGamePieceProperties(7, true, true);
		gamePieces[1].setGamePieceProperties(8, false, true);
		gamePieces[2].setGamePieceProperties(9, false, false);
	}
}

void GeneralPlayer::initGamePiecesAndFlagsLocation() {
	GamePiece * gamePiece_board = nullptr;
	char ch;

	for (int row = 0; row < NUM_OF_ROWS_OR_COLUMNS; ++row)
		for (int col = 0; col < NUM_OF_ROWS_OR_COLUMNS; ++col) {
			ch = _board->charAt(col + 1, row + 1);
			if (ch >= '1' && ch <= '9') {
				if (playerIndex == 0)
					gamePieces[ch - '1'].setRowColumn(row, col);
				else
					gamePieces[ch - '7'].setRowColumn(row, col);
			}
			if (ch == 'A') {
				if (playerIndex == 0)
					flag_self.setRowCol(row, col);
				else
					flag_opponent.setRowCol(row, col);
			}
			if (ch == 'B') {
				if (playerIndex == 0)
					flag_opponent.setRowCol(row, col);
				else
					flag_self.setRowCol(row, col);
			}
		}
	return;
}

GameMove GeneralPlayer::play(const GameMove& opponentsMove) {
	int from_x = 0, to_x = 0, from_y = 0, to_y = 0;
	
	setPlayerNextMove(opponentsMove);

	if (currGamePiece) {
		from_x = to_x = currGamePiece->getColumn() + 1;
		from_y = to_y = currGamePiece->getRow() + 1;
	}

	switch (currDirection) {
	case Directions::UP:
		to_y--;
		break;
	case Directions::DOWN:
		to_y++;
		break;
	case Directions::LEFT:
		to_x--;
		break;
	case Directions::RIGHT:
		to_x++;
		break;
	case Directions::STOP:
	default:
		break;
	}

	return GameMove(from_x, from_y, to_x, to_y);
}


//------------------------------ KEYBOARD PLAYER ------------------------------//

void KeyboardPlayer::getInputFromConsole() {

}

void KeyboardPlayer::setPlayerNextMove(const GameMove& opponentsMove) {
	char ch;
	GamePiece * gamePiece;

	getInputFromConsole();

	if (getCurrGamePiece())
		updatePlayerPrevMove();

	if (!inputStream.empty()) {
		ch = inputStream.front();
		inputStream.pop();
		if (A_GP_1 == ch || A_GP_2 == ch || A_GP_3 == ch || B_GP_7 == ch || B_GP_8 == ch || B_GP_9 == ch) {
			if ((gamePiece = getGamePieceById_self(ch - '0')) && gamePiece->getIsAlive()) {
				nextGamePiece = gamePiece;
				
				if (getCurrGamePiece() == nullptr)
					setCurrGamePiece(gamePiece);
			}
		}
		else {
			setCurrGamePiece(nextGamePiece);
			if (A_UP == ch || B_UP == ch)
				setCurrDirection(Directions::UP);
			if (A_DOWN == ch || B_DOWN == ch)
				setCurrDirection(Directions::DOWN);
			if (A_LEFT == ch || B_LEFT == ch)
				setCurrDirection(Directions::LEFT);
			if (A_RIGHT == ch || B_RIGHT == ch)
				setCurrDirection(Directions::RIGHT);
		}
	}
}

//------------------------------ FILES PLAYER ------------------------------//

bool FilePlayer::loadMovesFromFile() {
	bool movesFileExists = openMovesFile();
	bool res = true;

	if (movesFileExists) {
		if (currMovesFile.is_open()) {
			readMovesFromFile();
			currMovesFile.close();
			res = true;
		}
		else {
			clearScreen();
			cout << "Failed to open moves file" << endl;
			system("pause");
			res = false;
		}
	}
	else
		res = false;

	return res;
}

void FilePlayer::setPlayerNextMove(const GameMove& opponentsMove) {
	GameMove_Parsed currMove;
	
	if (getCurrGamePiece())
		updatePlayerPrevMove();

	if (!moves.empty()) {
		currMove = moves.front();
		if (currMove.getGIteration() == iteration) {

			GamePiece * gamePiece = getGamePieceById_self(currMove.getGPiece());
			setCurrGamePiece(gamePiece);

			Directions direction = currMove.getDir();
			setCurrDirection(direction);
			moves.pop();
		}
		else {

		}
	}
	iteration = iteration + 2;
}

void GeneralPlayer::updatePlayerPrevMove() {
	BoardPosition destTile = getPositionOfDestMove();
	char ch = _board->charAt(destTile.col + 1, destTile.row + 1);

	if (ch >= '1' && ch <= '9')
		selfGamePieceOnDestTile(ch, destTile);
	else if (ch == CHAR_OF_OPPONENTS_GAME_PIECE)
		opponentGamePieceOnDestTile();
	else
		currDirection = Directions::STOP;
}

void GeneralPlayer::selfGamePieceOnDestTile(char gamePieceOnDestTile, BoardPosition destTile) {
	char movingGamePiece = '0' + (currGamePiece->getId());

	if (movingGamePiece == gamePieceOnDestTile)
		currGamePieceMovedOnGameBoard();
	else {
		currDirection = Directions::STOP;
		getGamePieceById_self(gamePieceOnDestTile - '0')->setRowColumn(destTile.row, destTile.col);
	}
}

void GeneralPlayer::opponentGamePieceOnDestTile() {
	currDirection = Directions::STOP;
	currGamePiece->setDead();
	currGamePiece = nullptr;
}

void GeneralPlayer::currGamePieceMovedOnGameBoard() {
	BoardPosition updatedPos = currGamePiece->getPos();

	switch (currDirection) {
	case Directions::UP:
		updatedPos.row--;
		break;
	case Directions::DOWN:
		updatedPos.row++;
		break;
	case Directions::LEFT:
		updatedPos.col--;
		break;
	case Directions::RIGHT:
		updatedPos.col++;
            break;
    case Directions::STOP:
	default:
		break;
	}

	currGamePiece->setBoardPos(updatedPos);
}

BoardPosition GeneralPlayer::getPositionOfDestMove() const {
	BoardPosition res, currPos;
	
	if (currGamePiece) {
		currPos = currGamePiece->getBoardPos();
		res = getBoardPositionInDirection(currPos, currDirection);
	}
		
	return res;
}

BoardPosition GeneralPlayer::getBoardPositionInDirection(BoardPosition from, Directions direction) const {
	BoardPosition to, illegalBoardPos;

	if (isLegalBoardPos(from)) {
		to = from;
		switch (direction) {
		case Directions::DOWN:
			to.row++;
			break;
		case Directions::UP:
			to.row--;
			break;
		case Directions::RIGHT:
			to.col++;
			break;
		case Directions::LEFT:
			to.col--;
			break;
		case Directions::STOP:
			break;
		default:
			break;
		}
	}

	if (isLegalBoardPos(to))
		return to;

	return illegalBoardPos;
}

bool GeneralPlayer::isLegalBoardPos(const BoardPosition pos) const {
	return pos.row >= 0 && pos.row < NUM_OF_ROWS_OR_COLUMNS 
		&& pos.col >= 0 && pos.col < NUM_OF_ROWS_OR_COLUMNS;
}

bool FilePlayer::openMovesFile() {
	string fullMovesFileName, shortMovesFileName, movesFile_noType;
	bool fileFound = false;

	while (!files.empty() && !fileFound) {
		fullMovesFileName = path + "\\";
		shortMovesFileName = files.front();
		fullMovesFileName += shortMovesFileName;

		movesFile_noType = removePostfixFromFileName(shortMovesFileName, getPlayerIndex() == 0 ? MOVES_A_FILE_TYPE : MOVES_B_FILE_TYPE);

		if (isBoardFile) {
			if (currBoardFileName.compare(movesFile_noType) == 0)
				fileFound = true;
			else if (currBoardFileName.compare(movesFile_noType) > 0)
				files.pop();
		}
		else
			fileFound = true;
	}

	if (fileFound) {
		files.pop();
		currMovesFile.open(fullMovesFileName.c_str(), ios_base::in);
	}

	return fileFound;
}

bool FilePlayer::readMovesFromFile() {
	Directions direction;
	string buffer;
	int iteration, gamePieceId;
	GameMove_Parsed currMove;
	bool isLegalLineInput = true, readiteration = true, readGamePiece = true, readDirection = true;

	while (!currMovesFile.eof() && isLegalLineInput) {

		readiteration = getIterationFromFile(iteration);
		if (currMovesFile.eof())
			isLegalLineInput = false;

		readGamePiece = getGamePieceFromFile(gamePieceId);
		if (currMovesFile.eof())
			isLegalLineInput = false;

		readDirection = getDirectionFromFile(direction);

		isLegalLineInput = isLegalLineInput && readiteration && readGamePiece && readDirection;

		if (isLegalLineInput) {
			currMove.setIteration(iteration);
			currMove.setGPiece(gamePieceId);
			currMove.setDir(direction);

			moves.push(currMove);
		}

		getline(currMovesFile, buffer);
	}

	return isLegalLineInput;
}

bool FilePlayer::getIterationFromFile(int & iteration) {
	char ch;
	bool readiteration = false;
	iteration = 0;

	while (currMovesFile.get(ch) && !(ch >= '0' && ch <= '9') && ch != '\n') {}
	if (!currMovesFile.eof()) {
		readiteration = true;
		iteration *= 10;
		iteration += ch - '0';
		while (currMovesFile.get(ch) && (ch >= '0' && ch <= '9')) {
			iteration *= 10;
			iteration += ch - '0';
		}
	}
	return readiteration;
}

bool FilePlayer::getGamePieceFromFile(int & gamePiece) {
	char ch;
	bool readGamePiece = false;

	while (currMovesFile.get(ch) && !(ch >= '0' && ch <= '9') && ch != '\n') {}
	if (!currMovesFile.eof()) {
		if (ch >= '0' && ch <= '9') {
			gamePiece = ch - '0';
			readGamePiece = true;
		}
		else
			readGamePiece = false;
	}

	return readGamePiece;
}

bool FilePlayer::getDirectionFromFile(Directions & direction) {
	char ch;
	bool readDirection = false;

	while (currMovesFile.get(ch) && 'u' != tolower(ch) && 'd' != tolower(ch) && 'l' != tolower(ch) && 'r' != tolower(ch) && ch != '\n') {}
	if (!currMovesFile.eof()) {
		switch (tolower(ch)) {
		case 'u':
			direction = Directions::UP;
			readDirection = true;
			break;
		case 'd':
			direction = Directions::DOWN;
			readDirection = true;
			break;
		case 'l':
			direction = Directions::LEFT;
			readDirection = true;
			break;
		case 'r':
			direction = Directions::RIGHT;
			readDirection = true;
			break;
		default:
			readDirection = false;
			break;
		}
	}
	return readDirection;
}

void AlgoPlayer::initGamePiecesProperties() {
	int statrGamePiecesIndex_self = getPlayerIndex() == 0 ? 1 : 7;
	int statrGamePiecesIndex_opponent = getPlayerIndex() == 0 ? 7 : 1;

	for (int i = 0; i < NUM_OF_PIECES_PER_PLAYER; ++i) {
		//init self game pieces properties
		getGamePieceById_self(statrGamePiecesIndex_self + i)->setGamePiecePropertiesById(statrGamePiecesIndex_self + i);
		//init opponent's game pieces properties
		gamePieces_opponent[statrGamePiecesIndex_opponent + i].setGamePiecePropertiesById(statrGamePiecesIndex_opponent + i);
	}
}

void AlgoPlayer::updateOpponentsGamePiece(const GameMove & opponentsMove) {
	if (opponentsMove.from_x == 0 && opponentsMove.from_y == 0 && opponentsMove.to_x == 0 && opponentsMove.to_y == 0)
		return;

	BoardPosition pos(opponentsMove.from_y - 1, opponentsMove.from_x - 1);
	
	for (int i = 0; i < NUM_OF_PIECES_PER_PLAYER; ++i) {
		if (gamePieces_opponent[i].getBoardPos() == pos)
			gamePieces_opponent[i].setRowColumn(opponentsMove.to_y - 1, opponentsMove.to_x - 1);
	}
}


void AlgoPlayer::initGameLocations() {
	char ch;
	int index_gamePieceOpponent = 0;
	GamePiece * gamePiece;

	for (int row = 0; row < NUM_OF_ROWS_OR_COLUMNS; ++row)
		for (int col = 0; col < NUM_OF_ROWS_OR_COLUMNS; ++col) {
			ch = getCharFromBoard(col + 1, row + 1);
			cBoard[row][col] = ch;

			if (ch >= '1' && ch <= '9') {
				gamePiece = getGamePieceById_self(ch - '0');
				if (gamePiece)
					gamePiece->setRowColumn(row, col);
			}

			switch (ch) {
			case 'A':
				if (getPlayerIndex() == 0)
					getFlag_self().setRowCol(row, col);
				else
					getFlagOpponent().setRowCol(row, col);
				break;
			case 'B':
				if (getPlayerIndex() == 0)
					getFlagOpponent().setRowCol(row, col);
				else
					getFlag_self().setRowCol(row, col);
				break;
			case '#':
				gamePieces_opponent[index_gamePieceOpponent].setRowColumn(row, col);
				index_gamePieceOpponent++;
				break;
			default:
				break;
			}
		}
	return;
}

void AlgoPlayer::initDistanceMaps()
{
	char tmp;
	int insert_self[NUM_OF_PIECES_PER_PLAYER];
	int insert_opponent[NUM_OF_PIECES_PER_PLAYER];

	for (int i = 0; i < NUM_OF_ROWS_OR_COLUMNS; i++)
	{
		for (int j = 0; j < NUM_OF_ROWS_OR_COLUMNS; j++)
		{
			tmp = cBoard[i][j];
			for (int k = 0; k < NUM_OF_PIECES_PER_PLAYER; k++) {
				insert_self[k] = 99;
				insert_opponent[k] = 99;
			}
			switch (tmp)
			{
			case 'A':
				if (getPlayerIndex() == 0)
					setSelfFlagOnDistanceMap(insert_self, insert_opponent);
				else
					setOpponentFlagOnDistanceMap(insert_self, insert_opponent);
				break;
			case 'B':
				if (getPlayerIndex() == 1)
					setSelfFlagOnDistanceMap(insert_self, insert_opponent);
				else
					setOpponentFlagOnDistanceMap(insert_self, insert_opponent);
				break;
			case 'T':
				setForestOnDistanceMap(insert_self, insert_opponent);
				break;
			case 'S':
				setSeaOnDistanceMap(insert_self, insert_opponent);
				break;
			}
			for (int k = 0; k < NUM_OF_PIECES_PER_PLAYER; k++)
			{	//inits all cells in maps accordingly after setting the correct insert value for each map
				distanceMap_flagSelf[k][i][j] = insert_self[k];
				distanceMap_flagOpponent[k][i][j] = insert_opponent[k];
			}

		}
	}
}

void AlgoPlayer::setSeaOnDistanceMap(int permissionArr_self[NUM_OF_PIECES_PER_PLAYER], int permissionArr_opponent[NUM_OF_PIECES_PER_PLAYER]) {
	for (int i = 0; i < NUM_OF_PIECES_PER_PLAYER; ++i) {
		if (!getGamePieceByInternalIndex(i)->getAmphibian()) {
			permissionArr_self[i] = -1;
			permissionArr_opponent[i] = -1;
		}
	}
}

void AlgoPlayer::setForestOnDistanceMap(int permissionArr_self[NUM_OF_PIECES_PER_PLAYER], int permissionArr_opponent[NUM_OF_PIECES_PER_PLAYER]) {
	for (int i = 0; i < NUM_OF_PIECES_PER_PLAYER; ++i) {
		if (!getGamePieceByInternalIndex(i)->getForestRanger()) {
			permissionArr_self[i] = -1;
			permissionArr_opponent[i] = -1;
		}
	}
}

void AlgoPlayer::setSelfFlagOnDistanceMap(int permissionArr_self[NUM_OF_PIECES_PER_PLAYER], int permissionArr_opponent[NUM_OF_PIECES_PER_PLAYER]) {
	for (int i = 0; i < NUM_OF_PIECES_PER_PLAYER; ++i) {
		permissionArr_self[i] = 0;
		permissionArr_opponent[i] = -1;
	}
}

void AlgoPlayer::setOpponentFlagOnDistanceMap(int permissionArr_self[NUM_OF_PIECES_PER_PLAYER], int permissionArr_opponent[NUM_OF_PIECES_PER_PLAYER]) {
	for (int i = 0; i < NUM_OF_PIECES_PER_PLAYER; ++i) {
		permissionArr_self[i] = 99;
		permissionArr_opponent[i] = 0;
	}
}

void AlgoPlayer::clacDistanceMap(int * map, BoardPosition dest, int step)
{
	// create local array of points in orbit to current point.
	BoardPosition orbit[4];
	orbit[0].row = dest.row - 1;	// up
	orbit[0].col = dest.col;
	orbit[1].row = dest.row + 1;	// down
	orbit[1].col = dest.col;
	orbit[2].col = dest.col - 1;	// left
	orbit[2].row = dest.row;
	orbit[3].col = dest.col + 1;	// right
	orbit[3].row = dest.row;

	if (dest.row >= NUM_OF_ROWS_OR_COLUMNS || dest.col >= NUM_OF_ROWS_OR_COLUMNS || dest.row <0 || dest.col <0)
		return;

	map[dest.row * NUM_OF_ROWS_OR_COLUMNS + dest.col] = step;

	for (int i = 0; i < 4; i++)
	{
		if (isLegalCalcStep(map, orbit[i]))
		{
			if (map[orbit[i].row * NUM_OF_ROWS_OR_COLUMNS + orbit[i].col] > step + 1)
			{
				clacDistanceMap(map, orbit[i], step + 1);
			}
		}
	}




}

void AlgoPlayer::distanceMapCalcLauncher()
{
	for (int i = 0; i < NUM_OF_PIECES_PER_PLAYER; i++)
	{
		clacDistanceMap((int*)distanceMap_flagSelf[i], getFlag_self().getPos(), 0);
		clacDistanceMap((int*)distanceMap_flagOpponent[i], getFlagOpponent().getPos(), 0);
	}
}

bool AlgoPlayer::isLegalCalcStep(int * map, BoardPosition dest)
{
	if (dest.col >= NUM_OF_ROWS_OR_COLUMNS || dest.row >= NUM_OF_ROWS_OR_COLUMNS || dest.row <0 || dest.col <0)
		return false;

	if (map[dest.row * NUM_OF_ROWS_OR_COLUMNS + dest.col] == -1)
		return false;

	return true;
}

void AlgoPlayer::setPlayerForNewGame(int player)
{
	setPlayer(player);
	setName(player == 0 ? "A" : "B");
	initGamePiecesProperties();
	initGameLocations();
	initDistanceMaps();
	distanceMapCalcLauncher();
}

void AlgoPlayer::gpClosestToEnemyFlag()
{
	GamePiece * closest = nullptr, * currGamePiece;
	int min = 99;

	for (int i = 0; i < NUM_OF_PIECES_PER_PLAYER; i++)
	{
		currGamePiece = getGamePieceByInternalIndex(i);
		if (currGamePiece->getIsAlive())
		{
			// Reminder: distanceMap[4: GamePiece Mobility types][2: toFlagB(0), toFlagA(1)][y][x]
			int distance = distanceMap_flagOpponent[i][currGamePiece->getRow()][currGamePiece->getColumn()];
			if (distance < min)
			{
				distance = min;
				setCurrGamePiece(currGamePiece);
			}
		}
	}
}

void AlgoPlayer::advanceOffence()
{
	GamePiece * currGamePiece = getCurrGamePiece();
	if (currGamePiece == nullptr)
	{
		setCurrDirection(Directions::STOP);
	}
	else
	{
		int min = 99;
		int toX = 0, toY = 0;
		int internalIndex_gp = getPlayerIndex() == 0 ? currGamePiece->getId() - 1 : currGamePiece->getId() - 7;
		int * map = (int*)distanceMap_flagOpponent[internalIndex_gp];
		Orbit orbit(currGamePiece->getColumn(), currGamePiece->getRow());
		for (int i = 0; i < 4; i++)
		{
			BoardPosition p = orbit[i];
			if (isLegalCalcStep(map, orbit[i]))
			{
				int distance = map[p.row * NUM_OF_ROWS_OR_COLUMNS + p.col];
				if (distance < min)
				{
					min = distance;
					setCurrDirection((Directions)i);
				}
			}
		}
	}
}

void AlgoPlayer::setPlayerNextMove(const GameMove& opponentsMove) {

	if (getCurrGamePiece())
		updatePlayerPrevMove();

	updateOpponentsGamePiece(opponentsMove);

	gpClosestToEnemyFlag();
	advanceOffence();
}

