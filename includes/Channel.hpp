#pragma once

# include <string>
# include "Client.hpp"
# include <set>
# include <map>

class Client;

class Channel
{
    private:
        std::string _name;
        std::string _topic;
        std::set<int> _operators;
        std::map<int, Client*> _members;

    public:
        Channel();
        ~Channel();
        Channel(const Channel& other);
        Channel& operator=(const Channel& other);

        std::string getName() const;
        std::string getTopic() const;

        void setName(const std::string& name);
        void setTopic(const std::string& topic);
};