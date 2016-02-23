#include <iostream>

#include <MTM/Task.hpp>
#include <MTM/TaskManager.hpp>


//
// Generic helper functions
//
std::mutex cout_mutex;

void generic_task(int i, int sleep) {
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "#" << i << " | Starting task no. " << i << "...\n";
    }
    std::this_thread::sleep_for(std::chrono::seconds(sleep));
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "#" << i << " | Done!\n";
    }
}

template<typename T>
T generic_task_2(int sleep, T result) {
    std::this_thread::sleep_for(std::chrono::seconds(sleep));

    return result;
}


//
// Examples
//
void example_1() {
    using namespace MTM;

    {
        TaskManager taskManager(2);

        // Create tasks to be executed
        auto task1 = Task::make<int>([]()   { return generic_task_2(1, 42);    }); // after 1s
        auto task2 = Task::make<float>([]() { return generic_task_2(3, 3.14f); }); // after 3s
        auto task3 = Task::make<float>([]() { return generic_task_2(3, 2.71f); }); // after 4s
        // decltype(taskX) == std::tuple<std::shared_ptr<Task>, std::future<T>> (task and it's future)

        // Push task on queue and execute them
        taskManager.push(task1, task2, task3);

        // Wait for results and print them
        auto task1_result = Task::getResult(task1);
        std::cout << "Task #1 completed! Result: " << task1_result << std::endl;

        auto task2_result = Task::getResult(task2);
        std::cout << "Task #2 completed! Result: " << task2_result << std::endl;

        auto task3_result = Task::getResult(task3);
        std::cout << "Task #3 completed! Result: " << task3_result << std::endl;
    }

    std::cout << "Example #1 | All tasks have been completed!" << std::endl << std::endl;
}

void example_2() {
    using namespace MTM;

    {
        TaskManager taskManager(4);

        // Create base tasks
        auto task1 = Task::make([]() { generic_task(1, 3); });
        auto task2 = Task::make([]() { generic_task(2, 2); });
        auto task3 = Task::make([]() { generic_task(3, 5); });
        
        // Create tasks that depends on task #1 and #2
        // they will be pushed to taskManager upon completion of tasks #1 and #2
        //
        // NOTE: It is guaranteed that task #4 and #5 won't start before completion of both #1 and #2
        //       so you task Y depends on X, and task Z depends on both X and Y, it's enough to say
        //       that Z depends only on Y (since X will be completed before Y will start)
        auto task4 = Task::make([]() { generic_task(4, 3); }, taskManager, task1, task2); 
        auto task5 = Task::make([]() { generic_task(5, 4); }, taskManager, task1, task2);

        // Push base tasks, all dependent tasks (#4 and #5) will be ignored and pushed
        // automatically when all of their dependencies (#1 and #2) will be completed
        taskManager.push(task1, task2, task3, task4, task5); // #4 and #5 will be ignored

        // Join after execution of last task (not necessary, destructor joins too)
        // after this call taskManager can't be used anymore (all worker threads are stopped)
        taskManager.join();
    }

    std::cout << "Example #2 | All tasks have been completed!" << std::endl << std::endl;
}

void example_3() {
    using namespace MTM;

    {
        TaskManager taskManager(2);

        // Create simple tasks
        auto task1 = Task::make([]() { generic_task(1, 3); });
        auto task2 = Task::make([]() { generic_task(2, 2); });
        auto task3 = Task::make([]() { generic_task(3, 2); }, taskManager, task1, task2);

        // Push them to taskManager
        taskManager.push(task1, task2);

        // Wait for taskManager to finish executing them
        taskManager.join();
        std::cout << "Example #3 | First part's tasks have been completed!" << std::endl << std::endl;

        // Restart taskManager object which allows you to add new tasks after joining
        taskManager.restart();

        // Create new tasks
        auto task4 = Task::make([]() { generic_task(4, 2); });
        auto task5 = Task::make([]() { generic_task(5, 4); });
        auto task6 = Task::make([]() { generic_task(6, 2); }, taskManager, task4);
        auto task7 = Task::make([]() { generic_task(7, 1); }, taskManager, task5);
        auto task8 = Task::make([]() { generic_task(8, 2); }, taskManager, task4, task5);
        auto task9 = Task::make([]() { generic_task(9, 3); }, taskManager, task4, task5, task6, task7, task8);
        // Actually, you don't have to mark #4, #5 as #9's dependencies, since #6, #7 and #8 already depends on them
        // and it is guaranteed that #6 won't start until completion of #4, #7 won't start until completion of #5
        // and #8 won't start until completion of both #4 and #5, sou you could only state your dependencies as #6, #7 and #8

        // Push only basic tasks (without dependencies)
        taskManager.push(task4, task5);
    }
    // All tasks will be joined at taskManager's destruction

    std::cout << "Example #3b | Second part's tasks have been completed!" << std::endl << std::endl;
}


//
// Main
//
int main() {
    example_1();
    example_2();
    example_3();

    return 0;
}
