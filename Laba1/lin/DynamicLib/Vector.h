#pragma once
#include "Number.h"

class Vector {
public:
    Vector();
    Vector(const Number& x, const Number& y);

    Number X() const;
    Number Y() const;

    Vector operator+(const Vector& other) const;
    Number Radius() const;
    Number Angle() const;

private:
    Number x_;
    Number y_;
};


extern const Vector VEC_ZERO_ZERO;
extern const Vector VEC_ONE_ONE;
