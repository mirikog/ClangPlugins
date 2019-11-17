/* Test file for Traces Plugin */

#include <stdio.h>
#include "tracesTest_c.h"

int main(int argc, const char * argv[]) {
    printf("%s\n", "start main");
    forFunc(10);
    whileFunc(10);
    ifFunc(10);
    switchFunc(10);
    printf("%s\n", "exit main");
    return 0;
}
