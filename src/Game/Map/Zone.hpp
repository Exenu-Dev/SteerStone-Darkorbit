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
#include "Singleton/Singleton.hpp"
#include "Utility/UtiLockedQueue.hpp"
#include "Threading/ThrTask.hpp"

#include "ZoneProducer.hpp"
#include "ZoneUpdater.hpp"
#include "Map.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    class Object;

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Map {

    class Entity::Object;

    /// Zone
    class Zone
    {
        DISALLOW_COPY_AND_ASSIGN(Zone);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        /// @p_WorkerCount : Worker Count
        Zone(uint32 const p_WorkerCount);
        /// Deconstructor
        ~Zone();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Start Updating Maps
        void Start();

        /// Add a new map
        /// @p_Base : Map being added
        void AddMap(Map::Base* p_Base);

        /// Get Map
        /// @p_Id : Map Id
        Map::Base* GetMap(uint32 const p_Id);

        /// Add to Map
        /// @p_Object : Object being added to map
        void AddObject(Entity::Object* p_Object);
        /// Remove Object from map
        /// @p_Object : Object being remvoed from map
        void RemoveObject(Entity::Object* p_Object);

        /// Schedule to update the zone
        /// @p_Diff : Execution Time
        void ScheduleUpdate(uint32 const p_Diff);

        /// Update Tasks
        /// @p_Diff : Execution Time
        bool Update();

        /// Update Masps
        /// @p_Diff : Execution Time
        void UpdateMaps(uint32 const p_Diff);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        std::unordered_map<uint32, Base*> m_Maps;       ///< Maps
        Core::Threading::Task::Ptr m_Task;              ///< Update Thread Task
        ZoneProducer<ZoneUpdaterTask*> m_UpdaterTask;   ///< Updater Task

        uint32 m_WorkerId;                              ///< Worker Id
        std::mutex m_Mutex;                             ///< Global Mutex
    };

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone