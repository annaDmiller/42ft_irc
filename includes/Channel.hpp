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
        std::set<int> _invited_members;
        std::map<int, Client*> _members;
        std::string _key; // = password
        std::string _modes;
        int _membersLimit;

        void sendInitReplies(const Client& client) const;

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
        void addMode(char new_mode);
        void removeMode(char mode_to_remove);
        void setKey(const std::string& key);
        void addOperator(const int& client_fd);
        void addMember(const int& client_fd, Client* client);

        void removeMember(const int& client_fd);

        bool canBeJoined() const;
        bool isKeyCorrect(const std::string& key) const;
        bool userIsMember(const int& client_fd) const;
        bool isEmpty() const;
        bool isOperator(int client_fd) const;

        void handleMemberLimit(const bool& isAdding, int& limit);
        void handleKey(const bool& isAdding, std::string& password, Client& client);
        void handleOperators(const bool& isAdding, int& client_fd);

        void sendJoinMessages(const Client& client) const;
        void sendMessageToAll(const std::string& message) const;
        void sendMessageToAll(const Client &client, const Server& server, 
                const std::string& target, const std::string& message,
                const int& except_fd, const std::string& cmd) const;
        void sendMessageToAll(const Client &client, const Server& server, 
                const std::string& target, const std::string& message,
                std::set<int>& except_fds, const std::string& cmd) const;

        void sendMemberList(const Client& client) const;
};