#include "../include/test.h"


SimpleIntServer::SimpleIntServer(int portno, bool isDebug) : JC_Engine::Server<int, int>(portno) {}

// TODO: Implement fd read parsing
bool SimpleIntServer::readClientMsg(int clientFd, std::vector<std::byte>& toPopulate, size_t offset) {
    std::cout << "Attempting to read from " << clientFd << " given offset " << offset << std::endl; 
    return true;
}

int SimpleIntServer::parseClientMsg(std::vector<std::byte> msgArr) {
    std::cout << "Attempting to parse a client msg, just returning -1" << std::endl;
    return -1;
}


void SimpleIntServer::encodeServerMsg(const int& msg, std::vector<std::byte>& toPopulate) {
    std::byte newByte{msg % 2 == 0};
    toPopulate.push_back(newByte);
    return;
}
