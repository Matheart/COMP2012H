#include <iostream> /* File: temperature.h */
#include <cstdlib>
using namespace std;
const char CELSIUS = 'C', FAHRENHEIT = 'F';

class temperature
{
private:
    char scale;
    double degree;
public:
    // CONSTRUCTOR member functions
    temperature(); // Default constructor
    temperature(double d, char s);
    // ACCESSOR member functions: don't modify data
    char get_scale() const;
    double get_degree() const;
    void print() const;
    // MUTATOR member functions: will modify data
    void set(double d, char s);
    void fahrenheit(); // Convert to the Fahrenheit scale
    void celsius(); // Convert to the Celsius scale
};