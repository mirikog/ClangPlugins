/* Test file for Dependencies Mapping Plugin -
 Utils.h */

#pragma once
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
using namespace std;

enum { BLACK, DARK_BLUE, DARK_GREEN, DARK_CYAN, DARK_RED, DARK_PURPLE, OLIVE, WHITE, GREY, BLUE, GREEN, CYAN, RED, PURPLE, YELLOW };

void clearScreen();
string convertNumToStr(const int num);
int convertStrToNum(const char * str);
string removePostfixFromFileName(const string fileName, const string postfix);
