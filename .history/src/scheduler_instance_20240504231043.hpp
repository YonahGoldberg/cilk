#include "schedulers/child_scheduler.hpp"
#include "schedulers/simple_scheduler.hpp"
#include "schedulers/no_spawn_scheduler.hpp"
#include "schedulers/child_scheduler_lf.hpp"

// Define the global scheduler instance
extern SimpleScheduler<int> simpleScheduler;
extern ChildSchedulerLF<int> childSchedulerLF;
extern ChildScheduler<int> childScheduler;
extern NoSpawnScheduler<int> noSpawnScheduler;
extern Scheduler<int> *scheduler;