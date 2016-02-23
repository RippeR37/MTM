#ifndef __MTM_DETAIL__TASK_H_INCLUDED__
#define __MTM_DETAIL__TASK_H_INCLUDED__

#include <exception>
#include <future>
#include <memory>

#include <MTM/Task.hpp>


namespace MTM
{

    //
    // MTM::Task implementation
    //

    template<typename T>
    Task::ReturnType<T> Task::make(FuncType<T> f) {
        std::shared_ptr<std::promise<T>> promise = std::make_shared<std::promise<T>>();
        std::future<T> future = promise->get_future();

        FuncType<void> newTask = [promise, f]() {
            try {
                T result = f();
                promise->set_value(result);
            } catch(...) {
                promise->set_exception(std::current_exception());
            }
        };

        return std::make_tuple(_make(std::move(newTask)), std::move(future));
    }


    template<typename T, typename Q, typename... Qs>
    Task::ReturnType<T> Task::make(FuncType<T> f, TaskManager& tm, ReturnType<Q>& dependency, ReturnType<Qs>&... ds) {
        std::shared_ptr<std::promise<T>> promise = std::make_shared<std::promise<T>>();
        std::future<T> future = promise->get_future();

        FuncType<void> newTask = [promise, f]() {
            try {
                T result = f();
                promise->set_value(result);
            } catch(...) {
                promise->set_exception(std::current_exception());
            }
        };

        RefVectorType dependencies = _filterDependencies(dependency, ds...);

        return std::make_tuple(_make(std::move(newTask), tm, dependencies), std::move(future));
    }

    template<typename Q, typename... Qs>
    Task::ReturnType<void> Task::make(FuncType<void> f, TaskManager& tm, ReturnType<Q>& dependency, ReturnType<Qs>&... ds) {
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

        RefVectorType dependencies = _filterDependencies(dependency, ds...);

        return std::make_tuple(_make(std::move(newTask), tm, dependencies), std::move(future));
    }


    template<typename T>
    Task::TaskType&& Task::getTask(ReturnType<T>& task) {
        return std::move(std::get<0>(task));
    }

    template<typename T>
    Task::FutureType<T>&& Task::getFuture(ReturnType<T>& task) {
        return std::move(std::get<1>(task));
    }

    template<typename T>
    T Task::getResult(ReturnType<T>& task) {
        return std::get<1>(task).get();
    }


    template<typename Q, typename... Qs>
    Task::RefVectorType Task::_filterDependencies(ReturnType<Q>& dependency, ReturnType<Qs>&... ds) {
        return RefVectorType { std::get<0>(dependency), std::get<0>(ds)... };
    }

}

#endif
