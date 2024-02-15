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
#include "Unit.hpp"
#include "GameFlags.hpp"
#include "Diagnostic/DiaIntervalTimer.hpp"

#define FRIENDLY_CARGO_TIMER 8000ul
#define FRIENDLY_HONOR_TAKE_AWAY 500ul

namespace SteerStone { namespace Game { namespace Entity {

    /// Bonus Box
    class BonusBox : public Unit
    {
    public:
        friend class Map::PoolManager;

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        /// @p_Type : Type of Bonus Box
        BonusBox(BonusBoxType p_Type);
        /// Deconstructor
        ~BonusBox();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////
        //                GENERAL
        ///////////////////////////////////////////
        
        /// Update
        /// @p_Diff : Execution Time
        void Update(uint32 const p_Diff);

        /// Reward Credit on loot
        /// @p_Player : Player being rewarded
        void RewardCredit(Entity::Player* p_Player);

        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
    public:
        bool IsFriendlyCargo()      const { return m_IsFriendlyCargo;   }
        bool IsLooted()             const { return m_Looted;            }  
        BonusBoxType GetBoxType()   const { return m_BoxType;           }
        int32 GetOwnerId()          const { return m_OwnerId;           }

        void SetLooted() { m_Looted = true; }
        void SetOwnerId(int32 const p_OwnerId) { m_OwnerId = p_OwnerId; }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        int32 m_OwnerId;        ///< Owner of bonus box
        BonusBoxType m_BoxType;
        bool m_Looted;
        bool m_IsFriendlyCargo;
        Core::Diagnostic::IntervalTimer m_IntervalFriendlyCargo;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone