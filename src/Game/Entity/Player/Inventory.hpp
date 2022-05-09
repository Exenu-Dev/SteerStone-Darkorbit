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

#include "Item.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    class Player;

    class Inventory
    {
        DISALLOW_COPY_AND_ASSIGN(Inventory);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        /// @p_Player : Player owner
        Inventory(Player* p_Player);
        /// Deconstructor
        ~Inventory();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    public:
        /// Load Inventory from database
        void LoadInventory();

        /// Calculate player stats
        void CalculateStats();

        uint32 GetWeaponCount() const { return m_WeaponCount; }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        std::vector<Item> m_Items;
        uint32 m_WeaponCount;
        Player* m_Player;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone