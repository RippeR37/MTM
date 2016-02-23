## MDM - Multithreaded Task Manager

Simple and portable multithreaded task manager written in pure C++11 (no additional dependencies!). 
Allows you to execute tasks asynchronously on created thread pool with support for chaining/marking other tasks as dependencies.


### Example of usage
```cpp
    // Default number of thread pool (std::thread::hardware_concurency() or 2)
    MTM::TaskManager tm; // or MTM::TaskManager tm(n);
    
    // Task without result and any dependency
    auto task1 = MTM::Task::make(/* anything callable here with void return type */);

    // Task with result of type int
    auto task2 = MTM::Task::make<int>([]() { /* ... */ return 42; });

    // Task that depends on other tasks to be executed
    // will be added to task manager after execution of it's dependencies
    auto task3 = MTM::Task::make([]() { /* ... */ }, tm, task1, task2); // depends on task1 and task2

    // Push (basic) tasks to task manager
    // tasks with dependencies will be added automatically
    tm.push(task1, task2); 

    // You can wait (and block) for specific task to be completed
    MTM::Task::getResult(task1); // waits for task1 to finish

    // You can forget about created task at this point and store only it's future
    std::future<int> task2_future = MTM::Task::getFuture(task2);
    int r2 = task2_future.get(); // waits for task2 to finish, r2 == 42

    // You can join on task manager at any time (once) to block until all tasks will be executed
    tm.join(); // wait for all (#1, #2 and #3) tasks to finish
    // After that point, tm is stopped and won't run any new tasks
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
