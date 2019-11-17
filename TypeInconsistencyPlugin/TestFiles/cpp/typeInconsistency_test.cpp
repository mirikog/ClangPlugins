/* Test file for Type Inconsistency Plugin - 
 check mismatches between:
 (1) types defined with typdef and original types
 (2) types defined with 'using' and original types
 (3) bool and int 
 
 26 warnings are expected */

#include <iostream>
#include <vector>
using namespace std;

/*----- ctor example start -----*/

class Point {
private:
    int x;
    int y;
public:
    Point(int x_in, int y_in) {
        x = x_in;
        y = y_in;
    }
    Point(int xy): x(xy), y(xy) {};
};

Point foo() {
    return true;
}

Point goo() {
    return 3;
}

/*----- ctor example end -----*/

/*----- type definitions -----*/

class Base {
private:
    int a;
public:
    Base(int a_in) {
        a = a_in;
    }
};

class Derived : public Base {
private:
    int b;
public:
    Derived(int a_in, int b_in) : Base(a_in) {
        b = b_in;
    }
};

typedef int typedefInt;
using usingVector = vector<int>;
template <typename T> using V = std::vector<T*>;

/*----- functions definitions -----*/

/**** bool ****/
/* mismatch in assignment */
int func2() {
    return 10;
}

/* mismatch in function arguemnt */
void func1(bool a) {
    return;
}

/* mismatch in return value */
bool func4() {
    int a = 9;
    return a;   
}

bool func5() {
    return func2();
}

bool func6() {
    return 19;         
}

/**** typedef ****/
/* mismatch in assignment */
int func12() {
    return 10;
}

/* mismatch in function arguemnt */
void func11(typedefInt a) {
    return;
}

/* mismatch in return value */
typedefInt func14() {
    int a = 9;
    return a;   
}

typedefInt func15() {
    return func12();        
}

typedefInt func16() {
    return 19;  
}

/**** using ****/
/* mismatch in assignment */
vector<int> func22() {
    vector<int> v;
    return v;
}

/* mismatch in function arguemnt */
void func21(usingVector a) {
    return;
}

/* mismatch in return value */
usingVector func24() {
    vector<int> v;
    return v;        
}

usingVector func25() {
    return func22(); 
}

/*----- main implementation -----*/

int main() {
    cout << "hi!" << endl;
    
    int i_int = 1;
    typedefInt t_typedefInt;
    bool b_bool;
    vector<int> v_vector;
    usingVector u_usingVectour, w_usingVectour;
    Derived *d = new Derived(17, 19);
    Base *b;
    V<char> usingTemplate;
    vector<char*> charPtrVector;
    char c_char = 't';
    long l_long;

    /* OK assignments */
    b = d;       
    usingTemplate.push_back(&c_char);

    /*** bool ***/
    /* mismatch in assignment */ 
    b_bool = 9; 
    b_bool = i_int;
    b_bool = func2();       

    /* mismatch in function argument */ 
    func1(54);  
    func1(func2());
    func1(i_int);  

    /*** typedef ***/
    /* mismatch in assignment */ 
    t_typedefInt = 9;       
    t_typedefInt = i_int;   
    t_typedefInt = func12();

    /* mismatch in function argument */
    func11(54); 
    func11(func12());       
    func11(i_int); 

    /*** using ***/
    /* mismatch in assignment */ 
    w_usingVectour = v_vector;
    w_usingVectour = func22();
    charPtrVector = usingTemplate;

    /* mismatch in function argument */
    func21(func22());
    func21(v_vector);

    /* OK assignments */ 
    w_usingVectour = u_usingVectour;
    l_long = i_int;
    i_int = (int) l_long;
    i_int = l_long;

    auto v1 = 88;
    auto v2 = &c_char;
    auto v3 = l_long;
    
    foo();

    delete d;
    cout << "bye!" << endl;
    return 0;
}
