#include <iostream>
using namespace std;
#include "student-record.h"

int main(){
    student_record amy, bob;
    amy.set("Amy", 12345, 'F');
    bob.set("Bob", 34567, 'M');

    cout << amy.get_id() << endl;
    auto a = amy.get_name();
    cout << a << endl;

    amy.copy(bob);
    amy.print();

    return 0;
}