#include "my_include.h" /* File: even.cpp */
#include "extern.h"
bool even(int x)
{
    if (++num_calls > MAX_CALLS)
    {
        cout << "max #calls exceeded\n";
        exit(-1);
    }
    return (x == 0) ? true : odd(x-1);
}