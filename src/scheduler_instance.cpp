#include "schedulers/simple_cs_scheduler.hpp"
#include "schedulers/simple_scheduler.hpp"

SimpleScheduler<int> simpleScheduler;
SimpleCSScheduler<int> simpleCSScheduler;
Scheduler<int> *scheduler = &simpleScheduler;