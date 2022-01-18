#include <iostream> /* File: odd.cpp */
#include <cstdlib>
using namespace std;
/* Constants definitions */
const int MAX_CALLS = 100;
/* Global variable declarations */
extern int num_calls; // "extern" is a must for global variables
/* Function declarations */
extern bool even(int); // "extern" is optional for functions

bool odd(int x)
{
    if (++num_calls > MAX_CALLS)
    {
        cout << "max #calls exceeded\n"; exit(-1);
    }
    return (x == 0) ? false : even(x-1);
}