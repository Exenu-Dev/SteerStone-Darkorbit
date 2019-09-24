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

#include "Player.hpp"
#include "ZoneManager.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Unit::Unit()
    {
        m_Shield         = 0;
        m_MaxShield      = 0;
        m_HitPoints      = 0;
        m_MaxHitPoints   = 0;
        m_GatesAchieved  = 0;
        m_ClanId         = 0;
        m_ClanName.clear();
        m_Company        = Company::NOMAD;
        m_Rank           = 0;
        m_WeaponState    = 0;

        SetType(Type::OBJECT_TYPE_NPC);
        SetGUID(ObjectGUID(GUIDType::NPC));
    }
    /// Deconstructor
    Unit::~Unit()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update
    /// @p_Diff : Execution Time
    void Unit::Update(uint32 const p_Diff)
    {
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone