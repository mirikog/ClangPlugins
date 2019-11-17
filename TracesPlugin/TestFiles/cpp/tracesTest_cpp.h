/* Test file for Traces Plugin */

#ifndef Traces_h
#define Traces_h

#include <iostream>
#include <string>

using namespace std;

class House {
    string name = "avi";
    int length = 2, width = 5;
    double scoreIfClean = 0;

public:
    ~House();
    House();
    void forFunc(int input);
    void whileFunc(int input);
    void ifFunc(int input);
    void switchFunc(int input);
};

#endif /* Traces_h */
