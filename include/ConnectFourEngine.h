#include "GameEngine.h"
#include <utility>
#include <vector>
#include <iostream>

namespace JC_Engine {
    enum Space {
        EMPTY,
        RED,
        YELLOW
    };

    enum Stage {
        LOBBY,
        GAME,
        POSTGAME
    };

    // IF stage == Lobby, ignore msg
    // IF stage != Lobby, place currentMove at msg row
    struct ConnectFourMessage {
        int playerId;
        int msg;
    };

    class ConnectFourEngine : public GameEngine<ConnectFourMessage>{
        public:
            ConnectFourEngine();
            
            void processMessage(ConnectFourMessage msg) override;
            ConnectFourMessage getNextMessage() override;
        private:
            void addPlayer(int id);
            void handleMove(const ConnectFourMessage& msg, Space color, int otherPlayerId);

            std::vector<std::vector<Space>> _board;

            // _nextMsg is built on the simple assumption that each INBOUND message is met with a corresponding OUTBOUND message (1 - 1)
            ConnectFourMessage _nextMsg{1, 1}; // TODO: Replace invalid defaults
            Stage _gameState;
            bool _isRed = true;
            int _redId = -1;
            int _yellowId = -1;
    };
}
