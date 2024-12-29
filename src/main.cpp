#include <iostream>
#include "../include/server.h"
int main() {  
    

    std::cout << "Starting server " << std::endl;
    JC_Engine::Server testServer = JC_Engine::Server(8080);
    if (!testServer.isValid()) {
        testServer.printErr();
    }

    // while(1) {
    //     int acc = accept(server_sock, nullptr, nullptr);
    //     std::cout << "Recieved an incoming connection to our server. Fd is: " << acc << std::endl;
    // }


    return 0;
}

