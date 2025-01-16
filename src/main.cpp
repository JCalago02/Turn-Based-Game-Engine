#include <iostream>

#include <chrono>
#include <thread>

#include "../include/SimpleServer.h"
#include "../include/SimpleClient.h"

#define PORTNO 12345
int main() {  
    
    
    std::cout << "Instantiating server " << std::endl;
    SimpleIntServer myServer(PORTNO);

    std::cout << "Starting Server " << std::endl;
    myServer.start();
    if (!myServer.isValid()) {
        myServer.printErr();
        exit(1);
    } else {
        std::cout << "No errors on server startup " << std::endl;
    }

    std::cout << "Instantiating Client " << std::endl;
    SimpleIntClient myClient("127.0.0.1", PORTNO);


    std::cout << "Connecting Server " << std::endl;
    myClient.start();

    std::cout << "Accepting connection on server " << std::endl;
    myServer.acceptConnection();

    std::cout << "Sending client msg: " << std::endl;
    myClient.sendMsg(100);

    int waitTimeSec = 2;
    std::cout << "Sleeping for " << waitTimeSec << " seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(waitTimeSec));

    std::cout << "Reading server for messages: " << std::endl;
    int msg = myServer.getMsg();
    std::cout << "Recieved " << msg << " on server " << std::endl;

    std::cout << "Sending additional client msg: " << std::endl;
    myClient.sendMsg(100);

    std::cout << "Stopping client manually" << std::endl;
    myClient.stop();

    std::cout << "Stopping server manually" << std::endl;
    myServer.stop();
    
    std::cout << "Done!" << std::endl;
    return 0;
}

