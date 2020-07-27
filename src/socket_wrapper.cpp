#include"../include/socket_wrapper.h"
#include <unistd.h>
#include <string.h>

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

Socket_base::~Socket_base()
{
    close(socketfd);
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

Client::Client(int domain, int type, int protocol):Client_Socket_base(domain, type, protocol){}

bool Client::Connect(const std::string ip, uint16_t port)
{
    hostent* host = gethostbyname(ip.c_str());
    sockaddr_in adr;
    adr.sin_family = domain;
    adr.sin_addr.s_addr = reinterpret_cast<in_addr_t&>(host->h_addr_list[0]);
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

AcceptedClient::operator bool() const
{
    return socketfd >= 0;
}

Server::Server(int domain, int type, int protocol):Socket_base(domain, type, protocol){}

bool Server::Bind(uint16_t port, const std::string ip /* = "" */)
{
    sockaddr_in adr;
    adr.sin_family = domain;
    adr.sin_port = htons(port);
    if(ip != "")
    {
        hostent* host = gethostbyname(ip.c_str());
        adr.sin_addr.s_addr = reinterpret_cast<in_addr_t&>(host -> h_addr_list[0]);
    }
    else adr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(socketfd, (sockaddr*)&adr, sizeof(adr)) < 0)
    {
        /* ERROR */
        return false;
    }
    return true;
}

int Server::Listen()
{
    return listen(socketfd, N);
}

AcceptedClient Server::Accept()
{
    sockaddr_in addr;
    socklen_t addr_len;
    int clientfd = accept(socketfd, (sockaddr*)&addr, &addr_len);
    return AcceptedClient(clientfd, addr, addr_len);
}
