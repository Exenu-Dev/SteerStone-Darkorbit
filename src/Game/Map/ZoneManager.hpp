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
#include "GameFlags.hpp"
#include "Zone.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    class Object;

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Map {

    /// Map Manager
    class Manager
    {
        SINGLETON_P_D(Manager);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Load Maps
        void LoadMaps();

        /// Add to Map
        /// @p_Object : Object being added to map
        void AddToMap(Entity::Object* p_Object);
        /// Remove From Map
        /// @p_Object : Object being removed from map
        void RemoveFromMap(Entity::Object* p_Object);

        /// Get Map
        /// @p_Id : Map Id
        Map::Base* GetMap(uint32 const p_Id);

        /// Update Zones
        /// @p_Diff : Execution Time
        void Update(uint32 const p_Diff);

    private:
        std::unordered_map<uint32, Zone*> m_Zones;
        Core::Diagnostic::IntervalTimer m_IntervalTimer;
    };

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone

#define sZoneManager SteerStone::Game::Map::Manager::GetSingleton()