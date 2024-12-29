#include "../include/server.h"

namespace JC_Engine {
    Server::Server(int portno, int maxPendingConn): _portno(portno) {
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(_portno);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        if ((_sockFd = socket(serverAddress.sin_family, SOCK_STREAM, 0)) == -1) {
            _errStat = 1;
            return;
        }

        if (bind(_sockFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
            _errStat = 2;
            return;
        }


        if (listen(_sockFd, maxPendingConn) == -1) {
            _errStat = 3;
            return;
        }

        std::cout << "Server is open on port " << _portno << std::endl;
    }

    Server::~Server() {
        stop();
    }

    void Server::start() {
    }

    void Server::stop() {
        if (_sockFd != -1) {
            close(_sockFd);
            _sockFd = -1;
            std::cout << "Server was closed on port " << _portno << std::endl;
        }
    }

    bool Server::isValid() {
        return _errStat == 0;
    }


    void Server::printErr() {
        switch (_errStat) {
            case 1:
                std::cerr << "socket() failed";
                break;
            case 2:
                std::cerr << "bind() failed";
                break;
            case 3:
                std::cerr << "listen() failed";
                break;
            default:
                std::cout << "No error found" << std::endl;
                return;
        }
        std::cerr << strerror(errno) << std::endl;
    }

}




