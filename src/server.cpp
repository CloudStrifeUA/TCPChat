#include"../include/socket_wrapper.h"
#include<iostream>
#include<string.h>
#include<mutex>
#include<vector>
#include<future>

#define MAX_MSG_LEN 2048

using ClientsList = std::vector<AcceptedClient>;

void ProcessClient(AcceptedClient client, ClientsList* clients)
{   
    char msg_buffer[MAX_MSG_LEN];
    int readed = 0;
    while((readed = client.Read(msg_buffer, sizeof(msg_buffer))) > 0)
    {
        msg_buffer[readed] = '\0';
        std::string msg = msg_buffer;
        size_t exit = msg.find("_exit");
        if(exit != std::string::npos)
        {
            client.Write(std::string(msg.begin() + exit, msg.begin() + exit + 5));
            break;
        }
        auto end = clients -> end();
        for(auto cur = clients -> begin(); cur != end; ++cur)
        {
            if(*cur == client)
                continue;
            cur -> Write(std::string(msg_buffer));
        }
    }

    auto end = clients -> end();
    for(auto cur = clients -> begin(); cur != end; ++cur)
    {
        if(*cur == client)
        {
            std::cout << "Client disconnected" << std::endl;
            client.Close();
            clients -> erase(cur);
        }
    }
}

int main()
{
    TCPServer serv;
    if(!serv.Bind(27020))
    {
        std::cout << "Could not bind socket" << std::endl;
        return 0;
    }
    if(serv.Listen() < 0)
    {
        std::cout << "Listen failed" << std::endl;
        return 0;
    }
    ClientsList vClients;
    std::vector<std::future<void>> active_threads;
    while(true)
    {
        AcceptedClient client = serv.Accept();
        if(!client)
        {
            std::cout << "Could not accept client" << std::endl;
            std:: cout << strerror(errno) << std::endl;
            continue;
        }
        vClients.push_back(client);
        active_threads.push_back(std::async(std::launch::async, ProcessClient, client, &vClients));
    }
    return 0;
}