#include "../include/Nave.hpp"
#include <iostream>

class AstroNave : public Nave {
public:
    AstroNave(float x, float y);
};

AstroNave::AstroNave(float x, float y)
    : Nave(x, y, "assets/images/AstroNave_pixil.png") {}
