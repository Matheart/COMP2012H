#include <iostream> /* File: complex-test.cpp */
using namespace std;
#include "complex.h"

int main()
{
    Complex y(3, 4); y.print();
    cout << endl << "Return by value" << endl;
    Complex x(1, 2); x.print();
    x.add1(y).add1(y).print();
    x.print(); // for x, only add1 one time, the second time is exerted on the result of x.add1(y)
    cout << endl << "Return its pointer by value" << endl;
    x.add2(y)->print();
    x.print();
    cout << endl << "Return by reference" << endl;
    Complex z(1, 2); z.print();
    z.add3(y).add3(y).print();
    z.print();
    return 0;
}