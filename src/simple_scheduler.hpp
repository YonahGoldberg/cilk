#ifndef SIMPLE_CILK_SCHEDULER_HPP
#define SIMPLE_CILK_SCHEDULER_HPP

#include <vector>
#include <future>
#include <any>
#include <thread>

class SimpleScheduler {
public:
    SimpleScheduler() {};

    template <typename T>
    std::future<T> spawn(std::function<T()> func) {
        std::promise<T> prom;
        std::future<T> fut = prom.get_future();

        std::thread t([prom = std::move(prom), func = std::move(func)]() mutable {
            if constexpr(std::is_void<T>::value) {
                func();
                prom.set_value();
            } else {
                const T result = func();
                prom.set_value(std::move(result));
            }
        });

        t.detach();
        return std::move(fut);
    }
};

#endif