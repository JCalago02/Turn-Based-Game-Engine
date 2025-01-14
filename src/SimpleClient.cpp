#include "../include/SimpleClient.h"

SimpleIntClient::SimpleIntClient(const std::string& host, int portno, bool isDebug) : JC_Engine::Client<int, int>(host, portno){}

bool SimpleIntClient::readServerMsg(int clientFd, std::vector<std::byte>& toPopulate, size_t offset) {
    std::cout << "Attempting to read from" << clientFd << " given offset " << offset << std::endl;
    return true;
}


int SimpleIntClient::parseServerMsg(const std::vector<std::byte>& msgArr) { 
    std::cout << "Attempting to parse a client msg, just returning -1" << std::endl;
    return -1;
}


void SimpleIntClient::encodeClientMsg(const int& msg, std::vector<std::byte>& toPopulate) {
    std::byte newByte{msg % 2 == 0};
    toPopulate.push_back(newByte);
}
