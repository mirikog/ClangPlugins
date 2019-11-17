/* Test file for Dependencies Mapping Plugin -
 CmdConfig.h */

#pragma once
#include <iostream>
#include <string>
#include "Utils.h"
using namespace std;

#define MAX_ITERATIONS 1250;

class CmdConfig {
	bool boardFileInUse;
	char movesSource; // 'k': keyboard, 'a': algorithm, 'f': file
	int numberOfRounds;
	long int maxNumOfiterations; // relevant for algorithm game
	string path;	// default is working directory path, path doesn't have a '/' at the end
	bool quiet;		// true if quiet was in the command line arguments + board and moves were set to file mode
	int delay;		// if quiet mode is on, delay is set to 0
	bool recordGame;

	const string getWorkingDirectorty() const;

	int setPath(const string& inputPath);

	int setBoardFileInput(const char ch);

	int setMovesFileInput(const char ch);

public:
	CmdConfig();

	bool			isBoardFileInUse() const			{ return boardFileInUse; }
	char			getMovesSource() const				{ return movesSource; }
	const string&	getPath() const						{ return path; }
	bool			isQuiet() const						{ return quiet; }
	int				getDelay() const					{ return delay; }
	bool			isRecording() const					{ return recordGame; }
	void			setQuiet(const bool bit)			{ quiet = bit; }
	void			setDelay(const int _delay)			{ delay = _delay; }
	void			switchRecordOnOff()					{ recordGame = !recordGame; }
	void			decreaseNumberOfRoundsByOne()		{ numberOfRounds--; }
	int				getNumberOfRoundLeft() const 		{ return numberOfRounds; }
	long int		getMaxNumberOfItterations() const	{ return maxNumOfiterations; }

	const int parseCmdConfig(const int argc, char ** const argv);
private:

};
