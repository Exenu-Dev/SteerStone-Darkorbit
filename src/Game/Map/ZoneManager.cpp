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

#include "ZoneManager.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "Config/Config.hpp"
#include "Database/DatabaseTypes.hpp"

namespace SteerStone { namespace Game { namespace Map {

    SINGLETON_P_I(Manager);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    Manager::Manager()
    {
        m_IntervalTimer.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MAP_INTERVAL));
    }
    /// Deconstructor
    Manager::~Manager()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Load Maps
    void Manager::LoadMaps()
    {
        /// There are 29 total maps
        static const int32 l_Maps  = 29;
        static int32 l_WorkerCount = sConfigManager->GetInt("MapThreads", 0);

        if (l_WorkerCount > l_Maps)
        {
            LOG_ERROR("MapManager", "There can only be max 1 thread per map! changing to 1 thread per map.");
            l_WorkerCount = l_Maps;
        }

        for (uint32 l_I = 0; l_I < l_WorkerCount; l_I++)
            m_Zones[l_I] = new Zone();

        static int16 l_Remainder        = l_Maps % l_WorkerCount;
        static const int16 l_MapCount   = l_Remainder == 0 ? l_Maps : ((l_Maps - l_Remainder) / l_WorkerCount);
        static uint32 l_Increment       = l_MapCount;
        static uint32 l_Counter         = 0;

        for (auto& l_Itr : m_Zones)
        {
            for (;l_Counter < l_Increment; l_Counter++)
                l_Itr.second->AddMap(new Map::Base(l_Counter));

            l_Increment += l_MapCount;
        }

        /// There's a remainer, try to split the remainder equally to all zones
        while (l_Remainder > 0)
        {
            for (auto& l_Itr : m_Zones)
            {
                l_Itr.second->AddMap(new Map::Base(l_Counter));

                l_Counter++;
                l_Remainder--;
            }
        }

        m_ZoneUpdater.Activate(l_WorkerCount);
    }

    void Manager::AddToMap(Entity::Object* p_Object)
    {
        p_Object->GetMap()->Add(p_Object, true);
    }
    /// Remove From Map
    /// @p_Object : Object being removed from map
    void Manager::RemoveFromMap(Entity::Object* p_Object)
    {
        p_Object->GetMap()->Remove(p_Object, true);
    }

    /// Unload Maps
    void Manager::UnloadAll()
    {
        /// Close all operations
        m_ZoneUpdater.Deactivate();
    }

    /// Get Map
    /// @p_Id : Map Id
    Map::Base* Manager::GetMap(uint32 const p_Id)
    {
        for (auto& l_Itr : m_Zones)
        {
            if (Map::Base* l_Map = l_Itr.second->GetMap(p_Id))
                return l_Map;
        }

        LOG_ASSERT(false, "MapManager", "Map Id: %0 does not exist", p_Id);

        return nullptr;
    }

    /// Update Zones
    /// @p_Diff : Execution Time
    void Manager::Update(uint32 const p_Diff)
    {
        m_IntervalTimer.Update(p_Diff);
        if (!m_IntervalTimer.Passed())
            return;

        for (auto& l_Itr : m_Zones)
        {
            if (m_ZoneUpdater.Activated())
                m_ZoneUpdater.ScheduleUpdate(l_Itr.second, p_Diff);
            else
                l_Itr.second->Update(p_Diff);
        }

        if (m_ZoneUpdater.Activated())
            m_ZoneUpdater.Wait();
    }

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone