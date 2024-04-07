#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <functional>
#include <future>

// A generic thread scheduler. All schedulers we create share a common
// interface, which makes testing easier. To create a scheduler, extend this
// class and provide definitions for the virtual functions.
template <typename T> class Scheduler {
public:
  // Initialize the scheduler with a thread pool of size n
  virtual void init(int n) = 0;

  // Spawn new function to potentially be run in parallel.
  virtual std::future<T> spawn(std::function<T()> func) = 0;

  // While future is not valid, attempt to steal work
  virtual T steal(std::future<T> fut) = 0;

  virtual ~Scheduler() {} // Virtual destructor
};

#endif