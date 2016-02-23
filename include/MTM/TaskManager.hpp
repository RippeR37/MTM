#ifndef __MTM_TASKMANAGER_H_INCLUDED__
#define __MTM_TASKMANAGER_H_INCLUDED__

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>


namespace MTM
{

    class Task;

    class TaskManager
    {
    public:
        TaskManager();
        TaskManager(unsigned int workers);

        ~TaskManager();

        TaskManager(const TaskManager&) = delete;

        void push(std::function<void()> task);
        void push(const Task& task);
        void push(const std::shared_ptr<Task>& task);

        template<typename T>
        void push(const std::tuple<std::shared_ptr<Task>, std::future<T>>& task) {
            push(std::get<0>(task));
        }

        template<typename T, typename... Ts>
        void push(const T& task, const Ts&... rest) {
            push(task);
            push(rest...);
        }

        int activeTasks() const;
        int remainingTasks();

        void clearTasks();
        void join();
        void restart();

    protected:
        void _restart(unsigned int workers);
        void _spawnWorkers(unsigned int workers);
        void _joinWorkers();
        void _workerProcedure(int workerId);

        unsigned int _getThreadCount();

    protected:
        std::atomic<bool> _end;
        std::atomic<bool> _joined;
        std::atomic<int> _activeTasks;

        std::mutex _mutex;
        std::condition_variable _cVar;

        std::vector<std::thread> _workers;
        std::queue<std::function<void()>> _tasks;
    };

}

#endif
