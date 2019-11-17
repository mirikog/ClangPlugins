/* Test file for Dependencies Mapping Plugin -
 CMdConfig.cpp */

#include "CmdConfig.h"

const int CmdConfig::parseCmdConfig(const int argc, char ** const argv) {

	for (int i = 1; i < argc; i++) {
		if (strcmp("-board", argv[i]) == 0) {
			if (argc == i + 1 || !setBoardFileInput(*argv[i + 1])) {
				cout << "Board argument is incorrect" << endl;
				return 0;
			}
			if (argc != i + 2 && *argv[i + 2] >= '0' && *argv[i + 2] <= '9')
				numberOfRounds = convertStrToNum(argv[i + 2]);
		}
		else if (strcmp("-moves", argv[i]) == 0) {
			if (argc == i + 1 || !setMovesFileInput(*argv[i + 1])) {
				cout << "Moves argument is inncorrect" << endl;
				return 0;
			}
		}
		else if (strcmp("-path", argv[i]) == 0) {
			if (argc == i + 1 || !setPath(argv[i + 1])) {
				cout << "Path argument is inncorrect" << endl;
				return 0;
			}
		}
		else if (strcmp("-delay", argv[i]) == 0) {
			if (argc == i + 1) {
				cout << "Delay argument is inncorrect" << endl;
				return 0;
			}
			else
				delay = convertStrToNum(argv[i + 1]);
		}
		else if (strcmp("-quiet", argv[i]) == 0)
			quiet = true;
		else if (strcmp("-save", argv[i]) == 0)
			recordGame = true;
	}


	// Seperated from loop because both board_input_method and move_input_method needs to be known
	if (!boardFileInUse || getMovesSource() == 'k')
		quiet = false;
	if (quiet)
		delay = 0;

	return 1;
}

CmdConfig::CmdConfig() {
	boardFileInUse = false;
	movesSource = 'k';
	numberOfRounds = 10;
	maxNumOfiterations = MAX_ITERATIONS;
	path = getWorkingDirectorty();
	quiet = false;
	delay = 20;
	recordGame = false;
}

const string CmdConfig::getWorkingDirectorty() const {
	char buffer[4096];
	FILE * fp = popen("cd", "r");
	string res;

	if (fgets(buffer, 4095, fp) != nullptr)
		res = buffer;
	return res.substr(0, res.size() - 1);
}

int CmdConfig::setPath(const string& inputPath) {
	string res;

	if (inputPath.find('\\') == string::npos) {
		path = res;
		return 0;
	}

	if (inputPath.find(":\\") != string::npos)
		res = inputPath;
	else {
		if (inputPath.find_first_of('\\', 0) == 0)
			res = path + inputPath;
		else
			res = path + "\\" + inputPath;
	}

	if (res.find('\\', res.size() - 1) != string::npos)
		res = res.substr(0, res.size() - 1);

	path = res;
	return 1;
}

int CmdConfig::setBoardFileInput(char ch)
{
	if ('f' == tolower(ch)) {
		boardFileInUse = true;
		return 1;
	}
	if ('r' == tolower(ch)) {
		boardFileInUse = false;
		return 1;
	}
	return 0;
}


int CmdConfig::setMovesFileInput(char ch)
{
	if ('f' == tolower(ch)) {
		movesSource = 'f';
		return 1;
	}
	if ('k' == tolower(ch)) {
		movesSource = 'k';
		return 1;
	}
	if ('a' == tolower(ch)) {
		movesSource = 'a';
		return 1;
	}
	return 0;
}




