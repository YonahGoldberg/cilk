#ifndef SIMPLE_CILK_SCHEDULER_HPP
#define SIMPLE_CILK_SCHEDULER_HPP

#include <any>
#include <future>
#include <thread>
#include <vector>

#include "scheduler.hpp"

// A simple thread scheduler. We store the number of current threads available.
// Every time a user spawns a function to run in parallel, we check if there is
// a thread available. If there is, decrement threadsAvail, run the function in
// parallel, and increment threadsAvail after. Otherwise, simply run the
// function sequentially.
template <typename T> class SimpleScheduler : public Scheduler<T> {
private:
  int threadsAvail;
  std::mutex mut;

public:
  SimpleScheduler(){};

  void init(int n) { threadsAvail = n; }

  // Spawn a function to run "in parallel". If threadsAvail > 0, we can actually
  // run it in parallel Otherwise, run the function sequentially.
  std::future<T> spawn(std::function<T()> func) {
    std::promise<T> prom;
    std::future<T> fut = prom.get_future();
    bool runParallel = false;

    {
      std::unique_lock<std::mutex> lock(mut);
      // We have enough threads to run this function in parallel
      if (threadsAvail > 0) {
        // Decrement to ensure we don't allow too many theads to spawn.
        // Program crashes when there are too many threads.
        threadsAvail--;
        runParallel = true;
      }
    }

    // Spawn a thread to run the function in parallel
    if (runParallel) {
      std::thread t([prom = std::move(prom), func = std::move(func)]() mutable {
        // Check at compile time if we need to set the return result
        if constexpr (std::is_void<T>::value) {
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

    // We can not run this function in parallel, so run it sequentially
    if constexpr (std::is_void<T>::value) {
      func();
      prom.set_value();
    } else {
      const T result = func();
      prom.set_value(std::move(result));
    }

    return std::move(fut);
  }

  // Can't steal any more work in this implementation, just wait for future to
  // finish
  T steal(std::future<T> fut) { return fut.get(); }

  ~SimpleScheduler() {}
};

#endif