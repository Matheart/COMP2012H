#include "temperature.h" /* File: temperature_test.cpp */

int main()
{
    char scale;
    double degree;
    temperature x; // Use default constructor
    x.print(); cout << endl; // Check the default values
    cout << "Enter temperature (e.g., 98.6 F): ";
    while (cin >> degree >> scale)
    {
        x.set(degree, scale);
        x.fahrenheit(); x.print(); cout << endl;// Convert to Fahrenheit
        x.celsius(); x.print(); cout << endl; // Convert to Celsius
        cout << endl << "Enter temperature (e.g., 98.6 F): ";
    };
    return 0;
}