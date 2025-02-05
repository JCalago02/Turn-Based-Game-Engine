#include "../include/ConnectFourEngine.h"

namespace JC_Engine {
    ConnectFourEngine::ConnectFourEngine() {}

    void ConnectFourEngine::processMessage(ConnectFourMessage msg) {
        switch (_gameState) {
            case LOBBY:
                addPlayer(msg.playerId);
                break;
            case GAME:
                if (_isRed && _redId == msg.playerId) {
                    handleMove(msg, Space::RED, _yellowId);
                } else if (!_isRed && _yellowId == msg.playerId) {
                    handleMove(msg, Space::YELLOW, _redId);
                }
                break;
            default:
                break;
        }
    }

    ConnectFourMessage ConnectFourEngine::getNextMessage() {
        return _nextMsg;
    }

    // TODO: Error handling for invalid adds?
    void ConnectFourEngine::addPlayer(int id) {
        if (_redId == -1) {
            _redId = id;
            _nextMsg = {id, 1};
        } else if (_yellowId == -1) {
            _yellowId = id;
            _nextMsg = {id, 1};
        } else {
            _nextMsg = {id, -1};
        }
    }

    void ConnectFourEngine::handleMove(const ConnectFourMessage& msg, Space color, int otherPlayerId) {
        int col = msg.msg;
        std::vector<Space>& currentCol = _board[col];

        if (currentCol[0] != Space::EMPTY) {
            // invalid item, send request back confirming this
            std::cout << "An invalid write was sent to the server " << std::endl;
            _nextMsg = {msg.playerId, -1};
            return;
        }

        size_t i = 0;
        while (i < currentCol.size() && currentCol[i] == Space::EMPTY) {
            i++; 
        }
        currentCol[i-1] = color;
        _isRed = !_isRed;
        _nextMsg = {otherPlayerId, col};
        
    }
}
