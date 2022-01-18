/* File: temperature_accessors.cpp */
#include "temperature.h"

// ACCESSOR member functions
char temperature::get_scale() const
{
    return scale;
}

double temperature::get_degree() const
{
    return degree;
}

void temperature::print() const
{
    cout << degree << " " << scale;
}