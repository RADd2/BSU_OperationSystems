#pragma once

class Number {
private:
    double value_;
public:
    Number();
    explicit Number(double v);

    Number operator+(const Number& other) const;
    Number operator-(const Number& other) const;
    Number operator*(const Number& other) const;
    Number operator/(const Number& other) const;

    double ToDouble() const;

    
};

Number Make(double v);

extern const Number ZERO;
extern const Number ONE;