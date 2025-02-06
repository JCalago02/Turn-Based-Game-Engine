#include "catch.hpp"
#include "../include/ConnectFourEngine.h"

#define PLAYERONEID 1
#define PLAYERTWOID 2

#define ACCEPTMSG 1
#define PROMPTMSG -2
#define WINMSG -3
#define LOSEMSG -4
#define REJMSG -5

void checkMessage(JC_Engine::ConnectFourMessage& msg, int expId, int expMsg) {
    REQUIRE(msg.playerId == expId);
    REQUIRE(msg.msg == expMsg);
}

void flushMessages(JC_Engine::ConnectFourEngine& engine) {
    JC_Engine::ConnectFourMessage msg{1, 1};
    while (msg.playerId != -1) {
        msg = engine.getNextMessage();
    }
}

void checkForWinnerAndLoser(JC_Engine::ConnectFourEngine& engine, int winnerId, int loserId) {

    JC_Engine::ConnectFourMessage winMsg = engine.getNextMessage();
    checkMessage(winMsg, winnerId, WINMSG);

    JC_Engine::ConnectFourMessage loseMsg = engine.getNextMessage();
    checkMessage(loseMsg, loserId, LOSEMSG);
}

TEST_CASE("Engine can only add up to two players", "[c4Eng]") {
    JC_Engine::ConnectFourEngine engine;

    int PLAYERTHREEID = 3;

    engine.processMessage({PLAYERONEID, 1});
    JC_Engine::ConnectFourMessage playerOneAck = engine.getNextMessage();
    checkMessage(playerOneAck, PLAYERONEID, ACCEPTMSG);


    engine.processMessage({PLAYERTWOID, 1});
    JC_Engine::ConnectFourMessage playerTwoAck = engine.getNextMessage();
    checkMessage(playerTwoAck, PLAYERTWOID, ACCEPTMSG);

    JC_Engine::ConnectFourMessage playerOnePrompt= engine.getNextMessage();
    checkMessage(playerOnePrompt, PLAYERONEID, PROMPTMSG);

    engine.processMessage({PLAYERTHREEID, 1});
    JC_Engine::ConnectFourMessage playerThreeRej = engine.getNextMessage();
    checkMessage(playerThreeRej, PLAYERTHREEID, REJMSG);
}

void addTwoPlayers(JC_Engine::ConnectFourEngine& engine, int idOne, int idTwo) {
    JC_Engine::ConnectFourMessage playerOneAdd{idOne, 1};
    JC_Engine::ConnectFourMessage playerTwoAdd{idTwo, 1};

    engine.processMessage(playerOneAdd);
    engine.processMessage(playerTwoAdd);
    
    flushMessages(engine);
}

TEST_CASE("Engine sends player one's move to player two", "[c4Eng]") {
    JC_Engine::ConnectFourEngine engine;
    
    addTwoPlayers(engine, PLAYERONEID, PLAYERTWOID);


    int PLAYERONEMOVE = 1;
    engine.processMessage({PLAYERONEID, PLAYERONEMOVE});
    

    JC_Engine::ConnectFourMessage playerTwoPrompt = engine.getNextMessage();
    checkMessage(playerTwoPrompt, PLAYERTWOID, PLAYERONEMOVE);
}


TEST_CASE("Engine sends player two's move to player one", "[c4Eng]") {
    JC_Engine::ConnectFourEngine engine;
    addTwoPlayers(engine, PLAYERONEID, PLAYERTWOID);


    int PLAYERONEMOVE = 1;
    engine.processMessage({PLAYERONEID, PLAYERONEMOVE});
    engine.getNextMessage();

    int PLAYERTWOMOVE = 2;
    engine.processMessage({PLAYERTWOID, PLAYERTWOMOVE});

    JC_Engine::ConnectFourMessage playerOnePrompt = engine.getNextMessage();
    checkMessage(playerOnePrompt, PLAYERONEID, PLAYERTWOMOVE);
}

/*  
 *  Win Condition: Vertical R
 *  xxxxxxx
 *  xxxxxxx
 *  xRxxxxx
 *  xRYxxxx
 *  xRYxxxx
 *  xRYxxxx
 */
TEST_CASE("Engine detects vertical win", "[c4Eng]") {
    JC_Engine::ConnectFourEngine engine;
    addTwoPlayers(engine, PLAYERONEID, PLAYERTWOID);
    int PLAYERONECOL = 1;
    int PLAYERTWOCOL = 2;

    for (int i = 0; i < 3; i++) {
        engine.processMessage({PLAYERONEID, PLAYERONECOL});
        engine.processMessage({PLAYERTWOID, PLAYERTWOCOL});
    }

    flushMessages(engine);
    engine.processMessage({PLAYERONEID, PLAYERONECOL});
    checkForWinnerAndLoser(engine, PLAYERONEID, PLAYERTWOID);
}


/*  
 *  Win Condition: Horizontal R
 *  xxxxxxx
 *  xxxxxxx
 *  xxxxxxx
 *  xxxxxxx
 *  YYYxxxx
 *  RRRRxxx
 */
TEST_CASE("Engine detects horizontal win", "[c4Eng]") {
    JC_Engine::ConnectFourEngine engine;
    addTwoPlayers(engine, PLAYERONEID, PLAYERTWOID);
    
    for (int col = 0; col < 3; col++) {
        engine.processMessage({PLAYERONEID, col});
        engine.processMessage({PLAYERTWOID, col});
    }

    flushMessages(engine);
    engine.processMessage({PLAYERONEID, 3});
    checkForWinnerAndLoser(engine, PLAYERONEID, PLAYERTWOID);
}


/*  
 *  Win Condition: Diag R
 *  xxxxxxx
 *  xxxxxxx
 *  Rxxxxxx
 *  YRxxxxx
 *  YYRxxxx
 *  RYYRRxx
 */
TEST_CASE("Engine detects diagonal (top-left to bot-right) win", "[c4Eng]") {
    JC_Engine::ConnectFourEngine engine;
    addTwoPlayers(engine, PLAYERONEID, PLAYERTWOID);
   
    // build row 1
    engine.processMessage({PLAYERONEID, 0});
    engine.processMessage({PLAYERTWOID, 1});
    engine.processMessage({PLAYERONEID, 3});
    engine.processMessage({PLAYERTWOID, 2});
    engine.processMessage({PLAYERONEID, 4});

    // build row 2 
    engine.processMessage({PLAYERTWOID, 0});
    engine.processMessage({PLAYERONEID, 2});
    engine.processMessage({PLAYERTWOID, 1});

    // build row 3 
    engine.processMessage({PLAYERONEID, 1});
    engine.processMessage({PLAYERTWOID, 0});


    flushMessages(engine);
    engine.processMessage({PLAYERONEID, 0});
    checkForWinnerAndLoser(engine, PLAYERONEID, PLAYERTWOID);
}


/*  
 *  Win Condition: Diag R
 *  xxxxxxx
 *  xxxxxxx
 *  xxxRxxx
 *  xxRYxxx
 *  xRYYxxx
 *  RYYRRxx
 */
TEST_CASE("Engine detects diagonal (bot-left to top-right) win", "[c4Eng]") {
    JC_Engine::ConnectFourEngine engine;
    addTwoPlayers(engine, PLAYERONEID, PLAYERTWOID);


    // build row 1
    engine.processMessage({PLAYERONEID, 0});
    engine.processMessage({PLAYERTWOID, 1});
    engine.processMessage({PLAYERONEID, 3});
    engine.processMessage({PLAYERTWOID, 2});
    engine.processMessage({PLAYERONEID, 4});

    // build row 2 
    engine.processMessage({PLAYERTWOID, 2});
    engine.processMessage({PLAYERONEID, 1});
    engine.processMessage({PLAYERTWOID, 3});

    // build row 3 
    engine.processMessage({PLAYERONEID, 2});
    engine.processMessage({PLAYERTWOID, 3});

    flushMessages(engine);
    engine.processMessage({PLAYERONEID, 3});
    checkForWinnerAndLoser(engine, PLAYERONEID, PLAYERTWOID);
}
