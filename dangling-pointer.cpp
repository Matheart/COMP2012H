#include <iostream> /* File: dangling-pointer.cpp */
#include <cstdio>
using namespace std;

int* create_and_init(int value) 
{
    int x = value; // x is a local variable
    int* p = &x; // p is also a local variable
    return p;
} // x gets deallocated after the function call 

int main()
{
    int* ip = create_and_init(10);
    cout << *ip << endl;
    return 0;
}