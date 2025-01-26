#include "catch.hpp"
#include "../include/ConcurrentQueue.h"
#include <thread>

TEST_CASE("ConcurrentQueue<int> - push() and pop() single elem", "[cQueue]") {
    JC_Engine::ConcurrentQueue<int> q;
    q.push(1);
    REQUIRE(q.pop() == 1);
}



TEST_CASE("ConcurrentQueue<int> - push() and pop() multiple elem", "[cQueue]") {
    JC_Engine::ConcurrentQueue<int> q;

    q.push(1);
    q.push(2);
    q.push(3);


    REQUIRE(q.pop() == 1);
    REQUIRE(q.pop() == 2);
    REQUIRE(q.pop() == 3);
}


TEST_CASE("ConcurrentQueue<int> - pop() blocks", "[cQueue]") {
    JC_Engine::ConcurrentQueue<int> q;
    bool flag = false;

    std::thread t([&flag, &q]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            flag = true;
            q.push(1);
    });

    q.pop();
    REQUIRE(flag == true);
    t.join();
}
