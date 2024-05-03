#include "schedulers/child_scheduler.hpp"
#include "schedulers/simple_scheduler.hpp"
#include "schedulers/no_spawn_scheduler.hpp"
#include "schedulers/child_scheduler_lf.hpp"

SimpleScheduler<int> simpleScheduler;
ChildScheduler<int> childScheduler;
ChildSchedulerLF<int> childSchedulerLF;
NoSpawnScheduler<int> noSpawnScheduler;
Scheduler<int> *scheduler = &noSpawnScheduler;