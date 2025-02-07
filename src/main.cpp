#include "../include/Server.h"
#include "../include/Client.h"
#include "../include/ConnectFourEngine.h"

#define PORTNO 12345
int main() {
    JC_Engine::Server<JC_Engine::ConnectFourMessage, JC_Engine::ConnectFourMessage> server(PORTNO);
    JC_Engine::Client<JC_Engine::ConnectFourMessage, JC_Engine::ConnectFourMessage> playerOne("127.0.0.1", PORTNO);
    JC_Engine::Client<JC_Engine::ConnectFourMessage, JC_Engine::ConnectFourMessage> playerTwo("127.0.0.1", PORTNO);

    JC_Engine::ConnectFourEngine engine;

    server.start();
    playerOne.start();
    playerTwo.start();

    int p1Id = server.acceptConnection();
    int p2Id = server.acceptConnection();

    std::cout << "P1Id: " << p1Id << ", P2Id: " << p2Id << std::endl;
    
    JC_Engine::ConnectFourMessage initPing{1, 1};

    playerOne.sendMsg(initPing);
    playerTwo.sendMsg(initPing);

    std::pair<int, JC_Engine::ConnectFourMessage> playerOneJoin = server.getMsg();
    std::pair<int, JC_Engine::ConnectFourMessage> playerTwoJoin = server.getMsg();

    engine.processMessage({playerOneJoin.first, 1});
    engine.processMessage({playerTwoJoin.first, 1});


    JC_Engine::ConnectFourMessage p1JoinConf = engine.getNextMessage();
    JC_Engine::ConnectFourMessage p2JoinConf = engine.getNextMessage();
    JC_Engine::ConnectFourMessage p1StartPrompt = engine.getNextMessage();

    std::cout << "p1 Join ret msg: " << p1JoinConf.playerId << ", " << p1JoinConf.msg << std::endl;
    std::cout << "p2 Join ret msg: " << p2JoinConf.playerId << ", " << p2JoinConf.msg << std::endl;

    server.sendMsg(p1JoinConf.playerId, p1JoinConf);
    server.sendMsg(p2JoinConf.playerId, p2JoinConf);
    server.sendMsg(p1StartPrompt.playerId, p1StartPrompt);

    JC_Engine::ConnectFourMessage clientP1JoinConf = playerOne.getMsg();
    JC_Engine::ConnectFourMessage clientP2JoinConf = playerTwo.getMsg();
    JC_Engine::ConnectFourMessage clientP1StartPrompt = playerOne.getMsg();

    std::cout << "client p1 Join ret msg: " << clientP1JoinConf.playerId << ", " << clientP1JoinConf.msg << std::endl;
    std::cout << "client p2 Join ret msg: " << clientP2JoinConf.playerId << ", " << clientP2JoinConf.msg << std::endl;
    std::cout << "client p1 Prompt ret msg: " << clientP1StartPrompt.playerId << ", " << clientP1StartPrompt.msg << std::endl;
}

