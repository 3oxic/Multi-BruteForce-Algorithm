//
//  slave.hpp
//  Socketio
//
//  Created by Stratos Ntallaris on 7/13/18.
//  Copyright © 2018 Stratos Ntallaris. All rights reserved.
//
#ifndef slave_hpp
#define slave_hpp

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

#define threadNumber 10
#define packetPasswords 20
/* Connection with coordinator definitions */
#define SLAVE_CONNECTED 2
#define COORDINATOR_SENT_PASSWORDS 3
#define ASK_FOR_PASSWORDS 4
#define NO_MORE_PASSWORDS_FROM_COORDINATOR 5
#define PASSWORD_FOUND 6
#define ASK_FOR_BRUTEFORCE_INFO 7
#define RECEIVE_BRUTEFORCE_INFO 8
#define BRUTEFORCE_INFO_NOT_AVAILABLE 9
#define GOT_PASSWORDS_FROM_CLIENT 11

/* end of connection with coordinator definitions */

class slave{
    

public:
    slave();
    slave(std::string bruteforceUrl, unsigned int short port, std::string username, std::vector<std::string> passwords);
    /* Open slave with co-ordinator */
    
    slave(std::string bruteforceUrl, unsigned int short port, std::string username, std::string coOrdinatorIp, unsigned int short coOrdinatorPort);
    slave(std::string coOrdinatorIp, unsigned int short coOrdinatorPort);
    void startThreads();
    void startPasswordConn();
private:
    
    struct networkPacket{
        int packetType;
        std::string passwords[packetPasswords];
        std::string restInfo;
    };
    
    
    std::mutex socketLock;
    std::mutex passwordsLock;
    std::mutex tryPasswordLock;
    bool correctPassword=false;
    std::string bruteforceUrl;
    unsigned short int port;
    std::string username;
    std::vector<std::string> passwords;
    std::thread tryPasswordThreads[threadNumber];
    std::string coOrdinatorIp;
    unsigned short int coOrdinatorPort;
    int coordinatorSock;
    
    void tryPassword(std::string);
    void getPasswordsFromCoordinator(std::string , unsigned short int );
    void messageReceived(struct networkPacket , int );
    
    

    

};
#endif /* slave_hpp */
