#include "GameEngine.h"
#include <utility>
#include <vector>

namespace JC_Engine {
    enum Space {
        EMPTY,
        RED,
        YELLOW
    };

    enum Stage {
        LOBBY,
        REDT,
        YELLOWT
    };

    struct ConnectFourMessage {
        int playerId;
        int msg;
    };

    class ConnectFourEngine : public GameEngine<ConnectFourMessage>{
        public:
            ConnectFourEngine();
            
            void processMessage(ConnectFourMessage msg) override;
            std::pair<int, ConnectFourMessage> getNextMessage() override;
        private:
            void addPlayer(int id);
            void handleMove(int move);

            std::vector<std::vector<Space>> _board;
            Stage _gameState;
            int _playerCount = 0;
            int _redId = -1;
            int _yellowId = -1;

    };
}
