#ifndef NO_SPAWN_SCHEDULER_HPP
#define NO_SPAWN_SCHEDULER_HPP

#include <future>

#include "scheduler.hpp"

template <typename T> class NoSpawnScheduler : public Scheduler<T> {
public:
  NoSpawnScheduler(){};

  T run(std::function<T()> func, int n) {
    return func();
  }

  std::future<T> spawn(std::function<T()> func) {
    std::promise<T> prom;
    std::future<T> fut = prom.get_future();
    prom.set_value(func());
    return fut;
  }

  T sync(std::future<T> fut) { 
    return fut.get();
  }
};

#endif