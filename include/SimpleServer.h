#pragma once

#include "Server.h"
#include <cstring>
#include <vector>
#include <utility>
#include <iostream>

class SimpleIntServer: public JC_Engine::Server<int, int> {
    public:
        SimpleIntServer(int portno, bool isDebug = false);
    protected:
        ssize_t readClientMsg(int clientFd, std::vector<std::byte>& toPopulate, size_t offset); 
        int parseClientMsg(const std::vector<std::byte>& msgArr);
        void encodeServerMsg(const int& msg, std::vector<std::byte>& toPopulate); 

};
