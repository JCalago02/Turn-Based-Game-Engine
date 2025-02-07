#define CATCH_CONFIG_MAIN 
#define TESTPORT 12345
#define LOOPBACKADDR "127.0.0.1"
#include "catch.hpp"
#include "../include/Server.h"
#include "../include/Client.h"

TEST_CASE("Server Recieves Single Msg From Client (Single Client)", "[server]") {

    JC_Engine::Server<int, int> myServer(TESTPORT);
    myServer.start();

    JC_Engine::Client<int, int> myClient(LOOPBACKADDR, TESTPORT);
    myClient.start();

    int clientId = myServer.acceptConnection();

    int MSG = 100;
    myClient.sendMsg(MSG);

    std::pair<int, int> clientIdAndMsg = myServer.getMsg();
    REQUIRE(clientIdAndMsg.first == clientId);
    REQUIRE(clientIdAndMsg.second == MSG);
}


TEST_CASE("Server Recieves Multiple Msgs From Client (Single Clients)", "[server]") {

    JC_Engine::Server<int, int> myServer(TESTPORT);
    myServer.start();


    JC_Engine::Client<int, int> myClient(LOOPBACKADDR, TESTPORT);
    myClient.start();

    myServer.acceptConnection();

    int NUMMSGS = 10;
    std::vector<int> sentMsgs(NUMMSGS);
    std::vector<int> recMsgs(NUMMSGS);

    for (int msg = 0; msg < NUMMSGS; msg++) {
        myClient.sendMsg(msg);
        sentMsgs.push_back(msg);
    }


    for (int msg = 0; msg < NUMMSGS; msg++) {
        recMsgs.push_back(myServer.getMsg().second);
    }

    for (int msgI = 0; msgI < NUMMSGS; msgI++) {
        REQUIRE(recMsgs[msgI] == sentMsgs[msgI]);
    }
}


TEST_CASE("Server Recieves Single Msg From Client (Multiple Clients)", "[server-mult]") {
    JC_Engine::Server<int, int> myServer(TESTPORT);
    myServer.start();


    int NUMCLIENTS = 10;
    std::vector<JC_Engine::Client<int, int>> clients;
    clients.reserve(NUMCLIENTS);

    std::vector<bool> recMsgs(NUMCLIENTS, false);
    for (int clientI = 0; clientI < NUMCLIENTS; clientI++) {
        clients.push_back(JC_Engine::Client<int, int>(LOOPBACKADDR, TESTPORT));
        clients[clientI].start();
        myServer.acceptConnection();
    }

    for (int clientI = 0; clientI < NUMCLIENTS; clientI++) {
        clients[clientI].sendMsg(clientI);
    }

    for (int clientI = 0; clientI < NUMCLIENTS; clientI++) {
        int msg = myServer.getMsg().second;
        recMsgs[msg] = true;
    }

    for (int clientI = 0; clientI < NUMCLIENTS; clientI++) {
        REQUIRE(recMsgs[clientI] == true);
    }
    

}


TEST_CASE("Client Recieves Single Msg from Server (Single Client)", "[client]") {
    JC_Engine::Server<int, int> myServer(TESTPORT);
    myServer.start();
    
    JC_Engine::Client<int, int> myClient(LOOPBACKADDR, TESTPORT);
    myClient.start();
    REQUIRE(myClient.isValid() == true); 

    int clientId = myServer.acceptConnection();


    int MSG = 10;
    myServer.sendMsg(clientId, MSG);

    REQUIRE(myClient.getMsg() == MSG);

}
