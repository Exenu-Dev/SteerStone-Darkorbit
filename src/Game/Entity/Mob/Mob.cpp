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
#include "Mob.hpp"
#include "Utility/UtilRandom.hpp"
#include "Utility/UtilMaths.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Mob::Mob()
    {
        m_Entry         = 0;
        m_Type          = 0;
        m_Experience    = 0;
        m_Behaviour     = Behaviour::BEHAVIOUR_PASSIVE;
        m_Honor         = 0;
        m_Credits       = 0;
        m_Uridium       = 0;
        m_Prometium     = 0;
        m_Endurium      = 0;
        m_Terbium       = 0;
        m_Prometid      = 0;
        m_Duranium      = 0;
        m_Promerium     = 0;
        m_Xenomit       = 0;
        m_Seprom        = 0;
        m_Palladium     = 0;

        m_MoveTimeMax   = 0;
        m_MoveTimeMin   = 0;
    }
    /// Deconstructor
    Mob::~Mob()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update
    /// @p_Diff : Execution Time
    void Mob::Update(uint32 const p_Diff)
    {
        UpdateMovement(p_Diff);

        Unit::Update(p_Diff);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update Movement
    /// @p_Diff : Execution Time
    void Mob::UpdateMovement(uint32 const p_Diff)
    {
        /// This switch statement is not affected by move timer
        switch (m_Behaviour)
        {
            /// Find closest player, and move to player
           /* case Behaviour::BEHAVIOUR_PASSIVE:
            {
                if (!HasTarget())
                {
                    Entity::Object* l_Player = GetGrid()->FindNearestPlayer(this, MAX_DISTANCE_FOLLOW);

                    if (!l_Player)
                        break;

                    /// Set our target
                    SetTarget(l_Player);
                }

                Object const* l_Target = GetTarget();

                float l_PositionX = l_Target->GetSpline()->GetPositionX();
                float l_PositionY = l_Target->GetSpline()->GetPositionY();

                float l_Distance = Core::Utils::DistanceSquared(GetSpline()->GetPositionX(), GetSpline()->GetPositionY(), l_PositionX, l_PositionY);

                /// It doesn't matter if mob is on another grid, the pooling system will handle this
                if (l_Target->ToUnit()->GetTargetGUID() != GetGUID() && l_Target->ToUnit()->GetDeathState() != DeathState::ALIVE || l_Target->ToPlayer()->IsJumping() || l_Distance > MAX_DISTANCE_FOLLOW)
                {
                    ClearTarget();
                    break;
                }

                /// If target is moving, then start following
                /// TODO; Check for out of bounds
                if (l_Target->GetSpline()->IsMoving())
                    GetSpline()->Move(l_PositionX + MAX_DISTANCE_AWAY_FROM_PLAYER, l_PositionY + MAX_DISTANCE_AWAY_FROM_PLAYER, 0, 0);

                return;
            }
            break;*/
            /// Find closest player, and move to player
            case Behaviour::BEHAVIOUR_AGGESSIVE:
            {
                if (!HasTarget())
                {
                    Entity::Object* l_Player = GetGrid()->FindNearestPlayer(this, MAX_DISTANCE_FOLLOW);

                    if (!l_Player)
                        break;

                    /// Set our target
                    SetTarget(l_Player);
                }

                Object const* l_Target = GetTarget();
               
                float l_PositionX = l_Target->GetSpline()->GetPositionX();
                float l_PositionY = l_Target->GetSpline()->GetPositionY();

                float l_Distance = Core::Utils::DistanceSquared(GetSpline()->GetPositionX(), GetSpline()->GetPositionY(), l_PositionX, l_PositionY);

                /// It doesn't matter if mob is on another grid, the pooling system will handle this
                if (l_Target->ToUnit()->GetDeathState() != DeathState::ALIVE || l_Target->ToPlayer()->IsJumping() || l_Distance > MAX_DISTANCE_FOLLOW)
                {
                    ClearTarget();
                    break;
                }

                /// If target is moving, then start following
                /// TODO; Check for out of bounds
                if (l_Target->GetSpline()->IsMoving())
                    GetSpline()->Move(l_PositionX + MAX_DISTANCE_AWAY_FROM_PLAYER, l_PositionY + MAX_DISTANCE_AWAY_FROM_PLAYER, 0, 0);
                else
                {
                    m_IntervalMoveTimer.Update(p_Diff);
                    if (!m_IntervalMoveTimer.Passed())
                        return;

                    float l_Degree = Core::Utils::FloatRandom(0, 360) * M_PI / 180;

                    l_PositionX += MAX_DISTANCE_AWAY_FROM_PLAYER * std::cos(l_Degree);
                    l_PositionY += MAX_DISTANCE_AWAY_FROM_PLAYER * std::sin(l_Degree);

                    GetSpline()->Move(l_PositionX, l_PositionY, 0, 0);

                    m_IntervalMoveTimer.SetInterval(m_MoveTimeMax);
                }

                return;
            }
            break;
        }

        m_IntervalMoveTimer.Update(p_Diff);
        if (!m_IntervalMoveTimer.Passed())
            return;

        /// Move around in our grid
        float l_GridX = std::get<0>(GetGridIndex()) + 1;
        float l_GridY = std::get<1>(GetGridIndex()) + 1;

        float l_MaxPositionX = l_GridX == 0 ? GetMap()->GetGridRadiusX() : l_GridX * GetMap()->GetGridRadiusX();
        float l_MaxPositionY = l_GridY == 0 ? GetMap()->GetGridRadiusY() : l_GridY * GetMap()->GetGridRadiusY();

        float l_MinPositionX = l_MaxPositionX - DISTANCE_AWAY_FROM_BORDER;
        float l_MinPositionY = l_MaxPositionY - DISTANCE_AWAY_FROM_BORDER;
        
        l_MinPositionX = (l_MinPositionX + MAX_DISTANCE_ROAMING_RANGE) - l_MinPositionX;
        l_MinPositionY = (l_MinPositionY + MAX_DISTANCE_ROAMING_RANGE) - l_MinPositionY;

        if (l_GridX > 1)
            l_MinPositionX = (l_GridX - 1) * GetMap()->GetGridRadiusX();

        if (l_GridY > 1)
            l_MinPositionY = (l_GridY - 1) * GetMap()->GetGridRadiusY();

        float l_NewPositionX = Core::Utils::FloatRandom(l_MinPositionX, l_MaxPositionX - DISTANCE_AWAY_FROM_BORDER);
        float l_NewPositionY = Core::Utils::FloatRandom(l_MinPositionY, l_MaxPositionY - DISTANCE_AWAY_FROM_BORDER);

        GetSpline()->Move(l_NewPositionX, l_NewPositionY, 0, 0);

        m_IntervalMoveTimer.SetInterval(Core::Utils::FloatRandom(m_MoveTimeMin, m_MoveTimeMax));
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone