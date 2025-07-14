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
        std::string _key; // = password
        std::string _modes;
        size_t _membersLimit;

        void sendInitReplies(Client& client);
        void sendJoinNotificationToAll(Client& client);

    public:
        Channel();
        ~Channel();
        Channel(const Channel& other);
        Channel& operator=(const Channel& other);

        std::string getName() const;
        std::string getTopic() const;
        std::string getChannelModes() const;
        std::string getKey() const;

        void setName(const std::string& name);
        void setTopic(const std::string& topic);
        void addModes(const std::string& adding_modes);
        void setKey(const std::string& key);
        void addOperator(const int& client_fd);
        void addMember(const int& client_fd, Client* client);

        void sendChannelMessages(Client& client);
};