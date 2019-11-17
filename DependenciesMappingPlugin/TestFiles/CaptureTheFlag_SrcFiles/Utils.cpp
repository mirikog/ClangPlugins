/* Test file for Dependencies Mapping Plugin -
 Utils.cpp */

#include "Utils.h"

void clearScreen() {
	system("cls");
}

int convertStrToNum(const char * str) {
	int res = 0;

	while (*str != '\0') {
		res *= 10;
		res += *str - '0';
		str++;
	}
	return res;
}

string removePostfixFromFileName(const string fileName, const string postfix) {
	return fileName.substr(0, fileName.size() - postfix.size() - 1);
}

string convertNumToStr(int num) {
	string res, lsd;

	while (num) {
		lsd = '0' + num % 10;
		res = lsd + res;
		num /= 10;
	}
	return res;
}
