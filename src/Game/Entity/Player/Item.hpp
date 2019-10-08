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
        uint32 GetCount() const { return m_Count;    }

        ItemTemplate const* GetItemTemplate() { return m_ItemTemplate; }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        uint32 m_Count;
        bool m_IsWeapon;
        bool m_IsSpeed;
        bool m_IsShield;
        ItemTemplate const* m_ItemTemplate;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone