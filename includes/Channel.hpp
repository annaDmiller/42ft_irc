#pragma once

# include <string>
# include "Client.hpp"
# include "Server.hpp"
# include <set>
# include <map>

class Client;
class Server;

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

        void sendInitReplies(const Client& client) const;
        bool isOperator(const int& client_fd) const;

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

        void removeMember(const int& client_fd);

        bool canBeJoined() const;
        bool isKeyCorrect(const std::string& key) const;
        bool userIsMember(const int& client_fd) const;

        void sendJoinMessages(const Client& client) const;
        void sendMessageToAll(const std::string& message) const;
        void sendMessageToAll(const Client &client, const Server& server, 
                const std::string& target, const std::string& message,
                const int& except_fd) const;
};