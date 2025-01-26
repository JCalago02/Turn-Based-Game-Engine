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
#include <utility>
#include <vector>
#include "ConcurrentQueue.h"

namespace JC_Engine {
    template <typename TClientMsg, typename TServerMsg>
    class Server {
        public:
            Server(int portno, bool isDebug = false);
            ~Server();
            void start(int maxPendingConn = 10);
            void stop();
            int acceptConnection();
            void evictConnection(int id);

            TClientMsg getMsg();
            void sendMsg(int clientFd, TServerMsg& msg);

            bool isValid();
            void printErr();
            void log(std::string msg);

        protected:
            virtual ssize_t readClientMsg(int clientFd, std::vector<std::byte>& toPopulate, size_t expectedBytes); 
            virtual TClientMsg parseClientMsg(const std::vector<std::byte>& msgArr);
            virtual void encodeServerMsg(const TServerMsg& msg, std::vector<std::byte>& toPopulate); 
        private:
            void process(); // thread method, not to be called otherwise 
            std::thread _workerThread;
            std::atomic<bool> _running;

            int _portno;
            int _sockFd = -1;
            int _nextId = 1;
            std::vector<struct pollfd> _clientFds;

            std::unordered_map<int, int> _idOfFd;
            std::unordered_map<int, int> _fdOfId; // TODO: This is ugly..., prolly should create a struct
            std::unordered_map<int, std::vector<std::byte>> _fdInBuffers; 
            std::unordered_map<int, ConcurrentQueue<TServerMsg>> _fdOutBuffers; 
            std::unordered_map<int, int> _fdInBufferSize;
            ConcurrentQueue<TClientMsg> _msgQueue; 

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

        _workerThread = std::thread(&Server::process, this);
    }


    template <typename TClientMsg, typename TServerMsg>
    void Server<TClientMsg, TServerMsg>::stop() {

        // shut down thread
        if (_running.load()) {
            _running.store(false);
            _workerThread.join();
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

        _idOfFd[newFd] = _nextId;
        _fdOfId[_nextId] = newFd;
        _fdInBuffers[_nextId] = std::vector<std::byte>(sizeof(TClientMsg));
        _fdInBufferSize[_nextId] = 0;
        _fdOutBuffers[_nextId];
        _clientFds.push_back(newFdObj);
        return _nextId++; 
    }


    template <typename TClientMsg, typename TServerMsg>
    void Server<TClientMsg, TServerMsg>::evictConnection(int id) {
        auto fdIt = _fdOfId.find(id);
        int fdToDelete = fdIt->second;
        bool invalidId = (fdIt == _fdOfId.end()); 
        if (invalidId) {
            return;
        }
        _fdOfId.erase(fdIt);


        for (size_t i = 0; i < _clientFds.size(); i++) {
            if (fdToDelete == _clientFds[i].fd) {
                _clientFds.erase(_clientFds.begin() + i);
                break;
            }
        }

        auto idIt = _idOfFd.find(fdToDelete);
        _idOfFd.erase(idIt);

        auto fdInIt = _fdInBuffers.find(id);
        _fdInBuffers.erase(fdInIt);

        auto fdInSizeIt = _fdInBufferSize.find(id);
        _fdInBufferSize.erase(fdInSizeIt);


        auto fdOutIt = _fdOutBuffers.find(id);
        _fdOutBuffers.erase(fdOutIt);

        shutdown(fdToDelete, SHUT_RDWR);
        close(fdToDelete);
    }
    // --------------------------------------------------------------------------------
    // messsage handling                                                              - 
    // --------------------------------------------------------------------------------
    template <typename TClientMsg, typename TServerMsg>
    TClientMsg Server<TClientMsg, TServerMsg>::getMsg() {
        return _msgQueue.pop();
    }

    template <typename TClientMsg, typename TServerMsg>
    void Server<TClientMsg, TServerMsg>::sendMsg(int clientFd, TServerMsg& msg) {
        auto it = _fdOutBuffers.find(clientFd);
        if (it != _fdOutBuffers.end()) {
            it->second.push(std::move(msg));
        }
    }


    template <typename TClientMsg, typename TServerMsg> 
    ssize_t Server<TClientMsg, TServerMsg>::readClientMsg(int clientFd, std::vector<std::byte>& toPopulate, size_t offset) {
        ssize_t bytesRead = read(clientFd, toPopulate.data() + offset, toPopulate.size() - offset);

        if (bytesRead == -1) {
            std::cerr << "Error ocurred while reading " << std::endl;
            return -1;
        } else if (bytesRead == 0) {
            return -1;
        }
       
        bool completedRead = (static_cast<size_t>(bytesRead) == (toPopulate.size() + offset));
        return completedRead;
    }

    template <typename TClientMsg, typename TServerMsg>
    TClientMsg Server<TClientMsg, TServerMsg>::parseClientMsg(const std::vector<std::byte>& msgArr) {
        TClientMsg recievedMsg;
        std::memcpy(&recievedMsg, msgArr.data(), sizeof(int));
        return recievedMsg;
    }

    // TODO: Still not finished
    template <typename  TClientMsg, typename TServerMsg>
    void Server<TClientMsg, TServerMsg>::encodeServerMsg(const TServerMsg& msg, std::vector<std::byte>& toPopulate) {
        std::byte newByte{msg % 2 == 0};
        toPopulate.push_back(newByte);
    }

    template <typename TClientMsg, typename TServerMsg>
    void Server<TClientMsg, TServerMsg>::process() {
        while (_running.load()) {
            // Timeout val is in MS: -1: Block indefinitely, 0: Block for 0
            int ret = poll(_clientFds.data(), _clientFds.size(), 1000); // Set to return every second
            if (ret == -1) {
                break; // TODO: Should introduce some error handling here
            }
            
            for (size_t i = 0; i < _clientFds.size(); i++) {

                int fd = _clientFds[i].fd;
                int id = _idOfFd[fd];
                // read messages
                if (_clientFds[i].revents & POLLIN) {
                    ssize_t clientStatus = readClientMsg(_clientFds[i].fd, _fdInBuffers[id], _fdInBufferSize[id]);
                    if (clientStatus == 1) {
                        const std::vector<std::byte>& clientMsgArr = _fdInBuffers[id];
                        _msgQueue.push(parseClientMsg(clientMsgArr));

                        _fdInBuffers[id].clear();
                        _fdInBuffers[id].resize(sizeof(TClientMsg));
                        _fdInBufferSize[id] = 0;
                    } else if (clientStatus == -1) { // should evict
                        evictConnection(id); 
                    }

                }
                
                // send messages
                const bool canWrite = _clientFds[i].revents & POLLOUT;
                while (!_fdOutBuffers[fd].wouldBlock() && canWrite) {
                    TServerMsg serverMsg = _fdOutBuffers[fd].pop();
                    sendMsg(_clientFds[i].fd, serverMsg);
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
