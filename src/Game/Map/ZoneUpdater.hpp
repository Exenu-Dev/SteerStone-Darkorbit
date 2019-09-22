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

#pragma once
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"
#include "ZoneProducer.hpp"
#include <mutex>
#include <atomic>

namespace SteerStone { namespace Core { namespace Threading {

    class Task;

}   ///< namespace Threading
}   ///< namespace Core
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Map {

    class Zone;
    class ZoneUpdater;

    /// Zone Updater Task
    class ZoneUpdaterTask
    {
    public:
        /// Constructor
        /// @p_ZoneUpdater : Zone Updater
        ZoneUpdaterTask(ZoneUpdater* p_ZoneUpdater);
        /// Deconstructor
        virtual ~ZoneUpdaterTask();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        virtual void Call() = 0;

        /// Notify that the task is done
        void UpdateFinished();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        ZoneUpdater* m_ZoneUpdater;
    };

    /// Zone Update Request
    class ZoneUpdateRequest : public ZoneUpdaterTask
    {
        DISALLOW_COPY_AND_ASSIGN(ZoneUpdateRequest);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        /// @p_Zone        : Zone
        /// @p_ZoneUpdater : Zone Updater
        /// @p_Diff        : Execution Time
        ZoneUpdateRequest(Zone* p_Zone, ZoneUpdater& p_ZoneUpdater, uint32 const p_Diff);
        /// Deconstructor
        ~ZoneUpdateRequest();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Call Zone Update
        void Call() override;

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        Zone* m_Zone;   ///< Zone
        uint32 m_Diff;  ///< Execution Time
    };

    /// Zone Updater
    class ZoneUpdater
    {
    public:
        /// Constructor
        ZoneUpdater();
        /// Deconstructor
        ~ZoneUpdater();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
    public:

        /// Activate Threads
        /// @p_WorkerCount : Number of threads to spawn
        void Activate(uint32 p_WorkerCount);

        /// Is MultiThreading Active
        bool Activated() const;

        /// Shut down worker threads
        void Deactivate();

        /// Schedule Update
        /// @p_Zone : Zone
        /// @p_Diff : Execution Time
        void ScheduleUpdate(Zone* p_Zone, uint32 const p_Diff);

        /// Update Finished
        void Finished();

        /// Wait till we have finished all requests
        void Wait();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
    private:
        /// Worker Thread
        bool WorkerThread();

    private:
        ZoneProducer<ZoneUpdaterTask*> m_UpdaterTask;   ///< Updater Task

        bool m_Activated;
        std::atomic<bool> m_CancelationToken;
        std::vector<std::shared_ptr<Core::Threading::Task>> m_Tasks;
        std::mutex m_Lock;
        std::condition_variable m_Condition;
        uint32 m_PendingRequests;
    };

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone