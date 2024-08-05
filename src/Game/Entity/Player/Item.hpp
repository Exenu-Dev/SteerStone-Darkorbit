/*
* Liam Ashdown
* Copyright (C) 2022
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
#include "GameFlags.hpp"
#include "json.hpp"
#include "GameFlags.hpp"
#include "json.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    struct ItemTemplate;

    class Item
    {
    public:
        friend class Inventory;

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        Item();
        /// Deconstructor
        ~Item();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////
    //            GETTERS/SETTERS
    ///////////////////////////////////////////
    public:
        bool IsWeapon()   const { return m_IsWeapon; }
        bool IsSpeed()    const { return m_IsSpeed;  }
        bool IsShield()   const { return m_IsShield; }
        bool IsExtra()    const { return m_IsExtra;  }

        uint32 GetId() const { return m_Id; }
        std::string GetUniqueId() const { return m_UniqueId; }

        template<typename KeyType>
        KeyType GetMetaData(const std::string& key) { return m_MetaData[key].get<KeyType>(); }
        void UpdateMetaData(const std::string& key, const nlohmann::json& value) { m_MetaData[key] = value; }

        /// Get Jump Chip Type
        JumpChipType GetJumpChipType() const;

        ItemTemplate const* GetItemTemplate() const { return m_ItemTemplate; }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        uint32 m_Id;
        bool m_IsWeapon;
        bool m_IsSpeed;
        bool m_IsShield;
        bool m_IsExtra;
        std::string m_UniqueId;
        nlohmann::json m_MetaData;
        ItemTemplate const* m_ItemTemplate;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone