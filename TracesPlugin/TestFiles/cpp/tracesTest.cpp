/* Test file for Traces Plugin */

#include "tracesTest_cpp.h"

House::~House() { }

House::House() { }

void House::forFunc(int input) {
   int a = 3, res = 0;
   cout << "10" << endl;

   for (int i = 0; i < length; ++i) {
       a += 1;
       for (int j = 1; j < input; ++j) {
           res += j * a;
       }
   }
}

void House::whileFunc(int input) {
   int a = 14, res = 0;
   cout << "11" << endl;

   while (a > input) {
     int b = 7;

     while (b + 1 < 10) {
       b++;
     }
     a--;
   }
}

void House::ifFunc(int input) {
    int a = 3, b = 77;
    cout << "12" << endl;

    if (input > 10) {
        cout << "hi!" << endl;
    }
    else if (b < 100) {
        cout << "hey:)" << endl;
    }
    else {
        if (a - 2 > 0) {
            cout << "hello..." << endl;
        }
        else if (b / 7 < 10) {
            cout << "hayush" << endl;
        }
        else {
            cout << b << endl;
        }
    }
}

void House::switchFunc(int input) {
    int a = 30, b = 12;
    cout << "13" << endl;

    switch (input) {
      case 70:
      cout << "hi" << endl;
      break;
      case 18:
      cout << "hello" << endl;
      break;
      case 100:
      cout << "bye" << endl;
      default:
      switch (b) {
        case 201:
        cout << "abcd" << endl;
        case 12:
        cout << "1234" << endl;
        break;
        default:
        cout << "wxyz" << endl;
        break;
      }
    }
}
