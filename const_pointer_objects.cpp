#include <iostream>

int main(){
    int *p1; // can change the object it points to, also can change the value
    int* const p2; // can change the value but cannot change the object it points to
    const int *p3; 
    int a = 3;
    const int* const p4 = &a;
    *p4 = 5;
}