#include "Server.hpp"

bool isSpecial(char car)
{
    std::string allowed_cars = "[]\\_^{|}";

    if (allowed_cars.find_first_of(car, 0) != std::string::npos)
        return (true);
    return (false);
}
