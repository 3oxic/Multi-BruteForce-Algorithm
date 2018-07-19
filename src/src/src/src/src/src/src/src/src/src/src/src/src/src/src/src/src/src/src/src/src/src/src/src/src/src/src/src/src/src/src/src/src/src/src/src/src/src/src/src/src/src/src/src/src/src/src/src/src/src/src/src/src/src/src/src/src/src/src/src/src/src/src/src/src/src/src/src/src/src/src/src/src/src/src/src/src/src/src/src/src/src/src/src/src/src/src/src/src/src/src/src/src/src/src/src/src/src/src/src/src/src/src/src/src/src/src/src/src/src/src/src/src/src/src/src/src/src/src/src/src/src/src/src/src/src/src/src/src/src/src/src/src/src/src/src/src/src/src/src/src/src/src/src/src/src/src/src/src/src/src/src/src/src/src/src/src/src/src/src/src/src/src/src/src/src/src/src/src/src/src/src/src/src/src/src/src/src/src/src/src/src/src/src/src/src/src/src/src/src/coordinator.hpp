#ifndef coordinator_hpp
#define coordinator_hpp


#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <stdio.h>



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

#define maxServerConnections 20

class coordinator{
    public:
        coordinator();
        void startServer();
    private:
        std::mutex socketLock;
        std::mutex loadPasswordsToVector;
        std::mutex connectionClosedLock;
        std::mutex noPasswordFoundLock;
        bool connectionClosed=false;
        int gotAllThePasswordsFromClient = 0;
        int socketToClose;
        std::condition_variable cc;
        int socket_desc , client_sock , c , read_size;
        struct sockaddr_in server , client;
        int currentConnectionIndex;
        int connected_client;
        std::vector<std::string> passwords;
        std::vector<int> connected_slaves;
        std::thread handleConnectionThreads[maxServerConnections];
        std::vector<int> activeConnections;
        std::string bruteForceUrl;
        unsigned short int port;
        std::string username;
        int convert_into_16;
        struct networkPacket{
            int packetType;
            std::string passwords[packetPasswords];
            std::string restInfo;
        };
    
        void onConnect(int , int );
        void messageReceived(struct networkPacket, int);
        void closeConnectionThread();
};
#endif /* coordinator_hpp */
