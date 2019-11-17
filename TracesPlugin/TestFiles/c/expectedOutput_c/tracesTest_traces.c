/* Test file for Traces Plugin */

#include "tracesTest_c.h"

#include <stdbool.h>
#include <stdio.h>

#define __TRACE__

void forFunc(int input) {
#ifdef __TRACE__	/* traces - begin */
   printf("\n\e[1;30m>>> program reached: %s, 6 <<<\e[0m \n" , __FILE__);
   printf("\e[1;30m>>> function: \e[1;31mforFunc\e[1;30m <<<\e[0m\n\n");
#endif				/*  traces - end  */
#ifdef __TRACE__	/* traces - begin */
   bool traces_inForLoop_1 = false;
#endif				/*  traces - end  */
#ifdef __TRACE__	/* traces - begin */
   bool traces_inForLoop_2 = false;
#endif				/*  traces - end  */
   int a = 3, res = 0;
   printf("%d\n", 10);

   for (int i = 0; i < input * 2; ++i) {
#ifdef __TRACE__	/* traces - begin */
       if (traces_inForLoop_1 == false)
       {
           traces_inForLoop_1 = true;
           printf("\n\e[1;30m>>> program reached: %s, 10 <<<\e[0m \n" , __FILE__);
           printf("\e[1;30m>>> for loop: \e[1;34m(i < input * 2)\e[1;30m <<< \e[0m\n\n");
       }
#endif				/*  traces - end  */
       a += 1;
       for (int j = 1; j < input; ++j) {
#ifdef __TRACE__	/* traces - begin */
           if (traces_inForLoop_2 == false)
           {
               traces_inForLoop_2 = true;
               printf("\n\e[1;30m>>> program reached: %s, 12 <<<\e[0m \n" , __FILE__);
               printf("\e[1;30m>>> for loop: \e[1;34m(j < input)\e[1;30m <<< \e[0m\n\n");
           }
#endif				/*  traces - end  */
           res += j * a;
       }
   }
}

void whileFunc(int input) {
#ifdef __TRACE__	/* traces - begin */
   printf("\n\e[1;30m>>> program reached: %s, 18 <<<\e[0m \n" , __FILE__);
   printf("\e[1;30m>>> function: \e[1;31mwhileFunc\e[1;30m <<<\e[0m\n\n");
#endif				/*  traces - end  */
#ifdef __TRACE__	/* traces - begin */
   bool traces_inWhileLoop_1 = false;
#endif				/*  traces - end  */
#ifdef __TRACE__	/* traces - begin */
   bool traces_inWhileLoop_2 = false;
#endif				/*  traces - end  */
   int a = 14, res = 0;
   printf("%d\n", 11);

   while (a > input) {
#ifdef __TRACE__	/* traces - begin */
     if (traces_inWhileLoop_1 == false)
     {
         traces_inWhileLoop_1 = true;
         printf("\n\e[1;30m>>> program reached: %s, 22 <<<\e[0m \n" , __FILE__);
         printf("\e[1;30m>>> while loop: \e[1;32m(a > input)\e[1;30m <<<\e[0m\n\n");
     }
#endif				/*  traces - end  */
     int b = 7;

     while (b + 1 < 10) {
#ifdef __TRACE__	/* traces - begin */
       if (traces_inWhileLoop_2 == false)
       {
           traces_inWhileLoop_2 = true;
           printf("\n\e[1;30m>>> program reached: %s, 25 <<<\e[0m \n" , __FILE__);
           printf("\e[1;30m>>> while loop: \e[1;32m(b + 1 < 10)\e[1;30m <<<\e[0m\n\n");
       }
#endif				/*  traces - end  */
       b++;
     }
     a--;
   }
}

void ifFunc(int input) {
#ifdef __TRACE__	/* traces - begin */
    printf("\n\e[1;30m>>> program reached: %s, 32 <<<\e[0m \n" , __FILE__);
    printf("\e[1;30m>>> function: \e[1;31mifFunc\e[1;30m <<<\e[0m\n\n");
#endif				/*  traces - end  */
    int a = 3, b = 77;
    printf("%d\n", 12);

    if (input > 10) {
#ifdef __TRACE__	/* traces - begin */
        printf("\n\e[1;30m>>> program reached: %s, 36 <<<\e[0m \n" , __FILE__);
        printf("\e[1;30m>>> if statement: \e[1;35m(input > 10)\e[1;30m, 'then' clause <<<\e[0m\n\n");
#endif				/*  traces - end  */
        printf("%s\n", "aaa");
    }
    else if (b < 100) {
#ifdef __TRACE__	/* traces - begin */
        printf("\n\e[1;30m>>> program reached: %s, 39 <<<\e[0m \n" , __FILE__);
        printf("\e[1;30m>>> if statement: \e[1;35m(b < 100)\e[1;30m, 'then' clause <<<\e[0m\n\n");
#endif				/*  traces - end  */
        printf("%s\n", "bbb");
    }
    else {
#ifdef __TRACE__	/* traces - begin */
        printf("\n\e[1;30m>>> program reached: %s, 42 <<<\e[0m \n" , __FILE__);
        printf("\e[1;30m>>> if statement: \e[1;35m(b < 100)\e[1;30m, 'else' clause <<<\e[0m\n\n");
#endif				/*  traces - end  */
        if (a - 2 > 0) {
#ifdef __TRACE__	/* traces - begin */
            printf("\n\e[1;30m>>> program reached: %s, 43 <<<\e[0m \n" , __FILE__);
            printf("\e[1;30m>>> if statement: \e[1;35m(a - 2 > 0)\e[1;30m, 'then' clause <<<\e[0m\n\n");
#endif				/*  traces - end  */
            printf("%s\n", "ccc");
        }
        else if (b / 7 < 10) {
#ifdef __TRACE__	/* traces - begin */
            printf("\n\e[1;30m>>> program reached: %s, 46 <<<\e[0m \n" , __FILE__);
            printf("\e[1;30m>>> if statement: \e[1;35m(b / 7 < 10)\e[1;30m, 'then' clause <<<\e[0m\n\n");
#endif				/*  traces - end  */
            printf("%s\n", "ddd");
        }
        else {
#ifdef __TRACE__	/* traces - begin */
            printf("\n\e[1;30m>>> program reached: %s, 49 <<<\e[0m \n" , __FILE__);
            printf("\e[1;30m>>> if statement: \e[1;35m(b / 7 < 10)\e[1;30m, 'else' clause <<<\e[0m\n\n");
#endif				/*  traces - end  */
            printf("%d\n", b);
        }
    }
}

void switchFunc(int input) {
#ifdef __TRACE__	/* traces - begin */
    printf("\n\e[1;30m>>> program reached: %s, 55 <<<\e[0m \n" , __FILE__);
    printf("\e[1;30m>>> function: \e[1;31mswitchFunc\e[1;30m <<<\e[0m\n\n");
#endif				/*  traces - end  */
    int a = 30, b = 12;
    printf("%d\n", 13);

    switch (input) {
      case 70:
#ifdef __TRACE__	/* traces - begin */
      printf("\n\e[1;30m>>> program reached: %s, 60 <<<\e[0m \n" , __FILE__);
      printf("\e[1;30m>>> switch statement: (input), \e[1;36mcase 70\e[1;30m <<<\e[0m\n\n");
#endif				/*  traces - end  */
      printf("%s\n", "eee");
      break;
      case 18:
#ifdef __TRACE__	/* traces - begin */
      printf("\n\e[1;30m>>> program reached: %s, 63 <<<\e[0m \n" , __FILE__);
      printf("\e[1;30m>>> switch statement: (input), \e[1;36mcase 18\e[1;30m <<<\e[0m\n\n");
#endif				/*  traces - end  */
      printf("%s\n", "fff");
      break;
      case 100:
#ifdef __TRACE__	/* traces - begin */
      printf("\n\e[1;30m>>> program reached: %s, 66 <<<\e[0m \n" , __FILE__);
      printf("\e[1;30m>>> switch statement: (input), \e[1;36mcase 100\e[1;30m <<<\e[0m\n\n");
#endif				/*  traces - end  */
      printf("%s\n", "ggg");
      default:
#ifdef __TRACE__	/* traces - begin */
      printf("\n\e[1;30m>>> program reached: %s, 68 <<<\e[0m \n" , __FILE__);
      printf("\e[1;30m>>> switch statement: (input), \e[1;36mdefault\e[1;30m <<<\e[0m\n\n");
#endif				/*  traces - end  */
      switch (b) {
        case 201:
#ifdef __TRACE__	/* traces - begin */
        printf("\n\e[1;30m>>> program reached: %s, 70 <<<\e[0m \n" , __FILE__);
        printf("\e[1;30m>>> switch statement: (b), \e[1;36mcase 201\e[1;30m <<<\e[0m\n\n");
#endif				/*  traces - end  */
        printf("%s\n", "hhh");
        case 12:
#ifdef __TRACE__	/* traces - begin */
        printf("\n\e[1;30m>>> program reached: %s, 72 <<<\e[0m \n" , __FILE__);
        printf("\e[1;30m>>> switch statement: (b), \e[1;36mcase 12\e[1;30m <<<\e[0m\n\n");
#endif				/*  traces - end  */
        printf("%s\n", "iii");
        break;
        default:
#ifdef __TRACE__	/* traces - begin */
        printf("\n\e[1;30m>>> program reached: %s, 75 <<<\e[0m \n" , __FILE__);
        printf("\e[1;30m>>> switch statement: (b), \e[1;36mdefault\e[1;30m <<<\e[0m\n\n");
#endif				/*  traces - end  */
        printf("%s\n", "jjj");
        break;
      }
    }
}
