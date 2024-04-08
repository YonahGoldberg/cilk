#include "scheduler_instance.hpp"

// Instantiate scheduler
SimpleScheduler<int> simpleScheduler;
SimpleCSScheduler<int> simpleCSScheduler;
Scheduler<int> *scheduler = &simpleScheduler;
