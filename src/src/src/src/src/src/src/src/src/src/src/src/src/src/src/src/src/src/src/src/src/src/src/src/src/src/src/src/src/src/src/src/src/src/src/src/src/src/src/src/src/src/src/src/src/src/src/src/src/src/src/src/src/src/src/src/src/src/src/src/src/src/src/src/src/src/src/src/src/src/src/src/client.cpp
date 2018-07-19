//
//  client.cpp
//  Socketio
//
//  Created by Stratos Ntallaris on 7/17/18.
//  Copyright © 2018 Stratos Ntallaris. All rights reserved.
//



#include "client.hpp"
client::client(std::string coOrdinatorIP, unsigned short int coOrdinatorPort){
    this->coOrdinatorIP = coOrdinatorIP;
    this->coOrdinatorPort = coOrdinatorPort;
    this->bruteForceUrl = "127.0.0.1";
    this->port = 3000;
    this->username = "test";

    
 
}

void client::openListen(){
    listenHandler = std::thread(&client::listen,this);
    listenHandler.join();
}

void client::listen(){
    int sock;
    struct networkPacket sendPacket;
    std::cout << "Now Listening" << std::endl;
    struct networkPacket recvPacket;
    
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1){
        printf("Socket Failed!");
    }
    puts("Socket Success!");
    server.sin_addr.s_addr = inet_addr(coOrdinatorIP.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons( coOrdinatorPort );
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        puts("\nConnect Failed!\n");
    }else{
        this->coOrdinatorSock = sock;
        sendPacket.packetType = CLIENT_CONNECTED;
        send(coOrdinatorSock, &sendPacket, sizeof(struct networkPacket), 0);
        sendPacket.packetType = GET_BRUTEFORCEINFO_FROM_CLIENT;
        sprintf(_tempBuffer, "%s,%d,%s" ,bruteForceUrl.c_str(), port, username.c_str());
        sendPacket.restInfo = std::string(_tempBuffer);
        send(coOrdinatorSock, &sendPacket, sizeof(struct networkPacket), 0);
        
    }
    std::thread sendPasswordsThread(&client::sendPasswords, this);
    while( recv(coOrdinatorSock , &recvPacket , sizeof(struct networkPacket) , MSG_WAITALL) > 0 ){
        messageReceived(recvPacket, coOrdinatorSock);
    }
    sendPasswordsThread.join();
}

void client::messageReceived(struct networkPacket recvPacket, int coOrdinatorSock){
    struct networkPacket sendPacket;
    switch (recvPacket.packetType) {
        case PASSWORD_FOUND:
            std::cout << "Password found !" << std::endl;
            std::cout << recvPacket.passwords[0] << std::endl;
            passwords.clear();
            break;
        case CLIENT_DENIED:
            std::cout << "Only one client is accepted at the time" << std::endl;
            exit(0);
            break;
        case TRIED_ALL_THE_PASSWORDS:
            std::cout << "Tried all the passwords nothing found" << std::endl;
            break;
        default:
            break;
    }
}

void client::initPasswordsTest(){
    for(int i=0;i<10000;i++){
        passwords.push_back(std::to_string(i));
    }
    passwords.push_back("test");
    passwords.push_back("HelloWorld!");
}

void client::sendPasswords(){
    initPasswordsTest();
    std::cout << "Trying to send passwords" << std::endl;
    struct networkPacket sendPacket;
    sendPacket.packetType = GET_PASSWORDS_FROM_CLIENT;
    while(passwords.size()%packetPasswords!=0){
        passwords.push_back("DummyPass");
    }
    std::cout << "This is a test" << std::endl;
    while( passwords.size()!=0 ){
        for(int i = 0 ;i < packetPasswords ; i++){
            sendPacket.passwords[i] = passwords[0];
            passwords.erase(passwords.begin());
        }
        send(coOrdinatorSock, &sendPacket, sizeof(struct networkPacket), 0);
    }
    sendPacket.packetType = GOT_PASSWORDS_FROM_CLIENT;
    send(coOrdinatorSock, &sendPacket, sizeof(struct networkPacket), 0);
}
