#include "../simple_scheduler.hpp"
#include "../simple_cs_scheduler.hpp"

SimpleScheduler<int> simpleScheduler;
SimpleCSScheduler<int> simpleCSScheduler;
Scheduler<int> *scheduler = &simpleScheduler;