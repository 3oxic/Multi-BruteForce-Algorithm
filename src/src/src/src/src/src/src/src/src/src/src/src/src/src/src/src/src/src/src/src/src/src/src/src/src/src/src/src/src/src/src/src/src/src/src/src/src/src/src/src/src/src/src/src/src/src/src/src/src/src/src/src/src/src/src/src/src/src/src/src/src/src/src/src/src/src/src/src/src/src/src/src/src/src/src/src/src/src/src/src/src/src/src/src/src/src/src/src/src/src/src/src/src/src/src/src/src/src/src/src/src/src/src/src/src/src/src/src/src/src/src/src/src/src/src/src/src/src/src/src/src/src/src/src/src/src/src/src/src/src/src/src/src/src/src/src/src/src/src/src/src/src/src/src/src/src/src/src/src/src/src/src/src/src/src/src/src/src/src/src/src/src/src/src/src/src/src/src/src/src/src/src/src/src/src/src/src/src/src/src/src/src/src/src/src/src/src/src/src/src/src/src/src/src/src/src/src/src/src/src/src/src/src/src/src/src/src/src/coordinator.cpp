#include "coordinator.hpp"
coordinator::coordinator(){
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        puts("Socket Failed");
    }
    puts("\nSocket Success!\n");
    
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 7777 );
    
    //Bind The Socket
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("\nBind Failed!\n");
    }
    puts("\nBind Success!\n");
    gotAllThePasswordsFromClient = 0;
    
}

void coordinator::onConnect(int client_sock, int currentConnectionIndex){
    struct networkPacket receivedPacket;
    while( recv(client_sock , &receivedPacket , sizeof(struct networkPacket) , MSG_WAITALL) > 0 ) {
        messageReceived(receivedPacket, client_sock);
    }
    activeConnections.erase(std::remove(activeConnections.begin(), activeConnections.end(), client_sock), activeConnections.end());
    if(client_sock == connected_client){
        connected_client = 0;
    }else{
        connected_slaves.erase(std::remove(connected_slaves.begin(), connected_slaves.end(), client_sock), connected_slaves.end());
        
    }
    std::cout << "Someone Disconnected" <<std::endl;
    if(connected_client == currentConnectionIndex){
        connected_client = 0 ;
    }
//    close(currentConnectionIndex);
    std::unique_lock<std::mutex> ccLock(connectionClosedLock);
    connectionClosed = true;
    socketToClose = currentConnectionIndex;
    cc.notify_one();


}

void coordinator::startServer(){
    listen(socket_desc , maxServerConnections);
    std::cout << "Now Listening" << std::endl;
    c = sizeof(struct sockaddr_in);
    currentConnectionIndex = 0;
    std::thread connectionCloseHandler(&coordinator::closeConnectionThread,this);
    while (currentConnectionIndex < maxServerConnections)
    {
        std::cout << currentConnectionIndex << std::endl;
        
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        
        if ( client_sock >= 0)
        {
            for(int i = 0; i< maxServerConnections ; i++){
                if(!handleConnectionThreads[i].joinable()){
                    handleConnectionThreads[i] = std::thread(&coordinator::onConnect,this, client_sock, i);
                    activeConnections.push_back(client_sock);
                    break;
                }
            }
        }
    }
    connectionCloseHandler.join();
}

void coordinator::closeConnectionThread(){
    while(1){
        std::unique_lock<std::mutex> ccLock(connectionClosedLock);
        while (!connectionClosed){
            cc.wait(ccLock);
        }
        connectionClosed = false;
        handleConnectionThreads[socketToClose].join();
    }

}

void coordinator::messageReceived(struct networkPacket incomingPacket, int client_sock){
    struct networkPacket sendPacket;
    struct networkPacket newPacket;
    char restInfo[2048];
    
    switch(incomingPacket.packetType){
        case CLIENT_CONNECTED:
            if( connected_client == 0 ){
                std::cout << "Client is connected" << std::endl;
                connected_client = client_sock;
            }else{
                std::cout << "Can't accept more clients " << std::endl;
                newPacket.packetType = CLIENT_DENIED;
                socketLock.lock();
                send(client_sock, &newPacket, sizeof(struct networkPacket), 0);
                std::cout << "Packet is sent " << std::endl;
                socketLock.unlock();
            }
            break;
        case SLAVE_CONNECTED:
            // create New procedure for the passwords.
            std::cout << "Slave is connected" << std::endl;
            connected_slaves.push_back(client_sock);
            break;
        case ASK_FOR_PASSWORDS:
            loadPasswordsToVector.lock();
//            std::cout << "recv ask for passwords req" << std::endl;
            if(passwords.size() == 0 && gotAllThePasswordsFromClient == 1){
                noPasswordFoundLock.lock();
                if(gotAllThePasswordsFromClient==1){
                    sendPacket.packetType = TRIED_ALL_THE_PASSWORDS;
                    std::cout << "No more passwords to try" << std::endl;
                    socketLock.lock();
                    send(connected_client, &sendPacket, sizeof(struct networkPacket), 0);
                    socketLock.unlock();
                }
                noPasswordFoundLock.unlock();
            }
            if(passwords.size() == 0  || passwords.size() < packetPasswords ){
//            std::cout << "No more passwords to send" << std::endl;
                sendPacket.packetType = NO_MORE_PASSWORDS_FROM_COORDINATOR;
            }else{
                sendPacket.packetType = COORDINATOR_SENT_PASSWORDS;
                for(int i = 0 ; i<packetPasswords; i++){
                    sendPacket.passwords[i] = passwords[0];
                    passwords.erase(passwords.begin());
                }
            }
            loadPasswordsToVector.unlock();
            socketLock.lock();
            send(client_sock, &sendPacket, sizeof(struct networkPacket), 0);
            socketLock.unlock();
//            std::cout << "Im Sending again to:" << client_sock << std::endl;
            break;
        case ASK_FOR_BRUTEFORCE_INFO:

            bruteForceUrl = "";
            port = 0;
            username = "";
            // sync it with boolean sos!=====!=====!
            this->gotAllThePasswordsFromClient = false;
            if(bruteForceUrl == "" && port == 0 && username == ""){
                sendPacket.packetType = BRUTEFORCE_INFO_NOT_AVAILABLE;
                socketLock.lock();
                send(client_sock, &sendPacket, sizeof(struct networkPacket), 0);
                socketLock.unlock();
            }else{
                sendPacket.packetType = GIVE_BRUTEFORCE_INFO;
                sprintf(restInfo, "%s,%hd,%s", this->bruteForceUrl.c_str(), this->port, this->username.c_str());
                sendPacket.restInfo = restInfo;
                socketLock.lock();
                send(client_sock, &sendPacket, sizeof(struct networkPacket), 0);
                socketLock.unlock();
            }
            break;
        case PASSWORD_FOUND:
            std::cout << "Password found" << std::endl;
            loadPasswordsToVector.lock();
            passwords.clear();
            loadPasswordsToVector.unlock();
            newPacket.packetType = PASSWORD_FOUND;
            newPacket.passwords[0] = incomingPacket.passwords[0];
            socketLock.lock();
            for(int i=0; i<connected_slaves.size(); i++){
                if(connected_slaves[i]!= client_sock){
                    send(connected_slaves[i], &newPacket, sizeof(struct networkPacket), 0);
                }
            }
            send(connected_client, &newPacket, sizeof(struct networkPacket), 0);
            socketLock.unlock();
            break;
        case GET_PASSWORDS_FROM_CLIENT:
            loadPasswordsToVector.lock();
            for(int i = 0 ; i <packetPasswords ; i++){
                passwords.push_back(incomingPacket.passwords[i]);
            }
            loadPasswordsToVector.unlock();
            break;
        case GOT_PASSWORDS_FROM_CLIENT:
            std::cout << "Got Passwords From Client " << std::endl;
            noPasswordFoundLock.lock();
            gotAllThePasswordsFromClient = 1;
            noPasswordFoundLock.unlock();
            break;
        case GET_BRUTEFORCEINFO_FROM_CLIENT:
            std::cout << "Got Brute Force Info" <<std::endl;
            this->bruteForceUrl = incomingPacket.restInfo.substr(0, incomingPacket.restInfo.find(","));
            incomingPacket.restInfo = incomingPacket.restInfo.substr(bruteForceUrl.size()+1, incomingPacket.restInfo.size() - bruteForceUrl.size()+1);
            convert_into_16 = std::stoi(incomingPacket.restInfo.substr(0, incomingPacket.restInfo.find(",")));
            this->port = convert_into_16 & 0xFFFFFFFF;
            this->username = incomingPacket.restInfo.substr(incomingPacket.restInfo.find(",")+1,incomingPacket.restInfo.size());
            break;
        default:
            std::cout << incomingPacket.packetType << std::endl;
            std::cout << incomingPacket.passwords[0] << std::endl;
            std::cout << incomingPacket.passwords[19] << std::endl;
            std::cout << incomingPacket.restInfo << std::endl;
            break;
    };
}


