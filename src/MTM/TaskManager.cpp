#include <MTM/Task.hpp>
#include <MTM/TaskManager.hpp>


namespace MTM
{

    TaskManager::TaskManager() : TaskManager(_getThreadCount()) { }

    TaskManager::TaskManager(unsigned int workers) {
        _end = false;
        _joined = false;
        _workers.reserve(workers);

        for(unsigned int i = 0; i < workers; ++i)
            _workers.emplace_back(std::bind(&TaskManager::_workerProcedure, this, i));
    }

    TaskManager::~TaskManager() {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _end = true;
            _cVar.notify_all();
        }
        
        _joinWorkers();
    }


    void TaskManager::push(std::function<void()> task) {
        std::unique_lock<std::mutex> lock(_mutex);

        _tasks.emplace(std::move(task));
        _cVar.notify_one();
    }

    void TaskManager::push(const Task& task) {
        push(task._task);
    }

    void TaskManager::push(const std::shared_ptr<Task>& task) {
        push(task->_task);
    }

    int TaskManager::activeTasks() const {
        return _activeTasks.load();
    }

    int TaskManager::remainingTasks() {
        std::unique_lock<std::mutex> lock(_mutex);

        return static_cast<int>(_tasks.size());
    }

    void TaskManager::clear() {
        std::unique_lock<std::mutex> lock(_mutex);

        std::queue<std::function<void()>>().swap(_tasks);
    }

    void TaskManager::join() {
        _joined = true;
    }


    void TaskManager::_joinWorkers() {
        for(auto& thread : _workers)
            if(thread.joinable())
                thread.join();
    }

    void TaskManager::_workerProcedure(int workerId) {
        std::function<void()> currentTask;

        (void) workerId; // unused

        for(;;)
        {
            {
                std::unique_lock<std::mutex> lock(_mutex);

                if(_joined && _tasks.empty() && _activeTasks.load() == 0) {
                    _end = true;
                    _cVar.notify_all();
                    break;
                }

                while(!_end && _tasks.empty())
                    _cVar.wait(lock);

                if(_end && _tasks.empty() && _activeTasks.load() == 0) {
                    break;
                } else if(!_tasks.empty()) {
                    currentTask = std::move(_tasks.front());
                    ++_activeTasks;
                    _tasks.pop();
                } else {
                    continue;
                }
            }

            try {
                currentTask();
            } catch(...) { } // ignore exception
            
            --_activeTasks;
        }
    }

    unsigned int TaskManager::_getThreadCount() {
        unsigned int result = std::thread::hardware_concurrency();

        if(result == 0)
            result = 2; // default number of workers

        return result;
    }

}
