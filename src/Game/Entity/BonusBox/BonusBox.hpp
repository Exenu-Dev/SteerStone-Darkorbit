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
#include "Object.hpp"
#include "GameFlags.hpp"
#include "Diagnostic/DiaIntervalTimer.hpp"

#define FRIENDLY_CARGO_TIMER 8000ul

namespace SteerStone { namespace Game { namespace Entity {

    /// Bonus Box
    class BonusBox : public Object
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

        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
    public:
        bool IsFriendlyCargo()      const { return m_IsFriendlyCargo;   }
        bool IsScheduledForDelete() const { return m_ScheduleForDelete; }
        BonusBoxType GetBoxType()   const { return m_BoxType;           }
        int32 GetOwnerId()          const { return m_OwnerId;           }

        uint32 GetPrometium()       const { return m_Prometium;         }
        uint32 GetEndurium()        const { return m_Endurium;          }
        uint32 GetTerbium()         const { return m_Terbium;           }
        uint32 GetPrometid()        const { return m_Prometid;          }
        uint32 GetDuranium()        const { return m_Duranium;          }
        uint32 GetPromerium()       const { return m_Promerium;         }
        uint32 GetXenomit()         const { return m_Xenomit;           }
        uint32 GetSeprom()          const { return m_Seprom;            }
        uint32 GetPalladium()       const { return m_Palladium;         }

        void SetScheduleForDelete(bool const p_Schedule) { m_ScheduleForDelete = p_Schedule; }
        void SetOwnerId(int32 const p_OwnerId) { m_OwnerId = p_OwnerId; }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        int32 m_OwnerId;        ///< Owner of bonus box
        BonusBoxType m_BoxType;

        uint32 m_Prometium;
        uint32 m_Endurium;
        uint32 m_Terbium;
        uint32 m_Prometid;
        uint32 m_Duranium;
        uint32 m_Promerium;
        uint32 m_Xenomit;
        uint32 m_Seprom;
        uint32 m_Palladium;

        bool m_ScheduleForDelete;
        bool m_IsFriendlyCargo;
        Core::Diagnostic::IntervalTimer m_IntervalFriendlyCargo;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone