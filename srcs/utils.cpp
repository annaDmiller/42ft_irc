#include "Server.hpp"

bool isSpecial(char car)
{
    std::string allowed_cars = "[]\\_^{|}";

    if (allowed_cars.find_first_of(car, 0) != std::string::npos)
        return (true);
    return (false);
}

bool isForbiddenForChannelName(char car)
{
    std::string forbidden_cars = "\r\n ,:";

    if (forbidden_cars.find_first_of(car, 0) != std::string::npos)
        return (true);
    return (false);
}

std::vector<std::string> ft_split(std::string str, char delim)
{
    std::vector<std::string> words;
    std::stringstream line(str);
    std::string word;

    while (getline(line, word, delim))
        words.push_back(word);
    
    return (words);
}

std::string ft_itos(int number)
{
    std::string str_number;
    char car;
    
    if (number < 0)
    {
        str_number += std::string("-");
        number *= -1;
    }

    if (number > 10)
        str_number += ft_itos(number / 10);
    
    car = '0' + number % 10;
    str_number += car;
    return (str_number);
}