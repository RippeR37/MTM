## MDM - Multithreaded Task Manager

Simple and portable multithreaded task manager written in pure C++11 (no additional dependencies!). 
Allows you to execute tasks asynchronously on created thread pool with support for chaining/marking other tasks as dependencies.


### Example of usage
```cpp
    using namespace MTM;

    // Default number of thread pool (std::thread::hardware_concurency() or 2)
    TaskManager tm; // or TaskManager tm(n);
    
    // Task with void result type and without any dependency
    auto task1 = Task::make(Callable);

    // Task with result of type int and without any dependency
    auto task2 = Task::make<int>([]() { /* ... */ return 42; });

    // Task that depends on other tasks to be executed
    // Will be added to task manager after execution of it's dependencies
    // NOTE: It's guaranteed to execute after completion of all of it's dependencies
    auto task3 = Task::make<int>([]() { return 1; }, tm, task1, task2); // depends on task1 and task2

    // Push (basic) tasks to task manager
    // Tasks with dependencies will be added automatically
    // NOTE: Push task which are dependencies only after creating all dependent tasks!
    tm.push(task1, task2); 

    // You can wait (block) and retrieve for specific task to be completed
    Task::getResult(task1);                    // void return type
    int task2_result = Task::getResult(task2); // int  return type

    // You can forget about created task when you created all dependent tasks
    // and pushed this one and store only it's std::future<T> object
    std::future<int> task3_future = Task::getFuture(task3);
    // Some time later... (might block)
    int task3_result = task3_future.get(); // task3_result == 1

    // You can join on task manager at any time (once) to block until all tasks will be executed
    tm.join(); // waits for all pushed and dependent tasks to finish
    
    // After that point, tm is stopped and won't run any new tasks
    // If you want to reuse this task manager object, use restart() method
    tm.restart();
    // Now tm acts just like new TaskManager instance.
```


### Requirements
- C++11 supporting compiler (project for Visual Studio 2013 provided)


### Platforms tested
- Windows (tested on Windows 7 x64)


### Compilers tested
- MSVC (tested on Visual Studio 2013)


### Dependencies
- None :)


### License
See [LICENSE](LICENSE) file.


### Examples of usage
You can find examples of this library's usage in [example.cpp](example.cpp).
