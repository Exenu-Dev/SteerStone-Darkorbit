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

#include "Map.hpp"
#include "Player.hpp"
#include "Database/DatabaseTypes.hpp"
#include "Zone.hpp"
#include "Diagnostic/DiaServerWatch.hpp"
#include "Threading/ThrTaskManager.hpp"
#include "Utility/UtiString.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Constructor
    Zone::Zone()
    {
    }
    /// Deconstructor
    Zone::~Zone()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

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
    Map::Base* Zone::GetMap(uint32 const p_Id) const
    {
        auto const l_Itr = m_Maps.find(p_Id);
        if (l_Itr != m_Maps.end())
            return l_Itr->second;

        return nullptr;
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

    /// Unload Maps
    void Zone::UnloadAll()
    {
        for (auto l_Itr : m_Maps)
        {
            l_Itr.second->UnloadAll();
            delete l_Itr.second;
        }

        m_Maps.clear();
    }

    /// @p_Diff : Execution Time
    void Zone::Update(const uint32 p_Diff)
    {
        /// Update all of our maps
        for (auto l_Itr : m_Maps)
            l_Itr.second->Update(p_Diff);
    }

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone