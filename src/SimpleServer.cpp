#include "../include/SimpleServer.h"


SimpleIntServer::SimpleIntServer(int portno, bool isDebug) : JC_Engine::Server<int, int>(portno) {}

// TODO: Implement fd read() error handling 
bool SimpleIntServer::readClientMsg(int clientFd, std::vector<std::byte>& toPopulate, size_t offset) {
    ssize_t bytesRead = read(clientFd, toPopulate.data() + offset, toPopulate.size() - offset);
    for (ssize_t i = 0; i < bytesRead; i++) {
        std::cout << offset + i << ", " << std::to_integer<int>(toPopulate[offset + i]) << std::endl;
    }
    if (bytesRead == -1) {
        std::cerr << "Error ocurred while reading " << std::endl;
    } else if (bytesRead == 0) {
        std::cout << "Graceful connection close for fd: " << clientFd << std::endl;
    }

    return (static_cast<size_t>(bytesRead) == (toPopulate.size() + offset));
}

int SimpleIntServer::parseClientMsg(const std::vector<std::byte>& msgArr) {
    int recievedMsg;
    std::memcpy(&recievedMsg, msgArr.data(), sizeof(int));
    return recievedMsg;
}


void SimpleIntServer::encodeServerMsg(const int& msg, std::vector<std::byte>& toPopulate) {
    std::byte newByte{msg % 2 == 0};
    toPopulate.push_back(newByte);
}
