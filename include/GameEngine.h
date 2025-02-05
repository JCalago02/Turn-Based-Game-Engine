#include <utility>

namespace JC_Engine {
    template <typename TGameMsg>
    class GameEngine {
        public:
            GameEngine() = default;
            
            virtual void processMessage(TGameMsg msg) = 0;
            virtual TGameMsg getNextMessage() = 0;
    };
}
