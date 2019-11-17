/* Test file for Dependencies Mapping Plugin -
 Game.cpp */
#include "Game.h"

int Game::counter = 0;

void Game::gameManager() {
	GameOptions gameState = cmdConfig.getMovesSource() == 'k' ? GameOptions::MAIN_MENU : GameOptions::NEW;
	bool playersSwapped = false;

	getFilesInDir();

	while (gameState != GameOptions::EXIT) {

		//display main menu
		while (gameState == GameOptions::MAIN_MENU) {
			if (playersSwapped) {
				swapPlayers();
				playersSwapped = false;
			}
			gameState = mainMenu(playersSwapped);
		}

		//check if need to skip to next round, otherwise play a round (board and moves are legal)
		if (gameState == GameOptions::NEW && setNewGame(gameState)) {
			players[0].player->setPlayerForNewGame(0);
			players[1].player->setPlayerForNewGame(1);
		
			gameState = playSingleRound();
		}
	}

	printGameEndBanner();
	return;
}

//--------------------play functions--------------------------------------

void Game::resetGame() {
	board.resetBoard();
	resetPlayerData();
}

void Game::resetPlayerData() {
	for (int i = 0; i < NUM_OF_PLAYERS; ++i) {
		players[i].player->resetPlayer();
		board.setPlayerCurrDirection(i, Directions::STOP);
		board.setPlayerCurrGamePiece(i, 0);
	}
}

bool Game::isBoardAvailable() const {
	bool tryLoadBoard = true;

	if (cmdConfig.isBoardFileInUse()) {
		if (boardFiles.empty()) {
			tryLoadBoard = false;
			clearScreen();
			cout << "There are no board files available" << endl;
			system("pause");
		}
	}
	else {
		if (cmdConfig.getMovesSource() == 'a') {
			if (cmdConfig.getNumberOfRoundLeft() == 0) {
				tryLoadBoard = false;
				clearScreen();
				cout << "Played All game rounds" << endl;
				system("pause");
			}
			else
				tryLoadBoard = true;
		}
	}

	return tryLoadBoard;
}

bool Game::setNewGame(GameOptions & gameState) {
	bool boardLoaded = false;
	bool areMoves = false;

	if (!isBoardAvailable() || !areAvailableMovesToContinueGame()) {
		gameState = GameOptions::EXIT;
		return false;
	}

	resetGame();
	boardLoaded = loadBoard();

	if (boardLoaded) {
		if (cmdConfig.getMovesSource() == 'a')
			cmdConfig.decreaseNumberOfRoundsByOne();
		areMoves = areAvailableMovesForNextRound();
	}

	gameState = GameOptions::NEW;
	return boardLoaded && areMoves;
}

bool Game::areAvailableMovesToContinueGame() const {
	bool res = true;

	if (cmdConfig.getMovesSource() == 'f') {
		res = !(((FilePlayer*)(players[0].player))->isFilesQueueEmpty()) ||
			!(((FilePlayer*)(players[1].player))->isFilesQueueEmpty());
	}

	if (res == false) {
		clearScreen();
		cout << "There are no moves files" << endl;
		system("pause");
	}

	return res;
}

bool Game::areAvailableMovesForNextRound() const {
	bool areMoves_playerA, areMoves_playerB;

	if (cmdConfig.getMovesSource() == 'f') {
		areMoves_playerA = ((FilePlayer*)players[0].player)->areAvailableMoves();
		areMoves_playerB = ((FilePlayer*)players[1].player)->areAvailableMoves();

		if (!areMoves_playerA && !areMoves_playerB) {
			if (cmdConfig.isBoardFileInUse()) {
				clearScreen();
				cout << "There are no moves files matching the board file " << boardFileName << endl;
				system("pause");
				return false;
			}
		}
		else
			return true;
	}
	return true;
}

GameOptions Game::playSingleRound() {
	bool moveOccurred = false, moveChanged = false;
	GameOptions gameState = GameOptions::CONTINUE;
	GeneralPlayer * winningPlayer = nullptr;
	int iteration = 1, numSteps = 0, side;
	queue<GameMove_Parsed> movesForRecording;
	GameMove * currGameMove = nullptr;
	GameMove_Parsed prevGameMove_A, prevGameMove_B, currGameMove_parsed;

	counter++;
	if (!cmdConfig.isQuiet())
		displayBoard();

	while (gameState == GameOptions::CONTINUE) {
		side = (iteration + 1) % 2;

		moveOccurred = false;
		moveChanged = false;

		if (gameState == GameOptions::CONTINUE) {

			//play a single turn on behalf of one of the players
			currGameMove = turn(side, moveOccurred, currGameMove);

			numSteps = moveOccurred ? numSteps + 1 : numSteps;

			//save move for recording to file
			currGameMove_parsed = parseCurrGameMove(side);
			moveChanged = isGameMoveChanged(currGameMove_parsed, prevGameMove_A, prevGameMove_B, side);

			if (cmdConfig.isRecording() && moveOccurred && moveChanged) {
				currGameMove_parsed.setIteration(iteration);
				movesForRecording.push(currGameMove_parsed);
			}

			//check if game has reached its end
			gameState = stopGameConditon(iteration);
			if (isGameOver(&winningPlayer) || gameState != GameOptions::CONTINUE) {
				printMatchEndBanner(winningPlayer, numSteps);
				if (winningPlayer != nullptr) {
					(*winningPlayer).addPointToScore();
					gameState = cmdConfig.getMovesSource() == 'k' ? GameOptions::MAIN_MENU : GameOptions::NEW;
				}
			}
		}

		iteration++;
	}
	if (currGameMove)
		delete currGameMove;

	if (cmdConfig.isRecording())
		recordMovesToFiles(movesForRecording);

	return gameState;
}

bool Game::isGameMoveChanged(GameMove_Parsed & currGameMove, GameMove_Parsed & prevGameMoveA, GameMove_Parsed & prevGameMoveB, const int side) const {
	bool res = false;

	if (currGameMove.getDir() != Directions::STOP) {
		if (side == 0) {
			res = prevGameMoveA != currGameMove;
			prevGameMoveA = currGameMove;
		}
		else {
			res = prevGameMoveB != currGameMove;
			prevGameMoveB = currGameMove;
		}
	}
	return res;
}

GameOptions Game::stopGameConditon(const long iteration) {
	GameOptions res = GameOptions::CONTINUE;

	switch (cmdConfig.getMovesSource()) {
	case 'a':
		if (iteration == cmdConfig.getMaxNumberOfItterations())
			res = GameOptions::NEW;
		break;
	case 'f':
		if (noAvailableMoves())
			res = GameOptions::NEW;
		break;
	case 'k':
		break;
	default:
		break;
	}
	
	return res;
}

void Game::printGameEndBanner() const {
	clearScreen();
	cout << "Game Summary" << endl <<
		"A points: " << players[0].player->getScore() << endl <<
		"B points: " << players[1].player->getScore() << endl;
}

void Game::printMatchEndBanner(GeneralPlayer * const winner, const int numSteps) const {
	clearScreen();

	cout << "Game cycle: " << counter << endl <<
		"Num moves: " << numSteps << endl <<
		"Winner: ";
	if (winner) {
		cout << winner->getName() << endl;
	}
	else
		cout << "NONE" << endl;

	system("pause");
}

bool Game::setBoardForNewGame_file() {
	string boardFileName_full, boardFileName_short;

	if (!boardFiles.empty()) {
		boardFileName_full = boardFiles.front();
		boardFiles.pop();
		boardFileName_short = removePostfixFromFileName(boardFileName_full, BOARD_FILE_TYPE);
		boardFileName = boardFileName_short;
		if (typeid(*players[0].player) == typeid(FilePlayer)) {
			((FilePlayer*)(players[0].player))->setCurrBoardFileName(boardFileName);
			((FilePlayer*)(players[1].player))->setCurrBoardFileName(boardFileName);
		}
		return board.readBoardFromFile(boardFileName_full.c_str(), cmdConfig.getPath());
	}

	return false;
}

void Game::setBoardForNewGame_random() {
	board.initBoardSurfaces_random();
	board.initGamePiecesAndFlags_random();
}

void Game::displayBoard() {
	if (!cmdConfig.isQuiet()) {
		board.drawBoard();
		printScoreBelowBoard();
	}
}

bool Game::loadBoard() {
	bool res = true;
	
	if (cmdConfig.isBoardFileInUse()) {
		res = setBoardForNewGame_file();
	}
	else {
		setBoardForNewGame_random();
		if (cmdConfig.isRecording())
			writeBoardToFile();
	}

	return res;
}

GameMove * Game::turn(const int side, bool & moveOccurred, GameMove * opponentsMove) {
	GameMove * currGameMove;

	if (opponentsMove) {
		currGameMove = new GameMove(players[side].player->play(*opponentsMove));
		delete opponentsMove;
	}
	else
		currGameMove = new GameMove(players[side].player->play(GameMove(0, 0, 0, 0)));
	
	if (isLegalGameMove(side, *currGameMove)) {
		if (Directions::STOP == getDirectionOfLegalGameMove(*currGameMove))
			moveOccurred = false;
		else
			moveOccurred = move(*currGameMove);
	}
	else
		board.setPlayerCurrDirection(side, Directions::STOP);

	return currGameMove;
}

bool Game::isLegalGameMove(const int side, const GameMove gameMove) {
	int relativeRow, relativeCol;
	GamePiece * gamePieceOnTile; 
	bool isLegalMove = true;

	if (gameMove.to_x >= 1 && gameMove.to_x <= NUM_OF_ROWS_OR_COLUMNS &&
		gameMove.to_y >= 1 && gameMove.to_y <= NUM_OF_ROWS_OR_COLUMNS) {

		gamePieceOnTile = board.getTile(gameMove.from_y - 1, gameMove.from_x - 1).getGamePieceOnTile();

		if (gamePieceOnTile && (gamePieceOnTile->getSide() == side) && gamePieceOnTile->getIsAlive()) {
			relativeCol = gameMove.to_x - gameMove.from_x;
			relativeRow = gameMove.to_y - gameMove.from_y;
			if (relativeCol == 0 || relativeRow == 0) {
				if (relativeCol < -1 && relativeCol > 1)
					isLegalMove = false;
				if (relativeRow < -1 && relativeRow > 1)
					isLegalMove = false;
			}
			else
				isLegalMove = false;
		}
		else
			isLegalMove = false;
	}
	else
		isLegalMove = false;

	return isLegalMove;
}

/*************************************************************\
* For a given player, Game::move checks the conditions prevailing on the board tile that 
* is in the chosen direction and updates the results of the move, according to the chosen 
* game piece:
* (1) what is the surface of the tile?
* (2) is there a game piece on the tile?
\*************************************************************/
bool Game::move(GameMove & gameMove) {
	GamePiece * gamePiece_destTile, * gamePiece_srcTile;
	bool isMovePossible;

	const Surface destSurface = board.getTile(gameMove.to_y - 1, gameMove.to_x - 1).getSurfaceType();
	gamePiece_destTile = board.getTile(gameMove.to_y - 1, gameMove.to_x - 1).getGamePieceOnTile();
	gamePiece_srcTile = board.getTile(gameMove.from_y - 1, gameMove.from_x - 1).getGamePieceOnTile();

	//switch according to the surface of the tile to which the game piece is directed to advance
	switch (destSurface) {
	case Surface::FR:
		destSurfaceIsForest(gamePiece_srcTile, gameMove, gamePiece_destTile, isMovePossible);
		break;
	case Surface::SEA:
		destSurfaceIsSea(gamePiece_srcTile, gameMove, gamePiece_destTile, isMovePossible);
		break;
	case Surface::FLAGA:
		destSurfaceIsFlagA(gamePiece_srcTile, gameMove, isMovePossible);
		break;
	case Surface::FLAGB:
		destSurfaceIsFlagB(gamePiece_srcTile, gameMove, isMovePossible);
		break;
	case Surface::REGULAR:
		destSurfaceIsRegular(gamePiece_srcTile, gameMove, gamePiece_destTile, isMovePossible);
		break;
	default:
		break;
	}
		
	return isMovePossible;
}

void Game::destSurfaceIsRegular(GamePiece * const movingGamePiece, const GameMove & gameMove, GamePiece * const gamePieceInDest, bool & isMovePossible) {
	if (gamePieceInDest)
		twoGamePiecesFace2Face(movingGamePiece, gameMove, gamePieceInDest, isMovePossible);
	else {
		isMovePossible = true;
		updateGamePieceMove(movingGamePiece, gameMove);
	}
}

void Game::destSurfaceIsSea(GamePiece * const movingGamePiece, const GameMove & gameMove, GamePiece * const gamePieceInDest, bool & isMovePossible) {
	if (movingGamePiece->getAmphibian()) // can game piece move in SEA tiles?
		if (gamePieceInDest) // does the tile in the direction already have a game piece on it?
			twoGamePiecesFace2Face(movingGamePiece, gameMove, gamePieceInDest, isMovePossible);
		else {
			updateGamePieceMove(movingGamePiece, gameMove);
			isMovePossible = true;
		}
	else
		isMovePossible = false;
}

void Game::destSurfaceIsForest(GamePiece * const movingGamePiece, const GameMove & gameMove, GamePiece * const gamePieceInDest, bool & isMovePossible) {
	if (movingGamePiece->getForestRanger()) // can game piece move in forest tiles?
		if (gamePieceInDest) // does the tile in the direction already have a game piece on it?
			twoGamePiecesFace2Face(movingGamePiece, gameMove, gamePieceInDest, isMovePossible);
		else {
			updateGamePieceMove(movingGamePiece, gameMove);
			isMovePossible = true;
		}
	else
		isMovePossible = false;
}

void Game::destSurfaceIsFlagA(GamePiece * const movingGamePiece, const GameMove & gameMove, bool & isMovePossible) {
	if (movingGamePiece->getSide() == 1) {
		updateGamePieceMove(movingGamePiece, gameMove);
		isMovePossible = true;
	}
	else
		isMovePossible = false;
}

void Game::destSurfaceIsFlagB(GamePiece * const movingGamePiece, const GameMove & gameMove, bool & isMovePossible) {
	if (movingGamePiece->getSide() == 0) {
		updateGamePieceMove(movingGamePiece, gameMove);
		isMovePossible = true;
	}
	else
		isMovePossible = false;
}

//assuming GameMove is legal
GameMove_Parsed Game::parseCurrGameMove(const int side) const {
	int movingGamePiece;
	Directions direction = Directions::STOP;
	GameMove_Parsed res;

	movingGamePiece = board.getPlayerCurrGamePieceId(side);
	res.setGPiece(movingGamePiece);

	direction = board.getPlayerCurrDirection(side);
	res.setDir(direction);

	return res;
}

//assuming GameMove is legal
Directions Game::getDirectionOfLegalGameMove(const GameMove & gameMove) const {
	int relativeRow, relativeCol;
	Directions dir = Directions::STOP;

	relativeCol = gameMove.to_x - gameMove.from_x;
	relativeRow = gameMove.to_y - gameMove.from_y;
	
	if (relativeCol == 1)
		dir = Directions::RIGHT;
	if (relativeCol == -1)
		dir = Directions::LEFT;
	if (relativeRow == 1)
		dir = Directions::DOWN;
	if (relativeRow == -1)
		dir = Directions::UP;

	return dir;
}

bool Game::isGameOver(GeneralPlayer ** const winningPlayer) const {
	int winningPlayerIndex;

	bool flagCaptured = board.isFlagCaptured(winningPlayerIndex);

	if (flagCaptured) {
		*winningPlayer = players[winningPlayerIndex].player;
		return true;
	}
	else {
		bool playerWithAllGamePiecesDead = board.isPlayerWithAllGamePiecesDead(winningPlayerIndex);
		if (playerWithAllGamePiecesDead) {
			*winningPlayer = players[winningPlayerIndex].player;
			return true;
		}
	}

	*winningPlayer = nullptr;
	return false;
}

void Game::updateGamePieceMove(GamePiece * const movingGamePiece, const GameMove & gameMove) {
	//remove gamepiece from tile
	removeGamePieceFromTile(movingGamePiece, gameMove);

	//put game piece on tile
	putGamePieceOnTile(movingGamePiece, gameMove);

	//update player
	int side = movingGamePiece->getSide();
	board.setPlayerCurrDirection(side, getDirectionOfLegalGameMove(gameMove));
	board.setPlayerCurrGamePiece(side, movingGamePiece->getId());
}

void Game::removeGamePieceFromTile(GamePiece * const movingGamePiece, const GameMove & gameMove) {
	int srcRow = gameMove.from_y - 1;
	int srcCol = gameMove.from_x - 1;

	board.getTile(srcRow, srcCol).setGamePiece(nullptr);

	if (!cmdConfig.isQuiet())
		board.drawTile(srcRow, srcCol);
}

void Game::putGamePieceOnTile(GamePiece * const movingGamePiece, const GameMove & gameMove) {
	int destRow = gameMove.to_y - 1;
	int destCol = gameMove.to_x - 1;

	board.getTile(destRow, destCol).setGamePiece(movingGamePiece);
	movingGamePiece->setRowColumn(destRow, destCol);

	if (!cmdConfig.isQuiet())
		board.drawTile(destRow, destCol);
}

void Game::twoGamePiecesFace2Face(GamePiece * const movingGamePiece, const GameMove & gameMove, GamePiece * const gamePieceInTile, bool & moveOccurred) {
	GamePiece * winningGamePiece, * losingGamePiece;

	// check if the two game pieces ae of the same player
	if (gamePieceInTile->getSide() == movingGamePiece->getSide()) { // are the two game pieces of the same player?
		moveOccurred = false;
	}
	else {
		winningGamePiece = fight(movingGamePiece, gamePieceInTile, gameMove.to_y - 1, gameMove.to_x - 1);
		losingGamePiece = getLosingGamePiece(movingGamePiece, gamePieceInTile, winningGamePiece);

		updateLosingPlayerWithFightResults(losingGamePiece);
		updateBoardWithFightResults(movingGamePiece, winningGamePiece, losingGamePiece, gameMove);
		
		moveOccurred = true;
	}
}

void Game::updateBoardWithFightResults(GamePiece * const movingGamePiece, GamePiece * const winningGamePiece, GamePiece * const losingGamePiece, const GameMove & gameMove) {
	board.setGamePieceDead(losingGamePiece->getId());
	
	if (winningGamePiece == movingGamePiece)
		updateGamePieceMove(movingGamePiece, gameMove);
	else
		removeGamePieceFromTile(movingGamePiece, gameMove);
}

GamePiece * Game::getLosingGamePiece(GamePiece * const gp1, GamePiece * const gp2, GamePiece * winningGamePiece) {
	if (gp1 == winningGamePiece)
		return gp2;
	return gp1;
}

void Game::updateLosingPlayerWithFightResults(GamePiece * const losingGamePiece) {
	int sideLoser = losingGamePiece->getSide();
	board.setPlayerCurrDirection(sideLoser, Directions::STOP);
	board.setPlayerCurrGamePiece(sideLoser, 0);	
}

void Game::printScoreBelowBoard() const {
    int numSpaces = 20;
	if (players[1].player->getScore() > 9)
		numSpaces--;
	if (players[0].player->getScore() > 9)
		numSpaces--;
	cout << players[0].player->getName() << ": " << players[0].player->getScore();
	for (int i = 0; i < numSpaces; i++)
		cout << " ";
	cout << players[1].player->getName() << ": " << players[1].player->getScore() << endl;
}

void Game::getFilesInDir() {
	int strMax, maxRandom = 0, currRandom;
	char buffer[4096], *str, *nextToken;
	string dirCommand = "dir /a-d /on /b 2>nul ", fileName, generalRandomBoard(RANDOM_BOARD_FILE_PREFIX), lastRandomBoard_full, lastRandomBoard_short;
	bool randomBoardFileFound = false;

	dirCommand += cmdConfig.getPath();
	FILE * fp = popen(dirCommand.c_str(), "r");

	while (fgets(buffer, 4095, fp)) {
		strMax = strlen(buffer);

		fileName = string(buffer, strlen(buffer) - 1);

		str = strtok_r(buffer, ".", &nextToken);
		str = strtok_r(NULL, ".", &nextToken);
		str[strlen(str) - 1] = '\0';

		if (strcmp(str, BOARD_FILE_TYPE) == 0) {
			boardFiles.push(fileName);
			if (fileName.compare(0, generalRandomBoard.size(), generalRandomBoard) == 0) {
				currRandom = getNumberOfRandomFile(fileName);
				if (currRandom > maxRandom) {
					maxRandom = currRandom;
					lastRandomBoard_full = fileName;
					randomBoardFileFound = true;
				}
			}
		}
		if (strcmp(str, MOVES_A_FILE_TYPE) == 0 && typeid(*players[0].player) == typeid(FilePlayer))
			((FilePlayer*)players[0].player)->addFileNameToQueue(fileName);
		if (strcmp(str, MOVES_B_FILE_TYPE) == 0 && typeid(*players[1].player) == typeid(FilePlayer))
			((FilePlayer*)players[1].player)->addFileNameToQueue(fileName);
	}
	if (randomBoardFileFound && false == cmdConfig.isBoardFileInUse()) {
		lastRandomBoard_short = removePostfixFromFileName(lastRandomBoard_full, BOARD_FILE_TYPE);
		boardFileName = lastRandomBoard_short;
	}

	pclose(fp);
	return;
}

int Game::getNumberOfRandomFile(const string fileName) const {
	string shortFileName, numStr;
	int res;

	shortFileName = removePostfixFromFileName(fileName, BOARD_FILE_TYPE);
	numStr = shortFileName.substr(strlen(RANDOM_BOARD_FILE_PREFIX), string::npos);
	res = convertStrToNum(numStr.c_str());
	return res;
}

string Game::generateNewRandomFileName() const {
	string numFile, randomBoardPrefix(RANDOM_BOARD_FILE_PREFIX), fileName;
	int num;
	char * numStr;

	numFile = boardFileName.substr(randomBoardPrefix.size());

	numStr = new char[numFile.size() + 1];
	strncpy(numStr, numFile.c_str(), numFile.size());
	numStr[numFile.size()] = '\0';

	num = convertStrToNum(numStr) + 1;
	numFile = convertNumToStr(num);
	fileName = randomBoardPrefix + numFile;

	delete[] numStr;
	return fileName;
}

//--------------------------- KEYBOARD GAME ---------------------------//

GameOptions Game::mainMenu(bool & playersSwapped) {
	int choice = 0;

	clearScreen();
	cout << "Score:" << endl <<
		"------" << endl <<
		players[0].player->getName() << ": " << players[0].player->getScore() << "            " <<
		players[1].player->getName() << ": " << players[1].player->getScore() << endl << endl;

	cout << "Main Menu:" << endl <<
		"----------" << endl <<
		"1. Choose names" << endl <<
		"2. Start a game" << endl <<
		"3. Start a reversed game" << endl <<
		"4. Clear score" << endl;
	cmdConfig.isRecording() == true ? cout << "5. Stop game recording" << endl : cout << "5. Record game" << endl;
	cout << "9. Leave game" << endl;
	cin >> choice;

	switch (choice) {
	case 1:
		receiveNamesFromPlayers();
		break;
	case 2:
		return GameOptions::NEW;
		break;
	case 3:
		swapPlayers();
		playersSwapped = true;
		return GameOptions::NEW;
		break;
	case 4:
		players[0].player->clearScore();
		players[1].player->clearScore();
		break;
	case 5:
		cmdConfig.switchRecordOnOff();
		break;
	case 9:
		return GameOptions::EXIT;
		break;
	default:
		cout << "Selected option is not in menu." << endl;
		break;
	}
	getchar();
	//options 1/4/default were selected
	return GameOptions::MAIN_MENU;
}

//--------------------main menu: option 1------------------------------

void Game::receiveNamesFromPlayers() {
	string currName;
	cin.get();
	//getting name input from each player
	for (int i = 0; i < 2; i++) {
		cout << endl << "Enter a name for player " << (char)('A' + i) << endl;
		getline(cin, currName);
		players[i].player->setName(currName);
	}

}

//--------------------main menu: option 3------------------------------

void Game::swapPlayers() {
	string tempName = players[0].player->getName();
	int tempScore = players[0].player->getScore();

	players[0].player->setName(players[1].player->getName());
	players[0].player->setScore(players[1].player->getScore());

	players[1].player->setName(tempName);
	players[1].player->setScore(tempScore);
}

//--------------------ESC menu-----------------------------------------

GameOptions Game::escMenu()
{
	char choice;
	bool legalKey = false;

	clearScreen();
	cout << "Esc Menu:" << endl <<
		"---------" << endl <<
		"1. Resume game" << endl <<
		"2. Start over the same game" << endl <<
		"8. Go to main menu" << endl <<
		"9. Leave game" << endl;

	while (!legalKey) {
		cin >> choice;
		switch (choice) {
		case '1':
			clearScreen();
			board.drawBoard();
			printScoreBelowBoard();
			return GameOptions::CONTINUE;
		case '2':
			return GameOptions::NEW;
		case '8':
			clearScreen();
			return GameOptions::MAIN_MENU;
		case '9':
			return GameOptions::EXIT;
		default:
			cout << "Selected option is not in menu." << endl;
		}
	}
	return GameOptions::CONTINUE;
}

//--------------------------- FILES GAME ---------------------------//

bool Game::noAvailableMoves() const {
	bool res = false;

	if (typeid(*players[0].player) == typeid(FilePlayer)) {
		res = ((FilePlayer*)(players[0].player))->noAvailableMoves() &&
			((FilePlayer*)(players[1].player))->noAvailableMoves() &&
			board.getPlayerCurrDirection(0) == Directions::STOP &&
			board.getPlayerCurrDirection(1) == Directions::STOP;
	}

	return res;
}
