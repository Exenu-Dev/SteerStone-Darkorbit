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

#include "Map.hpp"
#include "Player.hpp"
#include "Database/DatabaseTypes.hpp"
#include "Zone.hpp"
#include "Diagnostic/DiaServerWatch.hpp"
#include "Threading/ThrTaskManager.hpp"
#include "Utility/UtiString.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Constructor
    /// @p_WorkerCount : Worker Count
    Zone::Zone(uint32 const p_Worker)
        : m_WorkerId(p_Worker)
    {
    }
    /// Deconstructor
    Zone::~Zone()
    {
        sThreadManager->PopTask(m_Task);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Start Updating Maps
    void Zone::Start()
    {
        m_Task = sThreadManager->PushTask(Core::Utils::StringBuilder("ZONE_WORKER_THREAD_%0", m_WorkerId), Core::Threading::TaskType::Normal, 0, std::bind(&Zone::Update, this));
    }

    /// Add a new map
    /// @p_Base : Map being added
    void Zone::AddMap(Map::Base* p_Base)
    {
        m_Maps[p_Base->GetId()] = p_Base;

        /// Load Map
        p_Base->Load();
    }

    /// Get Map
    /// @p_Id : Map Id
    Map::Base* Zone::GetMap(uint32 const p_Id)
    {
        return m_Maps[p_Id];
    }

    /// Add to Map
    /// @p_Object : Object being added to map
    void Zone::AddObject(Entity::Object* p_Object)
    {
        p_Object->GetMap()->Add(p_Object);
    }
    /// Remove Object from map
    /// @p_Object : Object being remvoed from map
    void Zone::RemoveObject(Entity::Object* p_Object)
    {
        p_Object->GetMap()->Remove(p_Object);
    }

    /// Schedule to update the zone
    /// @p_Diff : Execution Time
    void Zone::ScheduleUpdate(uint32 const p_Diff)
    {
        std::lock_guard<std::mutex> l_Guard(m_Mutex);

        m_UpdaterTask.Push((ZoneUpdaterTask*)new ZoneUpdateRequest(this, p_Diff));
    }

    /// Update Tasks
    bool Zone::Update()
    {
        while (true)
        {
            ZoneUpdaterTask* l_ZoneUpdaterTask = nullptr;
            m_UpdaterTask.WaitAndPop(l_ZoneUpdaterTask);

            l_ZoneUpdaterTask->Call();

            delete l_ZoneUpdaterTask;
        }

        return true;
    }

    /// Update Maps
    /// @p_Diff : Execution Time
    void Zone::UpdateMaps(const uint32 p_Diff)
    {
        /// Update all of our maps
        for (auto l_Itr : m_Maps)
            l_Itr.second->Update(p_Diff);
    }

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone