/* Test file for Traces Plugin */

#include "tracesTest_c.h"

void forFunc(int input) {
   int a = 3, res = 0;
   printf("%d\n", 10);

   for (int i = 0; i < input * 2; ++i) {
       a += 1;
       for (int j = 1; j < input; ++j) {
           res += j * a;
       }
   }
}

void whileFunc(int input) {
   int a = 14, res = 0;
   printf("%d\n", 11);

   while (a > input) {
     int b = 7;

     while (b + 1 < 10) {
       b++;
     }
     a--;
   }
}

void ifFunc(int input) {
    int a = 3, b = 77;
    printf("%d\n", 12);

    if (input > 10) {
        printf("%s\n", "aaa");
    }
    else if (b < 100) {
        printf("%s\n", "bbb");
    }
    else {
        if (a - 2 > 0) {
            printf("%s\n", "ccc");
        }
        else if (b / 7 < 10) {
            printf("%s\n", "ddd");
        }
        else {
            printf("%d\n", b);
        }
    }
}

void switchFunc(int input) {
    int a = 30, b = 12;
    printf("%d\n", 13);

    switch (input) {
      case 70:
      printf("%s\n", "eee");
      break;
      case 18:
      printf("%s\n", "fff");
      break;
      case 100:
      printf("%s\n", "ggg");
      default:
      switch (b) {
        case 201:
        printf("%s\n", "hhh");
        case 12:
        printf("%s\n", "iii");
        break;
        default:
        printf("%s\n", "jjj");
        break;
      }
    }
}
