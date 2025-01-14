#pragma once

#include "Client.h"
#include <iostream>

class SimpleIntClient : public JC_Engine::Client<int, int> {
    public:
        SimpleIntClient(const std::string& host, int portno, bool isDebug = false);
    protected:
            bool readServerMsg(int clientFd, std::vector<std::byte>& toPopulate, size_t offset = 0);
            int parseServerMsg(const std::vector<std::byte>& msgArr);
            void encodeClientMsg(const int& msg, std::vector<std::byte>& toPopulate);
};
