#include "schedulers/child_scheduler.hpp"
#include "schedulers/child_scheduler_lf.hpp"
#include "schedulers/no_spawn_scheduler.hpp"
#include "schedulers/simple_scheduler.hpp"

SimpleScheduler<int> simpleScheduler;
ChildSchedulerLF<int> childSchedulerLF;
ChildScheduler<int> childScheduler;
NoSpawnScheduler<int> noSpawnScheduler;
Scheduler<int> *scheduler = &noSpawnScheduler;