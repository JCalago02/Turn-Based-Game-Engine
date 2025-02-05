#include "../include/ConnectFourEngine.h"

namespace JC_Engine {
    ConnectFourEngine::ConnectFourEngine() {}

    void ConnectFourEngine::processMessage(ConnectFourMessage msg) {
        switch (_gameState) {
            case LOBBY:
                addPlayer(msg.playerId);
                break;
            case REDT:
                handleMove(msg.msg);
                break;
            case YELLOWT:
                handleMove(msg.msg);
                break;
        }
    }

    std::pair<int, int> ConnectFourEngine::getNextMessage() {
        return std::pair<int, int>();
    }

    // TODO: Error handling for invalid adds?
    void ConnectFourEngine::addPlayer(int id) {
        if (_redId == -1) {
            _redId = id;
            _playerCount++;
        } else if (_yellowId == -1) {
            _yellowId = id;
            _playerCount++;
        }
    }
}
