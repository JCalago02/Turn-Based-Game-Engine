#include <netinet/in.h>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <netdb.h>

namespace JC_Engine {
    template <typename TClientMsg, typename TServerMsg>
    class Client {
        public:
            Client(const std::string& host, int portno, bool isDebug = false);
            ~Client();
            void start();
            void stop();

            TServerMsg getMsg();
            void sendMsg(const TClientMsg& msg);
        protected:
            virtual bool readServerMsg(int clientFd, std::vector<std::byte>& toPopulate, size_t offset = 0) = 0;
            virtual TServerMsg parseServerMsg(const std::vector<std::byte>& msgArr) = 0;
            virtual void encodeClientMsg(const TClientMsg& msg);
        private:
            addrinfo *_res;
            bool _isDebug;
            int _sockFd = -1;
            int _errStat = 0;
    };

    template <typename TClientMsg, typename TServerMsg>
    Client<TClientMsg, TServerMsg>::Client(const std::string& host, int portno, bool isDebug) {
        addrinfo hints{}, *res;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;   
        _res = res;

        if (getaddrinfo(host.c_str(), std::to_string(portno).c_str(), &hints, &res) == 0) {
            _errStat = 1;
            return;
        }

        if ((_sockFd = socket(res->ai_family, res->ai_socktype, 0)) == -1) {
            _errStat = 2;
        }
    }


    template <typename TClientMsg, typename TServerMsg>
    void Client<TClientMsg, TServerMsg>::start() {
        if (connect(_sockFd, _res->ai_addr, _res->ai_addrlen) == -1) {
            _errStat = 3;
            return;
        }
    }


    template <typename TClientMsg, typename TServerMsg>
    Client<TClientMsg, TServerMsg>::~Client() {
        freeaddrinfo(_res);
        stop(); 
    }
}
