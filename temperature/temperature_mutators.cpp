#include "temperature.h" /* File: temperature_mutators.cpp */

void temperature::set(double d, char s)
{
    degree = d; scale = toupper(s); // lower case -> upper case
    if (scale != CELSIUS && scale != FAHRENHEIT)
    { cout << "Bad temperature scale: " << scale << endl; exit(-1); }
}

void temperature::fahrenheit() // Conversion to the Fahrenheit scale
{
    if (scale == CELSIUS)
    { degree = degree*9.0/5.0 + 32.0; scale = FAHRENHEIT; }
}

void temperature::celsius() // Conversion to the Celsius scale
{
    if (scale == FAHRENHEIT)
    { degree = (degree - 32.0)*5.0/9.0; scale = CELSIUS; }
}