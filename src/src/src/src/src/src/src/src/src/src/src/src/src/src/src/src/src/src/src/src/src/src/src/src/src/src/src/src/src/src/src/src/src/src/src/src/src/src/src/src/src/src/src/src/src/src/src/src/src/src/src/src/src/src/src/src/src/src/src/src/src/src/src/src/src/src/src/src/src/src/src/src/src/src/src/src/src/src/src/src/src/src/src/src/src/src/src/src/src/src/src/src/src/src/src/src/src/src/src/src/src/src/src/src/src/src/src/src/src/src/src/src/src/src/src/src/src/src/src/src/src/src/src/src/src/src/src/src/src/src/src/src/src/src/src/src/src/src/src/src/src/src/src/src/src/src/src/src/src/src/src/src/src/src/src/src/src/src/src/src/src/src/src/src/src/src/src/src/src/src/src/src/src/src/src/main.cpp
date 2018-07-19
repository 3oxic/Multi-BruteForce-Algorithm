//
//  main.cpp
//  Socketio
//
//  Created by Stratos Ntallaris on 7/12/18.
//  Copyright © 2018 Stratos Ntallaris. All rights reserved.
//
#include "slave.hpp"
#include "coordinator.cpp"
#include "client.hpp"
int main(int argc, const char * argv[]) {

//
//    slave sl("127.0.0.1",3000,"test",passwords);
//    sl.startThreads();
//    // slave *test = new sl(....);
//
//

//
//    client cl("127.0.0.1", 7777);
//    cl.openListen();
//    cl.sendPasswords();
//
    coordinator cord;
    cord.startServer();
//    slave sl("127.0.0.1", 7777);
//    sl.startPasswordConn();

    int test;
    scanf("%d", &test);
    
    
//    return 0;
}
