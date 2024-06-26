#pragma once

#include "simple_scheduler.hpp"

// Define the global scheduler instance
// extern SimpleScheduler<int> scheduler;
extern SimpleScheduler<int> simpleScheduler;
extern SimpleCSScheduler<int> simpleCSScheduler;
extern Scheduler<int> *scheduler = &simpleScheduler;
