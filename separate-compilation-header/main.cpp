#include "my_include.h" /* File: main.cpp */
#include "global.h"
int main()
{
    int x;
    while (cin >> x) // assume x > 0
    {
        num_calls = 0;
        cout << boolalpha << odd(x) << endl;
    }
    return 0;
}