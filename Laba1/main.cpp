#include <iostream>
#include "Number.h"
#include "Vector.h"

int main() {
    std::cout << "Number::Zero = " << ZERO.ToDouble()
        << ", Number::One = " << ONE.ToDouble() << "\n";

    std::cout << "(0,0) = (" << VEC_ZERO_ZERO.X().ToDouble()
        << ", " << VEC_ZERO_ZERO.Y().ToDouble() << ")\n";
    std::cout << "(1,1) = (" << VEC_ONE_ONE.X().ToDouble()
        << ", " << VEC_ONE_ONE.Y().ToDouble() << ")\n";

    Number nx = Make(3.0);
    Number ny = Make(4.0);
    Vector a(nx, ny);

    std::cout << "a.x = " << a.X().ToDouble() << std::endl;
    std::cout << "a.y = " << a.Y().ToDouble() << std::endl;
    std::cout << "a.x + a.y = " << (a.X() + a.Y()).ToDouble() << std::endl;

    Vector c = a + VEC_ONE_ONE;

    std::cout << "a = (" << a.X().ToDouble() << ", " << a.Y().ToDouble() << ")\n";
    std::cout << "c = a + (1,1) = (" << c.X().ToDouble()
        << ", " << c.Y().ToDouble() << ")\n";

    std::cout << "|a| = " << a.Radius().ToDouble() << "\n";
    std::cout << "angle(a) = " << a.Angle().ToDouble() << " rad\n";

    return 0;
}
