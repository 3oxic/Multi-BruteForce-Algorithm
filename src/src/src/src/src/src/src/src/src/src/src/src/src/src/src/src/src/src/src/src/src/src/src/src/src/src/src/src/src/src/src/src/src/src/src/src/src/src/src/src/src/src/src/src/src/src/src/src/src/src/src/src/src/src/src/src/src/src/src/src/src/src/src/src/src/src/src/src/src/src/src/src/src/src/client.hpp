#ifndef client_hpp
#define client_hpp
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<arpa/inet.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <unistd.h>

#define packetPasswords 20

/* Receiving packetType Definitions */
#define CLIENT_CONNECTED 1
#define SLAVE_CONNECTED 2
#define COORDINATOR_SENT_PASSWORDS 3
#define ASK_FOR_PASSWORDS 4
#define NO_MORE_PASSWORDS_FROM_COORDINATOR 5
#define PASSWORD_FOUND 6
#define ASK_FOR_BRUTEFORCE_INFO 7
#define GIVE_BRUTEFORCE_INFO 8
#define BRUTEFORCE_INFO_NOT_AVAILABLE 9
#define GET_PASSWORDS_FROM_CLIENT 10
#define GOT_PASSWORDS_FROM_CLIENT 11
#define CLIENT_DENIED 12
#define TRIED_ALL_THE_PASSWORDS 13
#define GET_BRUTEFORCEINFO_FROM_CLIENT 14
/* End of receiving packetType Definitions */


class client{
    private:
        char _tempBuffer[2048];
        std::string coOrdinatorIP;
        unsigned short int coOrdinatorPort;
        int coOrdinatorSock;
        std::vector<std::string> passwords;
        std::mutex socketLock;
        struct sockaddr_in server;
        std::string bruteForceUrl;
        unsigned int port ;
        std::string username ;
    
    struct networkPacket{
        int packetType;
        std::string passwords[packetPasswords];
        std::string restInfo;
    };
    
    void initPasswordsTest();
    void messageReceived(struct networkPacket, int);
    std::thread listenHandler;
    
    public:
    client(std::string, unsigned int short);
    void listen();
    void sendPasswords();
    void openListen();
};
#endif /* client_hpp */
