#include "../GameEngine.h"
#include <utility>
#include <vector>
#include <queue>
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
            bool isWin(size_t row, size_t col) const;
            
            bool checkHorizontal(size_t row, size_t col) const;
            bool checkVertical(size_t row, size_t col) const;
            bool checkDiag(size_t row, size_t col) const;

            std::vector<std::vector<Space>> _board;
            std::queue<ConnectFourMessage> _nextMsgQueue;
            Stage _gameState;
            bool _isRed = true;
            int _redId = -1;
            int _yellowId = -1;
    };
}
