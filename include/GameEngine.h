#include <utility>

namespace JC_Engine {
    template <typename TGameMsg>
    class GameEngine {
        public:
            GameEngine();
            
            virtual void processMessage(TGameMsg msg) = 0;
            virtual std::pair<int, TGameMsg> getNextMessage() = 0;
    };
}
