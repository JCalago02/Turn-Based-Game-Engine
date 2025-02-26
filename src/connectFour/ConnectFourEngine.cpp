#include "../../include/connectFour/ConnectFourEngine.h"

namespace JC_Engine {
    static inline constexpr size_t WIDTH = 7;
    static inline constexpr size_t HEIGHT = 6;

    ConnectFourEngine::ConnectFourEngine() {
        _board = std::vector<std::vector<Space>>(HEIGHT, std::vector<Space>(WIDTH));
    }

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
                } else {
                    std::cout << "Message dropped from " << msg.playerId << std::endl;
                    _nextMsgQueue.push({msg.playerId, -5});
                }
                break;
            default:
                break;
        }
    }

    ConnectFourMessage ConnectFourEngine::getNextMessage() {
        if (_nextMsgQueue.empty()) {
            return {-1, -1};
        }

        ConnectFourMessage msg = _nextMsgQueue.front();
        _nextMsgQueue.pop();
        return msg;
    }

    // TODO: Error handling for invalid adds?
    void ConnectFourEngine::addPlayer(int id) {
        if (_redId == -1) {
            _redId = id;
            _nextMsgQueue.push({id, 1}); // TODO: Use enums for more verbose messages?
        } else if (_yellowId == -1) {
            _yellowId = id;
            _nextMsgQueue.push({id, 1});
            _nextMsgQueue.push({_redId, -2});
            _gameState = Stage::GAME;
        }    
    }

    void ConnectFourEngine::handleMove(const ConnectFourMessage& msg, Space color, int otherPlayerId) {
        int col = msg.msg;

        bool noSpaceInCol = _board[0][col] != Space::EMPTY;
        if (noSpaceInCol) {
            // invalid item, send request back confirming this
            std::cout << "An invalid write was sent to the server " << std::endl;
            _nextMsgQueue.push({msg.playerId, -1});
            return;
        }

        size_t row = 1;
        while (row < HEIGHT && _board[row][col] == Space::EMPTY) {
            row++; 
        }
        row--;
        
        _board[row][col] = color;


        if (isWin(row, static_cast<size_t>(col))) {
            _nextMsgQueue.push({msg.playerId, -3});
            _nextMsgQueue.push({otherPlayerId, -4});
            return;
        }

        _isRed = !_isRed;
        _nextMsgQueue.push({otherPlayerId, col});
            
    }

    bool ConnectFourEngine::isWin(size_t row, size_t col) const {
        return checkHorizontal(row, col) || checkVertical(row, col) || checkDiag(row, col);
    }


    bool ConnectFourEngine::checkHorizontal(size_t row, size_t col) const {
        Space lastPlacedSpace = _board[row][col];

        size_t minCol = (col > 3) ? (col - 3) : 0;
        size_t maxCol = std::min(WIDTH, col + 4);
        int consecutiveMatches = 0;
        for (size_t checkCol = minCol; checkCol < maxCol; checkCol++) {
            bool isMatch = _board[row][checkCol] == lastPlacedSpace;
            consecutiveMatches = (consecutiveMatches + 1) * isMatch;
            if (consecutiveMatches == 4) {
                return true;
            }
        }
        return false;
    }


    bool ConnectFourEngine::checkVertical(size_t row, size_t col) const {

        Space lastPlacedSpace = _board[row][col];
        int consecutiveMatches = 1;
        size_t checkRow = row + 1;
        while (checkRow < HEIGHT) {
            bool isMatch = _board[checkRow][col] == lastPlacedSpace;
            consecutiveMatches = (consecutiveMatches + 1) * isMatch;
            if (consecutiveMatches == 4) {
                return true;
            }
            checkRow++;
        }

        return false;
    }


    bool ConnectFourEngine::checkDiag(size_t row, size_t col) const {

        Space lastPlacedSpace = _board[row][col]; 
        // check up-left -> down-right diag
        int consecutiveMatches = 0;
        size_t checkCol = col - 1;
        size_t checkRow = row - 1;
        while ((checkCol < WIDTH && checkRow < HEIGHT) && _board[checkRow][checkCol] == lastPlacedSpace) {
            consecutiveMatches++;
            checkCol--;
            checkRow--;
        }

        checkCol = col + 1;
        checkRow = row + 1;
        while ((checkCol < WIDTH && checkRow < HEIGHT) && _board[checkRow][checkCol] == lastPlacedSpace) {
            consecutiveMatches++;
            checkCol++;
            checkRow++;
        }

        if (consecutiveMatches + 1 >= 4) {
            return true;
        }


        // check down-left -> up-right diag 
        consecutiveMatches = 0;
        checkCol = col - 1;
        checkRow = row + 1;
        while ((checkCol < WIDTH && checkRow < HEIGHT) && _board[checkRow][checkCol] == lastPlacedSpace) {
            consecutiveMatches++;
            checkCol--;
            checkRow++;
        }

        checkCol = col + 1;
        checkRow = row - 1;
        while ((checkCol < WIDTH && checkRow < HEIGHT) && _board[checkRow][checkCol] == lastPlacedSpace) {
            consecutiveMatches++;
            checkCol++;
            checkRow--;
        }

        if (consecutiveMatches + 1 >= 4) {
            return true;
        }

        return false;
    }
}
