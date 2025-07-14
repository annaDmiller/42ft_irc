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

    while (getline(line, word, ','))
        words.push_back(word);
    
    return (words);
}