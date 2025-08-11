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
        std::string _whoSetTopic;
        std::string _whenSetTopic;
        std::set<int> _operators;
        std::set<int> _invited_members;
        std::map<int, Client*> _members;
        std::string _key; // = password
        std::string _modes;
        int _membersLimit;
        bool _isJustCreated;

        void sendInitReplies(const Client& client) const;

    public:
        Channel();
        ~Channel();
        Channel(const Channel& other);
        Channel& operator=(const Channel& other);

        std::string getName() const;
        std::string getChannelModes() const;

        void setName(const std::string& name);
        void setTopic(const std::string& topic, const std::string& nick);
        void addMode(char new_mode);
        void removeMode(char mode_to_remove);
        void addOperator(const int& client_fd);
        void addMember(const int& client_fd, Client* client);
        void addUserToInviteList(int& client_fd);
        void removeMember(const int& client_fd, Server& server);
        void checkJustCreated();

        bool canBeJoined() const;
        bool isKeyCorrect(const std::string& key) const;
        bool userIsMember(const int& client_fd) const;
        bool isEmpty() const;
        bool isOperator(int client_fd) const;
        bool isUserInvited(int client_fd) const;

        bool handleMemberLimit(const bool& isAdding, int& limit);
        bool handleKey(const bool& isAdding, std::string& password, Client& client);
        bool handleOperators(const bool& isAdding, int& client_fd, Client& client, std::string& target);

        void sendJoinMessages(const Client& client) const;
        void sendMessageToAll(const std::string& message) const;
        void sendMessageToAll(const Client &client, const Server& server, 
                const std::string& target, const std::string& message,
                const int& except_fd, const std::string& cmd) const;
        void sendMessageToAll(const Client &client, const Server& server, 
                const std::string& target, const std::string& message,
                std::set<int>& except_fds, const std::string& cmd) const;

        void sendMemberList(const Client& client) const;
        void printModes(Client& client) const;
        void printTopic(const Client& client) const;
};