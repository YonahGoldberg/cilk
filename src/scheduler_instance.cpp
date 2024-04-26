#include "schedulers/child_scheduler.hpp"
#include "schedulers/simple_scheduler.hpp"
#include "schedulers/no_spawn_scheduler.hpp"
#include "schedulers/cont_scheduler.hpp"

SimpleContScheduler<int> contScheduler;
SimpleScheduler<int> simpleScheduler;
ChildScheduler<int> childScheduler;
NoSpawnScheduler<int> noSpawnScheduler;
Scheduler<int> *scheduler = &noSpawnScheduler;