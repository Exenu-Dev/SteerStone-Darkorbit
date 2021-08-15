/*
* Liam Ashdown
* Copyright (C) 2021
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

#include "ZoneUpdater.hpp"
#include "Threading/ThrTaskManager.hpp"
#include "Utility/UtiString.hpp"
#include "Zone.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Constructor
    /// @p_ZoneUpdater : Zone Updater
    ZoneUpdaterTask::ZoneUpdaterTask(ZoneUpdater* p_ZoneUpdater)
        : m_ZoneUpdater(p_ZoneUpdater)
    { 
    }
    /// Deconstructor
    ZoneUpdaterTask::~ZoneUpdaterTask()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Notify that the task is done
    void ZoneUpdaterTask::UpdateFinished()
    {
        if (m_ZoneUpdater)
            m_ZoneUpdater->Finished();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    /// @p_Zone : Zone
    /// @p_Diff : Execution Time
    ZoneUpdateRequest::ZoneUpdateRequest(Zone* p_Zone, ZoneUpdater& p_ZoneUpdater, uint32 const p_Diff)
        : ZoneUpdaterTask(&p_ZoneUpdater), m_Zone(p_Zone), m_Diff(p_Diff)
    {
    }
    /// Deconstructor
    ZoneUpdateRequest::~ZoneUpdateRequest()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Call Zone Update
    void ZoneUpdateRequest::Call()
    {
        /// Update all of our maps
        for (auto l_Itr : m_Zone->m_Maps)
            l_Itr.second->Update(m_Diff);

        UpdateFinished();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    ZoneUpdater::ZoneUpdater()
    {
        m_CancelationToken  = false;
        m_PendingRequests   = 0;
        m_Activated         = false;
    }
    /// Deconstructor
    ZoneUpdater::~ZoneUpdater()
    {
        m_CancelationToken = true;

        /// Finish all requests before we clear
        Wait();

        for (auto l_Itr : m_Tasks)
            sThreadManager->PopTask(l_Itr);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Activate Threads
    /// @p_WorkerCount : Number of threads to spawn
    void ZoneUpdater::Activate(uint32 p_WorkerCount)
    {
        for (uint32 l_I = 0; l_I < p_WorkerCount; l_I++)
        {
            m_Tasks.push_back(sThreadManager->PushTask(Core::Utils::StringBuilder("ZONE_WORKER_THREAD_%0", l_I), Core::Threading::TaskType::Normal, 0, std::bind(&ZoneUpdater::WorkerThread, this)));
            m_Activated = true;
        }

        if (p_WorkerCount)
            m_Activated = true;
    }

    /// Is MultiThreading Active
    bool ZoneUpdater::Activated() const
    {
        return m_Activated;
    }

    /// Shut down worker threads
    void ZoneUpdater::Deactivate()
    {
        m_CancelationToken = true;

        /// Finish all requests before we clear
        Wait();

        m_UpdaterTask.ShutDown();

        for (auto l_Itr : m_Tasks)
            sThreadManager->PopTask(l_Itr);
    }

    /// Schedule Update
    /// @p_Zone : Zone
    /// @p_Diff : Execution Time
    void ZoneUpdater::ScheduleUpdate(Zone* p_Zone, uint32 const p_Diff)
    {
        std::lock_guard<std::mutex> l_Guard(m_Lock);

        m_PendingRequests++;

        m_UpdaterTask.Push((ZoneUpdaterTask*)new ZoneUpdateRequest(p_Zone, *this, p_Diff));
    }

    /// Worker Thread
    bool ZoneUpdater::WorkerThread()
    {
        while (true)
        {
            ZoneUpdaterTask* l_ZoneUpdaterTask = nullptr;
            m_UpdaterTask.WaitAndPop(l_ZoneUpdaterTask);

            if (m_CancelationToken)
                return false;

            l_ZoneUpdaterTask->Call();

            delete l_ZoneUpdaterTask;
        }

        return true;
    }

    /// Wait till we have finished all requests
    void ZoneUpdater::Wait()
    {
        std::unique_lock<std::mutex> l_Guard(m_Lock);

        while (m_PendingRequests > 0)
            m_Condition.wait(l_Guard);

        l_Guard.unlock();
    }

    /// Update Finished
    void ZoneUpdater::Finished()
    {
        std::lock_guard<std::mutex> l_Guard(m_Lock);

        m_PendingRequests--;

        m_Condition.notify_all();
    }

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone