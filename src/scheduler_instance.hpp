#include "schedulers/simple_scheduler.hpp"
#include "schedulers/simple_cs_scheduler.hpp"

// Define the global scheduler instance
extern SimpleScheduler<int> simpleScheduler;
extern SimpleCSScheduler<int> simpleCSScheduler;
extern Scheduler<int> *scheduler;