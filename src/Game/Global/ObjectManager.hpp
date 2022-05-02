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

#pragma once
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"
#include "Singleton/Singleton.hpp"
#include "ObjectData.hpp"
#include "GameFlags.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    class Unit;
    class Portal;
    class Station;
    class Mob;

}   ///< Entity
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Global {

    typedef std::unordered_map<uint32, Entity::MobTemplate*> MobMap;
    typedef std::unordered_map<uint32, std::vector<Entity::PortalTemplate*>> PortalMap;
    typedef std::unordered_map<uint32, Entity::StationTemplate*> StationMap;
    typedef std::unordered_map<uint32, Entity::ItemTemplate*> ItemTemplate;
    typedef std::unordered_map<uint32, Entity::ShipTemplate*> ShipTemplate;

    /// Object Manager
    class Manager
    {
        SINGLETON_P_D(Manager);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Load Mob Template
        void LoadMobTemplate();
        /// Load Portal Template
        void LoadPortalTemplate();
        /// Load Station Template
        void LoadStationTemplate();
        /// Load Item Template
        void LoadItemTemplate();
        /// Load Ship Template
        void LoadShipTemplate();

        /// Returns Mob Template
        /// @p_Entry : Entry
        Entity::MobTemplate const* GetMobTemplate(uint32 const p_Entry);
        /// Returns storage
        /// @p_MapId : Map Id
        std::vector<Entity::PortalTemplate*> const& GetPortalTemplate(uint32 const p_MapId);
        /// Returns Station Template
        /// @p_MapId : Map Id
        Entity::StationTemplate const* GetStationTemplate(uint32 const p_MapId);
        /// Returns Item Template
        /// @p_Entry : Entry Id
        Entity::ItemTemplate const* GetItemTemplate(uint32 const p_Entry);
        /// Returns Ship Template
        /// @p_Entry : Entry Id
        Entity::ShipTemplate const* GetShipTemplate(uint32 const p_Entry);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        MobMap m_MobTemplate;
        PortalMap m_PortalTemplate;
        StationMap m_StationTemplate;
        ItemTemplate m_ItemTemplate;
        ShipTemplate m_ShipTemplate;
    };

}   ///< namespace Global
}   ///< namespace Game
}   ///< namespace Steerstone

#define sObjectManager SteerStone::Game::Global::Manager::GetSingleton()