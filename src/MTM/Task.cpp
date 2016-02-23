#include <MTM/Task.hpp>
#include <MTM/TaskManager.hpp>


namespace MTM 
{

    Task::Task(FuncType<void> f) {
        _task = std::move(f);
    }

    Task::Task(FuncType<void> f, const RefVectorType& dependencies) {
        _task = std::move(f);
        _dependencyCounter = std::make_shared<AtomicCounterType>(dependencies.size());
    }

    Task::ReturnType<void> Task::make(FuncType<void> f) {
        std::shared_ptr<std::promise<void>> promise = std::make_shared<std::promise<void>>();
        std::future<void> future = promise->get_future();

        FuncType<void> newTask = [promise, f]() {
            try {
                f();
                promise->set_value();
            } catch(...) {
                promise->set_exception(std::current_exception());
            }
        };

        return std::make_tuple(_make(std::move(newTask)), std::move(future));
    }


    Task::TaskType&& Task::getTask(ReturnType<void>& task) {
        return std::move(std::get<0>(task));
    }

    Task::FutureType<void>&& Task::getFuture(ReturnType<void>& task) {
        return std::move(std::get<1>(task));
    }

    void Task::getResult(ReturnType<void>& task) {
        std::get<1>(task).get();
    }

    Task::TaskType Task::_make(FuncType<void> f) {
        return TaskType(new Task(f));
    }

    Task::TaskType Task::_make(FuncType<void> f, TaskManager& tm, RefVectorType& dependencies) {
        TaskType thisTask = TaskType(new Task(f, dependencies));

        for(const RefTaskType& dependencyRef : dependencies) {
            TaskType& dependency = dependencyRef;

            FuncType<void> oldTask = dependency->_task;
            FuncType<void> newTask = [oldTask, thisTask, &tm]() {
                oldTask();

                if(thisTask->_dependencyCounter->fetch_sub(1) - 1 == 0)
                    tm.push(thisTask);
            };

            dependency->_task = std::move(newTask);
        }

        return thisTask;
    }

}
