#pragma once

#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include "unistd.h"

namespace JC_Engine {
    class Server {
        public:
            Server(int portno, int maxPendingConn = 10);
            ~Server();
            void start();
            void stop();
            bool isValid();
            void printErr();
        private:
            int _portno;
            int _sockFd = -1;
            int _errStat = 0;

    };
}
