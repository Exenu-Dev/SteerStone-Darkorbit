/*
* Liam Ashdown
* Copyright (C) 2019
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Precompiled.hpp>

#include "ThrTaskManager.hpp"
#include "Threading/ThrLambdaTask.hpp"

#include "BaseLogger.hpp"

#include <algorithm>

namespace SteerStone { namespace Core { namespace Threading {

    SINGLETON_P_I(TaskManager);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    TaskManager::TaskManager()
        : m_LogTasks(true)
    {
        #ifdef STEERSTONE_CORE_DEBUG
            LOG_INFO("ThrTaskManager", "Initialized");
        #endif

        /// Default interval is 2 minutes
        m_OptimizeTask = std::make_shared<OptimizeTask>(10 * 1000);

        /// By default we are using all CPU cores but not the first one
        SetWorkerCount(std::thread::hardware_concurrency() - 1);
        PushTask(m_OptimizeTask);
    }
    /// Destructor
    TaskManager::~TaskManager()
    {
        LOG_INFO("ThrTaskManager", "Destroyed");
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Push task
    /// @p_Task : Task to push
    void TaskManager::PushTask(const Task::Ptr& p_Task)
    {
        if (m_LogTasks)
            LOG_INFO("ThrTaskManager", "Task %0 started", p_Task->GetTaskName());

        std::lock_guard<std::recursive_mutex> l_Lock(m_Mutex);

        if (p_Task->GetTaskType() == TaskType::Critical)
        {
            TaskWorker* l_CriticalWorker = new TaskWorker();

            /// Skip CPU 0 at first iteration, to save some CPU for the kernel
            l_CriticalWorker->SetCPUAffinty((m_CriticalTaskWorkers.size() + 1) % std::thread::hardware_concurrency());
            l_CriticalWorker->SetName(Utils::StringBuilder("CriticalTaskWorker_%0", m_CriticalTaskWorkers.size()));
            l_CriticalWorker->PushTask(p_Task);

            m_CriticalTaskWorkers.push_back(l_CriticalWorker);
        }
        else
        {
            if (!m_TaskWorkers.empty())
            {
                TaskWorker* l_CurrentWorker = m_TaskWorkers.at(0);
                uint64          l_MinAverageUpdateTime = l_CurrentWorker->GetAverageUpdateTime();

                for (std::size_t l_I = 1; l_I < m_TaskWorkers.size(); ++l_I)
                {
                    if (m_TaskWorkers[l_I]->GetAverageUpdateTime() < l_MinAverageUpdateTime)
                        l_CurrentWorker = m_TaskWorkers[l_I];
                }

                l_CurrentWorker->PushTask(p_Task);
            }

            m_Tasks.push_back(p_Task);

            if (m_TaskWorkers.empty())
                LOG_ERROR("ThrTaskManager", "Task %0 added with no worker", p_Task->GetTaskName());
        }
    }
    /// Push a lambda task
    /// @p_Name     : Task name
    /// @p_Period   : Task interval
    /// @p_Function : Task
    Task::Ptr TaskManager::PushTask(const std::string& p_Name, uint64 p_Period, const std::function<bool()>& p_Function)
    {
        Task::Ptr l_Task = std::make_shared<LambdaTask>(p_Name, p_Period, p_Function);

        PushTask(l_Task);

        return l_Task;
    }
    /// Push a lambda task
    /// @p_Period   : Task interval
    /// @p_Function : Task
    Task::Ptr TaskManager::PushTask(uint64 p_Period, const std::function<bool()>& p_Function)
    {
        std::string l_TaskName = Utils::StringBuilder("ANONYMOUS_LAMBDA_%0", clock());
        return PushTask(l_TaskName, p_Period, p_Function);
    }
    /// Push a lambda task
    /// @p_Name         : Task name
    /// @p_Function     : Task
    /// @p_Indefinitely : Keep the task alive till exit
    Task::Ptr TaskManager::PushRunOnceTask(const std::string& p_Name, const std::function<void()>& p_Function)
    {
        Task::Ptr l_Task = std::make_shared<LambdaTask>(p_Name, 0, [p_Function]() -> bool {
            p_Function();
            return false;
            });

        PushTask(l_Task);

        return l_Task;
    }
    /// Push a lambda task
    /// @p_Function : Task
    Task::Ptr TaskManager::PushRunOnceTask(const std::function<void()>& p_Function)
    {
        std::string l_TaskName = Utils::StringBuilder("ANONYMOUS_RUN_ONCE_LAMBDA_%0", clock());
        return PushRunOnceTask(l_TaskName, p_Function);
    }
    /// Pop task
    /// @p_Task : Task to pop
    void TaskManager::PopTask(const Task::Ptr& p_Task)
    {
        if (m_LogTasks)
            LOG_WARNING("ThrTaskManager", "Task %0 ended", p_Task->GetTaskName());

        std::lock_guard<std::recursive_mutex> l_Lock(m_Mutex);

        if (m_TaskWorkers.empty())
            return;

        if (p_Task->GetTaskType() == TaskType::Critical)
        {
            auto l_It = std::find_if(m_CriticalTaskWorkers.begin(), m_CriticalTaskWorkers.end(), [&p_Task](TaskWorker* p_Worker) -> bool {
                return p_Worker->HaveTask(p_Task);
                });

            if (l_It == m_CriticalTaskWorkers.end())
                return;

            TaskWorker* l_Worker = *l_It;
            l_Worker->Suspend();
            l_Worker->PopAll();

            delete l_Worker;

            m_CriticalTaskWorkers.erase(l_It);
        }
        else
        {
            auto l_It = std::find(m_Tasks.begin(), m_Tasks.end(), p_Task);

            if (l_It == m_Tasks.end())
                return;

            for (auto l_Worker : m_TaskWorkers)
                l_Worker->PopTask(p_Task);

            m_Tasks.erase(l_It);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get all tasks
    std::vector<Task::Ptr> TaskManager::GetTasks()
    {
        std::lock_guard<std::recursive_mutex> l_Lock(m_Mutex);
        return m_Tasks;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Set worker count
    /// @p_Count : Worker count
    void TaskManager::SetWorkerCount(uint32 p_Count)
    {
        std::lock_guard<std::recursive_mutex> l_Lock(m_Mutex);

        if (p_Count == m_TaskWorkers.size())
            return;

        if (p_Count >= (std::thread::hardware_concurrency() * 2))
            LOG_WARNING("ThrTaskManager", "Set %0 workers : To many worker for actual CPU unit", p_Count);

        if (p_Count < m_TaskWorkers.size())
        {
            auto l_WorkersCopy = m_TaskWorkers;
            m_TaskWorkers.resize(p_Count);

            std::size_t l_HardwareConcurency = std::thread::hardware_concurrency();

            for (size_t l_I = p_Count; l_I < l_WorkersCopy.size(); ++l_I)
            {
                TaskWorker* l_Worker = l_WorkersCopy[l_I];
                l_Worker->Suspend();

                delete l_Worker;
            }
        }
        else if (p_Count > m_TaskWorkers.size())
        {
            std::size_t l_HardwareConcurency = std::thread::hardware_concurrency();

            for (size_t l_I = m_TaskWorkers.size(); l_I < p_Count; ++l_I)
            {
                TaskWorker* l_Worker = new TaskWorker();

                /// Skip CPU 0 at first iteration, to save some CPU for the kernel
                l_Worker->SetCPUAffinty(static_cast<int32>((l_I + 1) % l_HardwareConcurency));
                l_Worker->SetName(Utils::StringBuilder("TaskWorker_%0", l_I));

                m_TaskWorkers.push_back(l_Worker);
            }
        }

        LOG_INFO("ThrTaskManager", "Set to %0 workers", p_Count);
    }
    /// Set optimize task period
    /// @p_Period : New period
    void TaskManager::SetOptimizePeriod(uint64 p_Period)
    {
        m_OptimizeTask->SetTaskPeriod(p_Period);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Optimize
    void TaskManager::Optimize()
    {
        m_Mutex.lock();

        if (m_TaskWorkers.empty() || m_Tasks.empty())
        {
            m_Mutex.unlock();
            return;
        }

        std::vector<TaskWorker*> l_TaskWorkers = m_TaskWorkers;
        m_Mutex.unlock();

        for (auto l_Worker : l_TaskWorkers)
            l_Worker->PreSuspend();

        for (auto l_Worker : l_TaskWorkers)
        {
            l_Worker->Suspend();
            l_Worker->PopAll();
        }

        m_Mutex.lock();

        std::sort(m_Tasks.begin(), m_Tasks.end(), [](const Task::Ptr& p_A, const Task::Ptr& p_B)
            {
                return (p_A->GetTaskAverageUpdateTime() < p_B->GetTaskAverageUpdateTime());
            });

        std::size_t l_WorkerCount = m_TaskWorkers.size();
        std::size_t l_CurrentWorker = 0;

        for (auto& l_Task : m_Tasks)
        {
            m_TaskWorkers[l_CurrentWorker % l_WorkerCount]->PushTask(l_Task);
            l_CurrentWorker++;
        }

        for (std::size_t l_I = 0; l_I < m_TaskWorkers.size(); ++l_I)
            m_TaskWorkers[l_I]->Resume();

        LOG_INFO("ThrTaskManager", "Optimized : %0 tasks on %1 workers", m_Tasks.size(), m_TaskWorkers.size());

        m_Mutex.unlock();
    }

}   ///< namespace Threading
}   ///< namespace Core
}   ///< namespace SteerStone
