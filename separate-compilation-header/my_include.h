/* File: my_include.h */
/* Include system or user-defined header files */
#include <iostream>
#include <cstdlib>
using namespace std;
/* Constant definitions */
const int MAX_CALLS = 100;
/* External function declarations */
extern bool odd(int); // "extern" is optional for functions
extern bool even(int);