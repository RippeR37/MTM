#ifndef __MTM_TASK_HPP_INCLUDED__
#define __MTM_TASK_HPP_INCLUDED__

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <tuple>
#include <vector>


namespace MTM 
{

    class TaskManager;


    class Task
    {
    public:
        //
        // Public type definitions
        //
        using TaskType = std::shared_ptr<Task>;

        template<typename FuncResultType = void>
        using FuncType = std::function<FuncResultType()>;

        template<typename FuncResultType = void>
        using FutureType = std::future<FuncResultType>;

        template<typename FuncResultType = void>
        using ReturnType = std::tuple<TaskType, FutureType<FuncResultType>>;

    protected:
        //
        // Protected type definitions
        //
        using AtomicCounterType = std::atomic<int>;
        using AtomicCounterPtrType = std::shared_ptr<AtomicCounterType>;
        using RefTaskType = std::reference_wrapper<TaskType>;
        using RefVectorType = std::vector<RefTaskType>;


    public:
        //
        // Public static methods
        //
        template<typename Type>
        static ReturnType<Type> make(FuncType<Type> f);
        static ReturnType<void> make(FuncType<void> f);

        template<typename Type, typename Q, typename... Qs>
        static ReturnType<Type> make(FuncType<Type> f, TaskManager& tm, ReturnType<Q>& dependency, ReturnType<Qs>&... ds);
        template<typename Q, typename... Qs>
        static ReturnType<void> make(FuncType<void> f, TaskManager& tm, ReturnType<Q>& dependency, ReturnType<Qs>&... ds);

        // Return & result access helpers
        template<typename Type>
        static TaskType&& getTask(ReturnType<Type>& task);
        static TaskType&& getTask(ReturnType<void>& task);

        template<typename Type>
        static FutureType<Type>&& getFuture(ReturnType<Type>& task);
        static FutureType<void>&& getFuture(ReturnType<void>& task);

        template<typename Type>
        static Type getResult(ReturnType<Type>& task);
        static void getResult(ReturnType<void>& task);

        
    protected:
        //
        // Protected methods
        //
        Task(FuncType<void> f);
        Task(FuncType<void> f, const RefVectorType& dependencies);

        static TaskType _make(FuncType<void> f);
        static TaskType _make(FuncType<void> f, TaskManager& tm, RefVectorType& dependencies);

        template<typename Q, typename... Qs>
        static RefVectorType _filterDependencies(ReturnType<Q>& dependency, ReturnType<Qs>&... ds);

    protected:
        //
        // Protected variables
        //
        FuncType<void> _task;
        AtomicCounterPtrType _dependencyCounter;

    friend class TaskManager;
    };

}

#endif

#include <MTM/detail/_Task.hpp>
