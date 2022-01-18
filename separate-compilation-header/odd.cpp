#include "my_include.h" /* File: odd.cpp */
#include "extern.h"
bool odd(int x)
{
    if (++num_calls > MAX_CALLS)
    {
        cout << "max #calls exceeded\n";
        exit(-1);
    }
    return (x == 0) ? false : even(x-1);
}