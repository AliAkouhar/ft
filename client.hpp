#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
public:
    // Canonical form
    Client();
    Client(const Client& other);
    Client& operator=(const Client& other);
    ~Client();

    // Getters and setters for client information
    const std::string& getNickname() const;
    void setNickname(const std::string& nickname);

    const std::string& getUsername() const;
    void setUsername(const std::string& username);

    const std::string& getHostname() const;
    void setHostname(const std::string& hostname);

    const std::string& getRealname() const;
    void setRealname(const std::string& realname);

    int getSocket() const;
    void setSocket(int socket);

private:
    std::string _nickname;
    std::string _username;
    std::string _hostname;
    std::string _realname;
    int _socket;
};

#endif // CLIENT_HPP