/* Test file for Type Inconsistency Plugin - 
 check mismatches between enum and int 
 
 17 warnings are expected */

#include <stdio.h>

enum Color { red, green, blue };
enum ColorA { pink, yellow };

/* mismatch in return value */
enum Color func1() {
    unsigned int a = 7;
    return a;
}

enum Color func2() {
    return 3;
}

unsigned int func3() {
    return red;
}

unsigned int func4() {
    enum Color b = blue;
    return b;
}

void func5(enum Color c) {
    unsigned int t = 8;
}

void func6(unsigned int n) {
    unsigned int r = 55;
}


int main() {
    enum Color c;
    unsigned int a = 256;
    unsigned char ch = 3;
    
    printf("hello!\n");
    
    /* mismatch in assignment */
    c = blue;
    c = 1;
    a = c;
    a = red;
    c = a;
    c = ch;
    
    a = func1();
    a = func2();
    c = func3();
    c = func4();
    
    /* mismatch in function argument */
    func5(0);
    func5(a);
    func6(blue);
    func6(c);

        
    printf("goodbye!\n");
    return 0;
}
