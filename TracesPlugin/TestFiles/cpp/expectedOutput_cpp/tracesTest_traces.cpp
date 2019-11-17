/* Test file for Traces Plugin */

#include "tracesTest_cpp.h"

#include <iostream>

#define __TRACE__

House::~House() {
#ifdef __TRACE__	/* traces - begin */
    std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 5 <<<\e[0m" <<  std::endl;
    std::cout << "\e[1;30m>>> function: \e[1;31m~House\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
 }

House::House() {
#ifdef __TRACE__	/* traces - begin */
    std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 7 <<<\e[0m" <<  std::endl;
    std::cout << "\e[1;30m>>> function: \e[1;31mHouse\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
 }

void House::forFunc(int input) {
#ifdef __TRACE__	/* traces - begin */
   std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 10 <<<\e[0m" <<  std::endl;
   std::cout << "\e[1;30m>>> function: \e[1;31mforFunc\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
#ifdef __TRACE__	/* traces - begin */
   bool traces_inForLoop_1 = false;
#endif				/*  traces - end  */
#ifdef __TRACE__	/* traces - begin */
   bool traces_inForLoop_2 = false;
#endif				/*  traces - end  */
   int a = 3, res = 0;
   cout << "10" << endl;

   for (int i = 0; i < length; ++i) {
#ifdef __TRACE__	/* traces - begin */
       if (traces_inForLoop_1 == false)
       {
           traces_inForLoop_1 = true;
           std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 14 <<<\e[0m" <<  std::endl;
           std::cout << "\e[1;30m>>> for loop: \e[1;34m(i < length)\e[1;30m <<< \e[0m\n" << std::endl;
       }
#endif				/*  traces - end  */
       a += 1;
       for (int j = 1; j < input; ++j) {
#ifdef __TRACE__	/* traces - begin */
           if (traces_inForLoop_2 == false)
           {
               traces_inForLoop_2 = true;
               std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 16 <<<\e[0m" <<  std::endl;
               std::cout << "\e[1;30m>>> for loop: \e[1;34m(j < input)\e[1;30m <<< \e[0m\n" << std::endl;
           }
#endif				/*  traces - end  */
           res += j * a;
       }
   }
}

void House::whileFunc(int input) {
#ifdef __TRACE__	/* traces - begin */
   std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 22 <<<\e[0m" <<  std::endl;
   std::cout << "\e[1;30m>>> function: \e[1;31mwhileFunc\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
#ifdef __TRACE__	/* traces - begin */
   bool traces_inWhileLoop_1 = false;
#endif				/*  traces - end  */
#ifdef __TRACE__	/* traces - begin */
   bool traces_inWhileLoop_2 = false;
#endif				/*  traces - end  */
   int a = 14, res = 0;
   cout << "11" << endl;

   while (a > input) {
#ifdef __TRACE__	/* traces - begin */
     if (traces_inWhileLoop_1 == false)
     {
         traces_inWhileLoop_1 = true;
         std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 26 <<<\e[0m" <<  std::endl;
         std::cout << "\e[1;30m>>> while loop: \e[1;32m(a > input)\e[1;30m <<<\e[0m\n" << std::endl;
     }
#endif				/*  traces - end  */
     int b = 7;

     while (b + 1 < 10) {
#ifdef __TRACE__	/* traces - begin */
       if (traces_inWhileLoop_2 == false)
       {
           traces_inWhileLoop_2 = true;
           std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 29 <<<\e[0m" <<  std::endl;
           std::cout << "\e[1;30m>>> while loop: \e[1;32m(b + 1 < 10)\e[1;30m <<<\e[0m\n" << std::endl;
       }
#endif				/*  traces - end  */
       b++;
     }
     a--;
   }
}

void House::ifFunc(int input) {
#ifdef __TRACE__	/* traces - begin */
    std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 36 <<<\e[0m" <<  std::endl;
    std::cout << "\e[1;30m>>> function: \e[1;31mifFunc\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
    int a = 3, b = 77;
    cout << "12" << endl;

    if (input > 10) {
#ifdef __TRACE__	/* traces - begin */
        std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 40 <<<\e[0m" <<  std::endl;
        std::cout << "\e[1;30m>>> if statement: \e[1;35m(input > 10)\e[1;30m, 'then' clause <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
        cout << "hi!" << endl;
    }
    else if (b < 100) {
#ifdef __TRACE__	/* traces - begin */
        std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 43 <<<\e[0m" <<  std::endl;
        std::cout << "\e[1;30m>>> if statement: \e[1;35m(b < 100)\e[1;30m, 'then' clause <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
        cout << "hey:)" << endl;
    }
    else {
#ifdef __TRACE__	/* traces - begin */
        std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 46 <<<\e[0m" <<  std::endl;
        std::cout << "\e[1;30m>>> if statement: \e[1;35m(b < 100)\e[1;30m, 'else' clause <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
        if (a - 2 > 0) {
#ifdef __TRACE__	/* traces - begin */
            std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 47 <<<\e[0m" <<  std::endl;
            std::cout << "\e[1;30m>>> if statement: \e[1;35m(a - 2 > 0)\e[1;30m, 'then' clause <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
            cout << "hello..." << endl;
        }
        else if (b / 7 < 10) {
#ifdef __TRACE__	/* traces - begin */
            std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 50 <<<\e[0m" <<  std::endl;
            std::cout << "\e[1;30m>>> if statement: \e[1;35m(b / 7 < 10)\e[1;30m, 'then' clause <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
            cout << "hayush" << endl;
        }
        else {
#ifdef __TRACE__	/* traces - begin */
            std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 53 <<<\e[0m" <<  std::endl;
            std::cout << "\e[1;30m>>> if statement: \e[1;35m(b / 7 < 10)\e[1;30m, 'else' clause <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
            cout << b << endl;
        }
    }
}

void House::switchFunc(int input) {
#ifdef __TRACE__	/* traces - begin */
    std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 59 <<<\e[0m" <<  std::endl;
    std::cout << "\e[1;30m>>> function: \e[1;31mswitchFunc\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
    int a = 30, b = 12;
    cout << "13" << endl;

    switch (input) {
      case 70:
#ifdef __TRACE__	/* traces - begin */
      std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 64 <<<\e[0m" <<  std::endl;
      std::cout << "\e[1;30m>>> switch statement: (input), \e[1;36mcase 70\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
      cout << "hi" << endl;
      break;
      case 18:
#ifdef __TRACE__	/* traces - begin */
      std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 67 <<<\e[0m" <<  std::endl;
      std::cout << "\e[1;30m>>> switch statement: (input), \e[1;36mcase 18\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
      cout << "hello" << endl;
      break;
      case 100:
#ifdef __TRACE__	/* traces - begin */
      std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 70 <<<\e[0m" <<  std::endl;
      std::cout << "\e[1;30m>>> switch statement: (input), \e[1;36mcase 100\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
      cout << "bye" << endl;
      default:
#ifdef __TRACE__	/* traces - begin */
      std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 72 <<<\e[0m" <<  std::endl;
      std::cout << "\e[1;30m>>> switch statement: (input), \e[1;36mdefault\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
      switch (b) {
        case 201:
#ifdef __TRACE__	/* traces - begin */
        std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 74 <<<\e[0m" <<  std::endl;
        std::cout << "\e[1;30m>>> switch statement: (b), \e[1;36mcase 201\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
        cout << "abcd" << endl;
        case 12:
#ifdef __TRACE__	/* traces - begin */
        std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 76 <<<\e[0m" <<  std::endl;
        std::cout << "\e[1;30m>>> switch statement: (b), \e[1;36mcase 12\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
        cout << "1234" << endl;
        break;
        default:
#ifdef __TRACE__	/* traces - begin */
        std::cout << "\n\e[1;30m>>> program reached: " << __FILE__ << ", 79 <<<\e[0m" <<  std::endl;
        std::cout << "\e[1;30m>>> switch statement: (b), \e[1;36mdefault\e[1;30m <<<\e[0m\n" << std::endl;
#endif				/*  traces - end  */
        cout << "wxyz" << endl;
        break;
      }
    }
}
