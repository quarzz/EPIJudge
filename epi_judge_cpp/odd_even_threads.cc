#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <thread>

namespace {
    enum class Turn { Odd, Even };

    class AtomicMonitor {
    public:
        void wait(Turn turn) {
            while (m_turn != turn);
        }

        void next_turn(Turn turn) {
            m_turn = turn;
        }

    private:
        std::atomic<Turn> m_turn { Turn::Odd };
    };

    class ConditionMonitor {
    public:
        void wait(Turn turn) {
            std::unique_lock<std::mutex> lock { m_mutex };
            while (m_turn != turn) m_condition.wait(lock);
        }

        void next_turn(Turn turn) {
            std::lock_guard<std::mutex> guard { m_mutex };
            m_turn = turn;
            m_condition.notify_one();
        }

    private:
        Turn m_turn;
        std::mutex m_mutex;
        std::condition_variable m_condition;
    };

    template <typename Monitor>
    void print_odd(Monitor& monitor) {
        for (int i = 1; i < 100; i += 2) {
            monitor.wait(Turn::Odd);
            std::cout << i << '\n';
            monitor.next_turn(Turn::Even);
        }
    }

    template <typename Monitor>
    void print_even(Monitor& monitor) {
        for (int i = 2; i <= 100; i += 2) {
            monitor.wait(Turn::Even);
            std::cout << i << '\n';
            monitor.next_turn(Turn::Odd);
        }
    }
}

using Monitor = AtomicMonitor;

int main() {

    const auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < 10000; ++i) {
        Monitor atomic_monitor;
        std::thread odd { print_odd<Monitor>, std::ref(atomic_monitor) };
        std::thread even { print_even<Monitor>, std::ref(atomic_monitor) };
        odd.join();
        even.join();
    }

    const auto finish = std::chrono::steady_clock::now();
    const auto ellapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    std::cout << ellapsed.count() << "ms\n";
}
