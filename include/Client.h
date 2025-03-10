#pragma once

#include <cerrno>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <sys/socket.h>

namespace JC_Engine {
    template <typename TClientMsg, typename TServerMsg>
    class Client {
        public:
            Client() = default;
            Client(const std::string& host, int portno, bool isDebug = false);
            Client(const Client& other);
            Client(Client&& other);
            Client& operator=(const Client& other);
            Client& operator=(Client&& other) noexcept;
            ~Client();

            void start();
            void stop();
            bool isValid();

            TServerMsg getMsg();
            void sendMsg(const TClientMsg& msg);
        protected:
            virtual ssize_t readServerMsg(std::vector<std::byte>& toPopulate, size_t offset = 0);
            virtual TServerMsg parseServerMsg(const std::vector<std::byte>& msgArr);
            virtual void encodeClientMsg(const TClientMsg& msg, std::vector<std::byte>& toPopulate);
        private:

            addrinfo* copyAddrinfo(addrinfo *res);
            bool _manualFree = false;
            bool _isDebug;
            addrinfo *_res = NULL;
            int _sockFd = -1;
            int _errStat = 0;
    };

    template <typename TClientMsg, typename TServerMsg>
    Client<TClientMsg, TServerMsg>::Client(const std::string& host, int portno, bool isDebug) {
        addrinfo hints{}, *res;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;   

        if (getaddrinfo(host.c_str(), std::to_string(portno).c_str(), &hints, &res) != 0) {
            std::cerr << "getaddrinfo() failed " << std::endl;
            _errStat = 1;
            return;
        }

        if ((_sockFd = socket(res->ai_family, res->ai_socktype, 0)) == -1) {
            std::cerr << "Socket() failed " << std::endl;
            _errStat = 2;
        }

        _res = res;
    }


    template <typename TClientMsg, typename TServerMsg>
    void Client<TClientMsg, TServerMsg>::start() {
        if (connect(_sockFd, _res->ai_addr, _res->ai_addrlen) == -1) {
            std::cerr << "Connect() failed " << std::endl;
            std::cerr << strerror(errno) << std::endl;
            _errStat = 3;
        }
    }


    template <typename TClientMsg, typename TServerMsg>
    Client<TClientMsg, TServerMsg>::~Client() {
        stop(); 
    }


    template <typename TClientMsg, typename TServerMsg>
    void Client<TClientMsg, TServerMsg>::stop() {
        if (_res) {
            if (!_manualFree) {
                freeaddrinfo(_res);
            } else {
                free(_res);
            }
            _res = NULL;
        }

        if (_sockFd != -1) {
            close(_sockFd);
            _sockFd = -1;
        }
    }


    template <typename TClientMsg, typename TServerMsg>
    Client<TClientMsg, TServerMsg>::Client(const Client<TClientMsg, TServerMsg>& other) 
        : _isDebug(other._isDebug), 
          _res(copyAddrinfo(other._res)),
          _sockFd(dup(other._sockFd)),
          _errStat(other._errStat)
    {}


    template <typename TClientMsg, typename TServerMsg>
    Client<TClientMsg, TServerMsg>::Client(Client<TClientMsg, TServerMsg>&& other)
        : _isDebug(other._isDebug), 
          _res(copyAddrinfo(other._res)),
          _sockFd(dup(other._sockFd)),
          _errStat(other._errStat)
    {
        other.stop();
    }

    template <typename TClientMsg, typename TServerMsg>
    Client<TClientMsg, TServerMsg>& Client<TClientMsg, TServerMsg>::operator=(const Client<TClientMsg, TServerMsg>& other) {
        if (this == &other) return *this;
        stop();

        _isDebug = other._isDebug;
        _res = copyAddrinfo(other._res);
        _sockFd = dup(other._sockFd);
        _errStat = other._errStat;
        return *this;
    }


    template <typename TClientMsg, typename TServerMsg>
    Client<TClientMsg, TServerMsg>& Client<TClientMsg, TServerMsg>::operator=(Client<TClientMsg, TServerMsg>&& other) noexcept {
        if (this == &other) return *this;
        stop();


        _isDebug = other._isDebug;
        _res = copyAddrinfo(other._res);
        _sockFd = dup(other._sockFd);
        _errStat = other._errStat;
        other.stop();
        return *this;
    }

    template <typename TClientMsg, typename TServerMsg>
    addrinfo* Client<TClientMsg, TServerMsg>::copyAddrinfo(addrinfo* res) {
        if (!res) return NULL;

        addrinfo* newNode = new addrinfo;
        newNode->ai_addr = res->ai_addr; 
        newNode->ai_addrlen = res->ai_addrlen;
        newNode->ai_family = res->ai_family; 
        newNode->ai_socktype = res->ai_socktype;
       
        _manualFree = true;
        return newNode;
    }

    template <typename TClientMsg, typename TServerMsg>
    TServerMsg Client<TClientMsg, TServerMsg>::getMsg() {
        std::vector<std::byte> buffer(sizeof(TServerMsg));

        ssize_t completedMsg = 0;        
        while (completedMsg == 0) {
            completedMsg = readServerMsg(buffer, 0);
        }
        
        if (completedMsg == -1) {
            return TServerMsg(); // TODO: Should probably add some error handling here
        }
        return parseServerMsg(buffer);
    }


    template <typename TClientMsg, typename TServerMsg>
    void Client<TClientMsg, TServerMsg>::sendMsg(const TClientMsg& msg) {
        std::vector<std::byte> toPopulateBuffer(sizeof(TClientMsg));
        encodeClientMsg(msg, toPopulateBuffer);

        ssize_t bytesWritten = write(_sockFd, toPopulateBuffer.data(), sizeof(TClientMsg));
        if (bytesWritten == -1) {
            std::cerr << "Write() failed" << strerror(errno) << std::endl;
            return;
        }
    }


    template <typename TClientMsg, typename TServerMsg>
    ssize_t Client<TClientMsg, TServerMsg>::readServerMsg(std::vector<std::byte>& toPopulate, size_t offset) {
        ssize_t bytesRead = read(_sockFd, toPopulate.data() + offset, toPopulate.size() - offset);
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
    TServerMsg Client<TClientMsg, TServerMsg>::parseServerMsg(const std::vector<std::byte>& msgArr) {
        TServerMsg recievedMsg;
        std::memcpy(&recievedMsg, msgArr.data(), sizeof(TServerMsg));
        return recievedMsg;
    }


    template <typename TClientMsg, typename TServerMsg>
    void Client<TClientMsg, TServerMsg>::encodeClientMsg(const TClientMsg& msg, std::vector<std::byte>& toPopulate) {
        std::memcpy(toPopulate.data(), &msg, sizeof(int));
    }


    template <typename TClientMsg, typename TServerMsg>
    bool Client<TClientMsg, TServerMsg>::isValid() {
        return _errStat == 0;
    }
}
