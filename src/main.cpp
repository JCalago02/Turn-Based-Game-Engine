#include <iostream>

#include <chrono>
#include <thread>

#include "../include/test.h"
int main() {  
    

    std::cout << "Instantiating server " << std::endl;
    SimpleIntServer myServer(12345);

    std::cout << "Starting Server " << std::endl;
    myServer.start();

    std::cout << "Sleeping for 10 seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Stopping server manually" << std::endl;
    myServer.stop();
    
    std::cout << "Done!" << std::endl;
    return 0;
}

