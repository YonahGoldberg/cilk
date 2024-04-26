#include "schedulers/child_scheduler.hpp"
#include "schedulers/simple_scheduler.hpp"
#include "schedulers/no_spawn_scheduler.hpp"
#include "schedulers/cont_scheduler.hpp"

// Define the global scheduler instance
extern SimpleContScheduler<int> contScheduler;
extern SimpleScheduler<int> simpleScheduler;
extern ChildScheduler<int> childScheduler;
extern NoSpawnScheduler<int> noSpawnScheduler;
extern Scheduler<int> *scheduler;