#include "ConnectFourEngine.h"
#include "../Client.h"

namespace JC_Engine {
    /*
     * Handler for all possible inputs from the UI.
     * Maintains local game state while also decoding 
     * and handling requests to/from server
     */

    class ConnectFourClient {
        public:
            ConnectFourClient();
            std::vector<std::vector<Space>> getBoard();

        private:
            void placeCol(size_t col);
            void joinLobby(const std::string& host, int portno);


            Client<ConnectFourMessage, ConnectFourMessage> _client;
            std::vector<std::vector<Space>> _board;

    };
}
