#include "catch.hpp"
#include "../include/ConnectFourEngine.h"

TEST_CASE("Engine can only add up to two players", "[c4Eng]") {
    JC_Engine::ConnectFourEngine engine;

    int PLAYERONEID = 1;
    int PLAYERTWOID = 2;
    int PLAYERTHREEID = 3;

    JC_Engine::ConnectFourMessage playerOneAdd{PLAYERONEID, 1};
    JC_Engine::ConnectFourMessage playerTwoAdd{PLAYERTWOID, 1};
    JC_Engine::ConnectFourMessage playerThreeAdd{PLAYERTHREEID, 1};

    int ACCEPTCONN = 1;
    int REJCONN = -1;

    engine.processMessage(playerOneAdd);
    JC_Engine::ConnectFourMessage serverMsg = engine.getNextMessage();
    REQUIRE(serverMsg.playerId == PLAYERONEID);
    REQUIRE(serverMsg.msg == ACCEPTCONN);


    engine.processMessage(playerTwoAdd);
    serverMsg = engine.getNextMessage();
    REQUIRE(serverMsg.playerId == PLAYERTWOID);
    REQUIRE(serverMsg.msg == ACCEPTCONN);


    engine.processMessage(playerThreeAdd);
    serverMsg = engine.getNextMessage();
    REQUIRE(serverMsg.playerId == PLAYERTHREEID);
    REQUIRE(serverMsg.msg == REJCONN);
}
