#include <iostream> /* File: main.cpp */
using namespace std;
/* Constant definitions */
const int MAX_CALLS = 100;
/* Global variable definition */
int num_calls;
/* Function declarations */
extern bool odd(int); // "extern" is optional for functions

int main()
{
    int x;
    while (cin >> x) // Assume x > 0
    {
        num_calls = 0; cout << boolalpha << odd(x) << endl;
    }
    return 0;
}
// -c: compile from source code to object file
// -o: linking