#include <iostream>
#include <cstring>
using namespace std;

const int MAX_NAME_LEN = 32;

class student_record{
private:
    char gender;
    char name[MAX_NAME_LEN];
    unsigned int id;
public:
    // ACCESSOR member functions: const => won't modify data members
    const char* get_name() const {return name;}
    unsigned int get_id() const {return id;}
    void print() const{
        cout << name << endl << id << endl << gender << endl;
    }

    // MUTATOR member functions
    void set(const char my_name[], unsigned int my_id, char my_gender){
        strcpy(name, my_name); id = my_id; gender = my_gender;
    }
    void copy(const student_record& r){
        set(r.name, r.id, r.gender);
    }
};