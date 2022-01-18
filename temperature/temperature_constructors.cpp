/* File: temperature_constructors.cpp */
#include "temperature.h"

/* CONSTRUCTOR member functions */
// Default constructor
temperature::temperature()
{
    degree = 0.0;
    scale = CELSIUS;
}

// A general constructor
temperature::temperature(double d, char s)
{
    set(d, s); // Calling another member function
}
