#include"../include/socket_wrapper.h"
#include <unistd.h>
#include <string.h>
#include <iostream>

Socket_base::Socket_base(int _domain, int type, int protocol):domain(_domain)
{
    if((socketfd = socket(domain, type, protocol)) < 0)
    {
        /* ERROR */
    }
}

int Socket_base::Close()
{
    return close(socketfd);
}

bool Socket_base::operator !() const
{
    return socketfd < 0;
}

bool Socket_base::operator==(const Socket_base& other) const
{
    return socketfd == other.socketfd;
}

Client_Socket_base::Client_Socket_base(int domain, int type, int protocol):Socket_base(domain, type, protocol){}

int Client_Socket_base::Read(char* buffer, size_t size) const
{
    return read(socketfd, buffer, size);
}

int Client_Socket_base::Write(const std::string& msg)
{
    auto c_msg = msg.c_str();
    return write(socketfd, c_msg, strlen(c_msg));
}

TCPClient::TCPClient():Client_Socket_base(AF_INET, SOCK_STREAM, IPPROTO_TCP){}

bool TCPClient::Connect(const std::string ip, uint16_t port)
{
    hostent* host = gethostbyname(ip.c_str());
    sockaddr_in adr;
    adr.sin_family = domain;
    adr.sin_addr.s_addr = *((in_addr_t*)host->h_addr_list[0]);
    adr.sin_port = htons(port);
    if(connect(socketfd, (sockaddr*)&adr, sizeof(adr)) < 0)
    {
        /* ERROR */
        return false;
    }
    return true;
}

AcceptedClient::AcceptedClient(int client_socketfd, sockaddr_in _addr, socklen_t _addr_len):addr(_addr), addr_len(_addr_len)
{
    socketfd = client_socketfd;
}

TCPServer::TCPServer():Socket_base(AF_INET, SOCK_STREAM, IPPROTO_TCP){}

bool TCPServer::Bind(uint16_t port, const std::string ip /* = "" */)
{
    sockaddr_in adr;
    adr.sin_family = domain;
    adr.sin_port = htons(port);
    if(ip != "")
    {
        hostent* host = gethostbyname(ip.c_str());
        adr.sin_addr.s_addr = *((in_addr_t*)host->h_addr_list[0]);
    }
    else adr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(socketfd, (sockaddr*)&adr, sizeof(adr)) < 0)
    {
        /* ERROR */
        return false;
    }
    return true;
}

int TCPServer::Listen()
{
    return listen(socketfd, N);
}

AcceptedClient TCPServer::Accept()
{
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int clientfd = accept(socketfd, (sockaddr*)&addr, &addr_len);
    return AcceptedClient(clientfd, addr, addr_len);
}
