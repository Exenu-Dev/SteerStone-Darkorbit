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
#include "Diagnostic/DiaIntervalTimer.hpp"
#include "Unit.hpp"

#define DISTANCE_AWAY_FROM_BORDER 100          ///< Distance away from border
#define MAX_LAST_TIME_ATTACKED 10000

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

        ///////////////////////////////////////////
        //              GENERAL
        ///////////////////////////////////////////
    public:
        /// Update
        /// @p_Diff : Execution Time
        void Update(uint32 const p_Diff);

        /// Reward Credit/Uridium...
        /// @p_Player : Player is being rewarded
        void RewardKillCredit(Player* p_Player);

    private:
        /// Update Movement
        /// @p_Diff : Execution Time
        void UpdateMovement(uint32 const p_Diff);
        /// Handle Roaming
        /// @p_Distance : Distance from target
        /// @p_PositionX : Position X
        /// @p_PositionY : Position Y
        /// @p_Diff : Execution Time
        void HandleRoaming(const float p_Distance, float p_PositionX, float p_PositionY, uint32 const p_Diff);

    public:
        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////

        uint32 GetId()                                  const { return m_Entry;                                  }

        Player const* GetTaggedPlayer()                 const { return m_PlayerTagger;                           }
        void SetTaggedPlayer(Player const* p_Player)          { m_PlayerTagger = p_Player;                       }
        bool IsTaggedByPlayer()                         const { return m_PlayerTagger != nullptr ? true : false; }
        bool IsFleeing()                                const { return m_Fleeing;                                }

        /// Set Is Fleeing
        /// @p_Fleeing : Fleeing
        void SetIsFleeing(bool p_Fleeing);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        uint32 m_Entry;

        int32 m_RandomDistanceFromPlayerX;
        int32 m_RandomDistanceFromPlayerY;

        float m_HomePositionX;
        float m_HomePositionY;

        Player const* m_PlayerTagger; ///< Player is targetting us

        bool m_Fleeing; ///< Mob is fleeing away from player

        Core::Diagnostic::IntervalTimer m_IntervalMoveTimer;
        Core::Diagnostic::IntervalTimer m_IntervalRespawnTimer;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone