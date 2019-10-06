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
#include "Diagnostic/DiaIntervalTimer.hpp"
#include "Unit.hpp"

#define DISTANCE_AWAY_FROM_BORDER 100          ///< Distance away from border

namespace SteerStone { namespace Game { namespace Map {

    class PoolManager;

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Entity {

    enum Behaviour
    {
        BEHAVIOUR_PASSIVE   = 0,
        BEHAVIOUR_AGGESSIVE = 1
    };

    /// Mob
    class Mob : public Unit
    {
        DISALLOW_COPY_AND_ASSIGN(Mob);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        friend class Map::PoolManager;

    public:
        /// Constructor
        Mob();
        /// Deconstructor
        ~Mob();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Getters Functions
        uint32 GetId()          const { return m_Entry;         }
        uint32 GetExperience()  const { return m_Experience;    }
        uint16 GetBehaviour()   const { return m_Behaviour;     }
        uint32 GetRespawnTimer()const { return m_RespawnTimer;  }
        uint32 GetHonor()       const { return m_Honor;         }
        uint32 GetCredits()     const { return m_Credits;       }
        uint32 GetUridium()     const { return m_Uridium;       }
        uint32 GetPrometium()   const { return m_Prometium;     }
        uint32 GetEndurium()    const { return m_Endurium;      }
        uint32 GetTerbium()     const { return m_Terbium;       }
        uint32 GetPrometid()    const { return m_Prometid;      }
        uint32 GetDuranium()    const { return m_Duranium;      }
        uint32 GetPromerium()   const { return m_Promerium;     }
        uint32 GetXenomit()     const { return m_Xenomit;       }
        uint32 GetSeprom()      const { return m_Seprom;        }
        uint32 GetPalladium()   const { return m_Palladium;     }

        /// Update
        /// @p_Diff : Execution Time
        void Update(uint32 const p_Diff);

        /// Reward Credit/Uridium...
        /// @p_Player : Player is being rewarded
        void RewardKillCredit(Player* p_Player);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        /// Update Movement
        /// @p_Diff : Execution Time
        void UpdateMovement(uint32 const p_Diff);

    private:
        uint32 m_Entry;
        uint32 m_Experience;
        uint16 m_Behaviour;
        uint32 m_RespawnTimer;
        uint32 m_Honor;
        uint32 m_Credits;
        uint32 m_Uridium;
        uint32 m_Prometium;
        uint32 m_Endurium;
        uint32 m_Terbium;
        uint32 m_Prometid;
        uint32 m_Duranium;
        uint32 m_Promerium;
        uint32 m_Xenomit;
        uint32 m_Seprom;
        uint32 m_Palladium;

        uint32 m_MoveTimeMax;
        uint32 m_MoveTimeMin;

        int32 m_RandomDistanceFromPlayerX;
        int32 m_RandomDistanceFromPlayerY;

        float m_HomePositionX;
        float m_HomePositionY;

        Core::Diagnostic::IntervalTimer m_IntervalMoveTimer;
        Core::Diagnostic::IntervalTimer m_IntervalRespawnTimer;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone