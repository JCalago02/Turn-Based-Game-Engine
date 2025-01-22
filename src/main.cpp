#include <iostream>

#include <chrono>
#include <thread>

#include "../include/Server.h"
#include "../include/Client.h"

#define PORTNO 12345
int main() {  
    
    
    std::cout << "Instantiating server " << std::endl;
    JC_Engine::Server<int, int> myServer(PORTNO);

    std::cout << "Starting Server " << std::endl;
    myServer.start();
    if (!myServer.isValid()) {
        myServer.printErr();
        exit(1);
    } else {
        std::cout << "No errors on server startup " << std::endl;
    }

    std::cout << "Instantiating Client " << std::endl;
    JC_Engine::Client<int, int> myClient("127.0.0.1", PORTNO);


    std::cout << "Connecting Server " << std::endl;
    myClient.start();

    std::cout << "Accepting connection on server " << std::endl;
    myServer.acceptConnection();

    std::cout << "Sending client msg: " << std::endl;
    myClient.sendMsg(100);

    std::cout << "Sending additional client msg: " << std::endl;
    myClient.sendMsg(200);

    int waitTimeSec = 2;
    std::cout << "Sleeping for " << waitTimeSec << " seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(waitTimeSec));

    std::cout << "Reading server for messages: " << std::endl;
    int msg = myServer.getMsg();
    std::cout << "Recieved " << msg << " on server " << std::endl;
    
    msg = myServer.getMsg();
    std::cout << "Recieved " << msg << " on server " << std::endl;

    std::cout << "Stopping client manually" << std::endl;
    myClient.stop();

    std::cout << "Stopping server manually" << std::endl;
    myServer.stop();
    
    std::cout << "Done!" << std::endl;
    return 0;
}

