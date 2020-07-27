/*
    Purpose: Wrapps the TCP socket
*/
#ifndef _SOCKET_WRAPPER_H
#define _SOCKET_WRAPPER_H
#include<string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define N 1

class Socket_base
{
    public:
    Socket_base() = default;
    explicit Socket_base(int _domain, int type, int protocol);
    virtual int Close();
    virtual ~Socket_base();
    protected:
    int socketfd;
    int domain;
};

class Client_Socket_base: public Socket_base
{
    public:
    Client_Socket_base() = default;
    explicit Client_Socket_base(int domain, int type, int protocol);
    virtual int Read(char* buffer, size_t size) const;
    virtual int Write(const std::string& msg);
};

class Client: public Client_Socket_base
{
    public:
    explicit Client(int domain, int type, int protocol);
    bool Connect(const std::string ip, uint16_t port);
};

class AcceptedClient: public Client_Socket_base
{
    public:
    AcceptedClient() = default;
    explicit AcceptedClient(int client_socketfd, sockaddr_in _addr, socklen_t _addr_len);
    operator bool() const;
    sockaddr_in addr;
    socklen_t addr_len;
};

class Server: public Socket_base
{
    public:
    explicit Server(int domain, int type, int protocol);
    bool Bind(uint16_t port, const std::string ip = "");
    int Listen();
    AcceptedClient Accept();
};

#endif