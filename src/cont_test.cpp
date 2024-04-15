#include "simple_cont_scheduler.hpp"
SimpleContScheduler<void> scheduler;

void test() {
    std::cout << "in test\n";
    for (int i = 0; i < 10; i++) {
        scheduler.spawn([i] {
            std::cout << "Hello from iteration " << i << std::endl;
        });
    }
}

int main() {
    scheduler.run(test);
}