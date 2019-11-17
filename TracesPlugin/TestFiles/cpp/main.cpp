/* Test file for Traces Plugin */

#include <iostream>
#include "tracesTest_cpp.h"

int main(int argc, const char * argv[]) {
    cout << "start main" << endl;
    House h;
    h.forFunc(10);
    h.whileFunc(10);
    h.ifFunc(10);
    h.switchFunc(10);
    cout << "exit main" << endl;
    return 0;
}
