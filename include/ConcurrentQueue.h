#include <condition_variable>
#include <memory>
#include <queue>
#include <mutex>
#include <utility>

namespace JC_Engine {
    template <typename T>
    class ConcurrentQueue {
        public:
            ConcurrentQueue() = default;
            ~ConcurrentQueue() = default;
            void push (T&& t);
            bool wouldBlock();
            T pop ();
        private:
            std::queue<T> _q;
            std::condition_variable _cond;
            std::mutex _mut;
            
    };


    template <typename T>
    void ConcurrentQueue<T>::push(T&& t) {
        std::lock_guard<std::mutex> lk(_mut);
        _q.push(std::move(t));
        _cond.notify_one();
    }

    template <typename T>
    T ConcurrentQueue<T>::pop() {
        std::unique_lock<std::mutex> lk(_mut);
        _cond.wait(
                lk, [this]{return !this->_q.empty();});

        T res = std::move(_q.front());
        _q.pop();
        return res;
    }

    template <typename T>
    bool ConcurrentQueue<T>::wouldBlock() {
        std::lock_guard<std::mutex> lk(_mut);
        return _q.empty();
    }

}
