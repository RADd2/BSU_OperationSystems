#include "Number.h"

Number::Number() : value_(0.0) {}
Number::Number(double v) : value_(v) {}

Number Number::operator+(const Number& other) const { return Number(value_ + other.ToDouble()); }
Number Number::operator-(const Number& other) const { return Number(value_ - other.ToDouble()); }
Number Number::operator*(const Number& other) const { return Number(value_ * other.ToDouble()); }
Number Number::operator/(const Number& other) const { return Number(value_ / other.ToDouble()); }

double Number::ToDouble() const { return value_; }

Number Make(double v) { return Number(v); }

const Number ZERO(0.0);
const Number ONE(1.0);
