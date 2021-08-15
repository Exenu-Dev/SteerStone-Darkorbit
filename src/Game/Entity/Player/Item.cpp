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

#include "Item.hpp"
#include "ObjectData.hpp"

namespace SteerStone { namespace Game { namespace Entity {
    
    /// Constructor
    Item::Item()
    {
        m_Count         = 0;
        m_IsWeapon      = false;
        m_IsSpeed       = false;
        m_IsShield      = false;
        m_ItemTemplate  = nullptr;
    }
    /// Deconstructor
    Item::~Item()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone