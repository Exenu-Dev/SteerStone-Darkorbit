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

#include "BonusBox.hpp"
#include "ZoneManager.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    /// @p_Type : Type of Bonus Box
    BonusBox::BonusBox(BonusBoxType p_Type)
        : m_BoxType(p_Type)
    {
        m_OwnerId = -1;
        m_IsFriendlyCargo = true;
        m_ScheduleForDelete = false;
        m_IntervalFriendlyCargo.SetInterval(FRIENDLY_CARGO_TIMER);

        SetType(Type::OBJECT_TYPE_BONUS_BOX);
        SetGUID(ObjectGUID(GUIDType::BonusBox));
    }
    /// Deconstructor
    BonusBox::~BonusBox()
    {
        sZoneManager->RemoveFromMap(this, true);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////
    //                GENERAL
    ///////////////////////////////////////////

    /// Update
    /// @p_Diff : Execution Time
    void BonusBox::Update(uint32 const p_Diff)
    {
        m_IntervalFriendlyCargo.Update(p_Diff);
        if (m_IntervalFriendlyCargo.Passed())
        {
            if (m_IsFriendlyCargo)
                SetNeedToBeUpdated(true);

            m_IsFriendlyCargo = false;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone