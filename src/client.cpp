#include "../include/socket_wrapper.h"
#include <iostream>
#include <string.h>
#include <future>

#define MAX_MSG_LEN 2048
#define MAX_NAME_LEN 15
inline bool bServerOnline = true;

void ReadFromServer(TCPClient* client)
{
    char read_buffer[MAX_MSG_LEN];
    int readed = 0;
    while((readed = client -> Read(read_buffer, sizeof(read_buffer))) > 0)
    {
        read_buffer[readed] = '\0';
        if(strncmp(read_buffer, "_exit", 5) == 0)
        {
            std::cout << "Leaving the chat" << std::endl;
            return;
        }
        std::cout << read_buffer;
    }
    bServerOnline = false;
    std::cout << "Server probably closed connection" << std::endl;
    std::cout << "Type anything to leave the chat" << std::endl;
}

int main(int argc, char* argv[]) {
    if(argc == 1)
    {
        std::cout << "You must enter your name before entering the chat\n"\
        << "Exapmle: " << argv[0] << " my nickname\n" << "Note: the max name length is "\
        <<  MAX_NAME_LEN << std::endl;
        return 0;
    }
    std::string client_name;
    for(int i = 1; i < argc; ++i)
    {
        client_name += argv[i];
        if(i != argc - 1)
            client_name += " ";
        if(client_name.size() > MAX_NAME_LEN)
        {
            std::cout << "The name is too long, max name length is " << MAX_NAME_LEN << std::endl;
            return 0;
        }
    }
    
    TCPClient client;
    std::cout << "Starting chat" << std::endl;
    if(!client.Connect("localhost", 27020)) {
        std::cout << "Could not connect to the server" << std::endl;
        return 0;
    }
    std::cout << "Connected to the server" << std::endl;
    client.Write(client_name + " connected to the chat\n");
    std::string write;
    auto res = std::async(std::launch::async, ReadFromServer, &client);
    while(bServerOnline) {   
        getline(std::cin, write);
        write += "\n";
        client.Write(client_name + ": " + write);
        size_t exit = write.find("_exit");
        if(exit != std::string::npos)
            break;
    }
    client.Close();
    return 0;
}