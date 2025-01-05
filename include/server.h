#pragma once

#include <atomic>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <queue>
#include <sys/poll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

namespace JC_Engine {
    template <typename TClientMsg, typename TServerMsg>
    class Server {
        public:
            Server(int portno, bool isDebug = false);
            ~Server();
            void start(int maxPendingConn = 10);
            void stop();
            int acceptConnection();
            
            TClientMsg getMsg();
            void sendMsg(int clientFd, const TServerMsg& msg);

            bool isValid();
            void printErr();
            void log(std::string msg);

        protected:
            virtual void readClientMsg(int clientFd, std::vector<std::byte>& toPopulate, size_t offset = 0) = 0; 
            virtual TClientMsg parseClientMsg(std::vector<std::byte> msgArr) = 0;
            virtual void encodeServerMsg(const TServerMsg& msg, std::vector<std::byte>& toPopulate) = 0; 
        private:
            void process(); // thread method, not to be called otherwise 
            std::atomic<bool> _running;

            int _portno;
            int _sockFd = -1;
            int _nextId = 1;
            std::vector<struct pollfd> _clientFds;

            std::unordered_map<int, std::vector<std::byte>> _fdInBuffers; 
            std::unordered_map<int, std::queue<TServerMsg>> _fdOutBuffers; // TODO: Replace with concurrent queue
            std::queue<TClientMsg> _msgQueue; // TODO: Replace with concurrent queue

            bool _isDebug;
            int _errStat = 0;

    };



    // --------------------------------------------------------------------------------
    // connection handling                                                            - 
    // --------------------------------------------------------------------------------
    template <typename TClientMsg, typename TServerMsg>
    Server<TClientMsg, TServerMsg>::Server(int portno, bool isDebug): _portno(portno), _isDebug(isDebug) {
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


    }


    template <typename TClientMsg, typename TServerMsg>
    Server<TClientMsg, TServerMsg>::~Server() {
        stop();
    }


    template <typename TClientMsg, typename TServerMsg>
    void Server<TClientMsg, TServerMsg>::start(int maxPendingConn) {
        // open server for client connections
        if (listen(_sockFd, maxPendingConn) == -1) {
            _errStat = 3;
            return;
        }
        std::cout << "Server is open on port " << _portno << std::endl;

        _running.store(true);

        // TODO: Actually launch a child thread to call process()
        process();
    }


    template <typename TClientMsg, typename TServerMsg>
    void Server<TClientMsg, TServerMsg>::stop() {

        // shut down thread
        if (_running.load()) {
            _running.store(false);
        }

        // destroy connection resources
        if (_sockFd != -1) {
            close(_sockFd);
            _sockFd = -1;
            std::cout << "Server was closed on port " << _portno << std::endl;
        }
    }


    template <typename TClientMsg, typename TServerMsg>
    int Server<TClientMsg, TServerMsg>::acceptConnection() {
        int newFd = accept(_sockFd, NULL, NULL);

        struct pollfd newFdObj;
        newFdObj.fd = newFd;
        newFdObj.events = POLLIN | POLLOUT;


        _fdInBuffers[newFd] = std::vector<std::byte>();
        _fdOutBuffers[newFd] = std::queue<TServerMsg>(); // TODO: Replace with concurrent queue

        return _nextId++; 
    }


    // --------------------------------------------------------------------------------
    // messsage handling                                                              - 
    // --------------------------------------------------------------------------------
    template <typename TClientMsg, typename TServerMsg>
    TClientMsg Server<TClientMsg, TServerMsg>::getMsg() {
        return _msgQueue.front();
    }

    template <typename TClientMsg, typename TServerMsg>
    void Server<TClientMsg, TServerMsg>::sendMsg(int clientFd, const TServerMsg& msg) {
        auto it = _fdOutBuffers.find(clientFd);
        if (it != _fdOutBuffers.end()) {
            it->second.push(msg);
        }
    }

    template <typename TClientMsg, typename TServerMsg>
    void Server<TClientMsg, TServerMsg>::process() {
        while (_running.load()) {
            int ret = poll(_clientFds.data(), _clientFds.size(), -1);
            if (ret == -1) {
                break; // TODO: Should introduce some error handling here
            }

            for (size_t i = 0; i < _clientFds.size(); i++) {
                int fd = _clientFds[i].fd;

                // read messages
                if (_clientFds[i].revents & POLLIN) {
                    bool successfulRead = readClientMsg(_clientFds[i].fd, _fdInBuffers[fd], _fdInBuffers[fd].size());
                    if (successfulRead) {
                        _msgQueue.push(parseClientMsg(_fdInBuffers[fd]));
                    } else if (_fdInBuffers[fd].size() == 0) {
                        std::cerr << "Fd: " << fd << _clientFds[i].fd << " is being evicted by readClientMsg() " <<std::endl;
                        // TODO: Actually evict the client
                    }
                }

                // send messages
                const bool canWrite = _clientFds[i].revents & POLLOUT;
                while (!_fdOutBuffers[fd].empty() && canWrite) { // replace .empty() with threadsafe queue equiv (wouldBlock)
                    TServerMsg& outMsg = _fdOutBuffers[fd].front();
                    sendMsg(_clientFds[i].fd, outMsg);
                }
            }
            

        }
    }


    // --------------------------------------------------------------------------------
    // error handling                                                                 - 
    // --------------------------------------------------------------------------------
    template <typename TClientMsg, typename TServerMsg>
    bool Server<TClientMsg, TServerMsg>::isValid() {
        return _errStat == 0;
    }


    template <typename TClientMsg, typename TServerMsg>
    void Server<TClientMsg, TServerMsg>::printErr() {
        switch (_errStat) {
            case 1:
                std::cerr << "socket() failed: ";
                break;
            case 2:
                std::cerr << "bind() failed: ";
                break;
            case 3:
                std::cerr << "listen() failed: ";
                break;
            default:
                std::cout << "No error found" << std::endl;
                return;
        }
        std::cerr << strerror(errno) << std::endl;
    }
}
