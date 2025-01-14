#pragma once

#include "Server.h"
#include <vector>
#include <utility>
#include <iostream>

class SimpleIntServer: public JC_Engine::Server<int, int> {
    public:
        SimpleIntServer(int portno, bool isDebug = false);
    protected:
        bool readClientMsg(int clientFd, std::vector<std::byte>& toPopulate, size_t offset = 0); 
        int parseClientMsg(std::vector<std::byte> msgArr);
        void encodeServerMsg(const int& msg, std::vector<std::byte>& toPopulate); 

};
