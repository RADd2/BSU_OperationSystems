#include "Vector.h"
#include <cmath>

Vector::Vector() : x_(ZERO), y_(ZERO) {}
Vector::Vector(const Number& x, const Number& y) : x_(x), y_(y) {}

Number Vector::X() const { return x_; }
Number Vector::Y() const { return y_; }

Vector Vector::operator+(const Vector& other) const {
    return Vector(x_ + other.X(), y_ + other.Y());
}

Number Vector::Radius() const {
    double rx = x_.ToDouble(), ry = y_.ToDouble();
    return Make(std::sqrt(rx*rx + ry*ry));
}

Number Vector::Angle() const {
    return Make(std::atan2(y_.ToDouble(), x_.ToDouble()));
}

const Vector VEC_ZERO_ZERO(Make(0.0), Make(0.0));
const Vector VEC_ONE_ONE(Make(1.0), Make(1.0));
