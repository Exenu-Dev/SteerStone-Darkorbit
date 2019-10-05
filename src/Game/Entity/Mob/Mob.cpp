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

#include "Server/MiscPackets.hpp"
#include "World.hpp"
#include "Mob.hpp"
#include "ZoneManager.hpp"
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
        m_RespawnTimer  = 0;
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

        m_RandomDistanceFromPlayerX = 0;
        m_RandomDistanceFromPlayerY = 0;

        m_AttackRange = sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MOB_ATTACK_RANGE);
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
        switch (m_DeathState)
        {
            case DeathState::ALIVE:
            {
                UpdateMovement(p_Diff);

                Unit::Update(p_Diff);
            }
            break;
            case DeathState::JUST_DIED:
            {
                m_DeathState = DeathState::DEAD;

                CancelAttack();

                /// It's easier if we just remove mob from map, so we don't need to do extra
                /// checks whether object is dead etc...
                sZoneManager->RemoveFromMap(this);
            }
            break;
            case DeathState::DEAD:
            {
                m_IntervalRespawnTimer.Update(p_Diff);
                if (!m_IntervalRespawnTimer.Passed())
                    return;

                m_DeathState = DeathState::ALIVE;

                sZoneManager->AddToMap(this);
            }
            break;
            default:
                break;
        }
       
    }

    /// Reward Credit/Uridium...
    /// @p_Player : Player is being rewarded
    void Mob::RewardKillCredit(Player* p_Player)
    {
        p_Player->UpdateCredits(m_Credits);
        p_Player->UpdateUridium(m_Uridium);
        p_Player->UpdateExperience(m_Experience);
        p_Player->UpdateHonor(m_Honor);

        p_Player->ToPlayer()->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_CREDIT,     { m_Credits,    p_Player->GetCredits()    }));
        p_Player->ToPlayer()->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_URIDIUM,    { m_Uridium,    p_Player->GetUridium()    }));
        p_Player->ToPlayer()->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_HONOUR,     { m_Honor,      p_Player->GetHonor()      }));
        p_Player->ToPlayer()->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_EXPERIENCE, { m_Experience, p_Player->GetExperience(), p_Player->GetLevel() }));
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update Movement
    /// @p_Diff : Execution Time
    void Mob::UpdateMovement(uint32 const p_Diff)
    {
        m_IntervalMoveTimer.Update(p_Diff);
        if (!m_IntervalMoveTimer.Passed())
            return;

        switch (m_Behaviour)
        {
            /// Find closest player, and move to player
            case Behaviour::BEHAVIOUR_PASSIVE:
            {
                if (!HasTarget())
                {
                    Entity::Object* l_Player = GetGrid()->FindNearestPlayer(this, sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_FIND_PLAYER_DISTANCE));

                    if (!l_Player)
                        break;

                    /// Set our target
                    SetTarget(l_Player);

                    m_RandomDistanceFromPlayerX = Core::Utils::Int32Random(-400, 200);
                    m_RandomDistanceFromPlayerY = Core::Utils::Int32Random(-100, 200);
                }

                Object const* l_Target = GetTarget();

                /// Cancel attack if target does not exist
                if (!l_Target->ToPlayer())
                {
                    CancelAttack();
                    break;
                }

                float l_PositionX = l_Target->GetSpline()->GetPlannedPositionX();
                float l_PositionY = l_Target->GetSpline()->GetPlannedPositionY();
                float l_Distance = Core::Utils::DistanceSquared(GetSpline()->GetPlannedPositionX(), GetSpline()->GetPlannedPositionY(), l_PositionX, l_PositionY);

                /// Clear target if target is jumping
                if (l_Target->ToPlayer()->IsJumping())
                {
                    CancelAttack();
                    break;
                }
                /// Follow the target if our distance is out of range
                else if (l_Distance > sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MIN_FOLLOW_DISTANCE) && l_Distance < sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MAX_FOLLOW_DISTANCE))
                {
                    GetSpline()->Move(l_PositionX + m_RandomDistanceFromPlayerX, l_PositionY + m_RandomDistanceFromPlayerY, 0, 0);

                    /// We want to update the movement, as soon as possible to ensure we are keeping up to the player
                    m_IntervalMoveTimer.SetInterval(m_MoveTimeMax / 2);
                }
                else if (l_Distance > sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MAX_FOLLOW_DISTANCE))
                {
                    /// If target is far away, cancel the target and continue moving around in the ground
                    CancelAttack();
                    break;
                }
                else if (!IsAttacking())
                {
                    float l_Degree = Core::Utils::FloatRandom(0, 360) * M_PI / 180;

                    l_PositionX += std::abs(m_RandomDistanceFromPlayerX) * std::cos(l_Degree);
                    l_PositionY += std::abs(m_RandomDistanceFromPlayerY) * std::sin(l_Degree);

                    GetSpline()->Move(l_PositionX, l_PositionY, 0, 0);

                    m_IntervalMoveTimer.SetInterval(m_MoveTimeMax);
                }
                
                return;
            }
            break;
            /// Find closest player, and move to player
            case Behaviour::BEHAVIOUR_AGGESSIVE:
            {
                if (!HasTarget())
                {
                    Entity::Object* l_Player = GetGrid()->FindNearestPlayer(this, sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_FIND_PLAYER_DISTANCE));

                    if (!l_Player)
                        break;

                    /// Set our target
                    SetTarget(l_Player);

                    m_RandomDistanceFromPlayerX = Core::Utils::Int32Random(-400, 200);
                    m_RandomDistanceFromPlayerY = Core::Utils::Int32Random(-100, 200);
                }

                Object* l_Target = GetTarget();

                /// Cancel attack if target does not exist
                if (!l_Target->ToPlayer())
                {
                    CancelAttack();
                    break;
                }

                float l_PositionX = l_Target->GetSpline()->GetPlannedPositionX();
                float l_PositionY = l_Target->GetSpline()->GetPlannedPositionY();
                float l_Distance = Core::Utils::DistanceSquared(GetSpline()->GetPlannedPositionX(), GetSpline()->GetPlannedPositionY(), l_PositionX, l_PositionY);

                /// Clear target if target is jumping or our target is far away
                if (l_Target->ToPlayer()->IsJumping())
                {
                    CancelAttack();
                    break;
                }
                /// Follow the target if our distance is out of range
                else if (l_Distance > sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MIN_FOLLOW_DISTANCE) && l_Distance < sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MAX_FOLLOW_DISTANCE))
                {
                    GetSpline()->Move(l_PositionX + m_RandomDistanceFromPlayerX, l_PositionY + m_RandomDistanceFromPlayerY, 0, 0);

                    /// We want to update the movement, as soon as possible to ensure we are keeping up to the player
                    m_IntervalMoveTimer.SetInterval(m_MoveTimeMax / 2);
                }
                else if (l_Distance > sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MAX_FOLLOW_DISTANCE))
                {
                    /// If target is far away, cancel the target and continue moving around in the ground
                    CancelAttack();
                    break;
                }
                else if (l_Distance < sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MIN_FOLLOW_DISTANCE))
                {
                    float l_Degree = Core::Utils::FloatRandom(0, 360) * M_PI / 180;

                    l_PositionX += std::abs(m_RandomDistanceFromPlayerX) * std::cos(l_Degree);
                    l_PositionY += std::abs(m_RandomDistanceFromPlayerY) * std::sin(l_Degree);

                    GetSpline()->Move(l_PositionX, l_PositionY, 0, 0);

                    m_IntervalMoveTimer.SetInterval(m_MoveTimeMax);
                }

                Attack(l_Target);

                return;
            }
            break;
        }

        /// Move around in our grid
        float l_GridX = std::get<0>(GetGridIndex()) + 1;
        float l_GridY = std::get<1>(GetGridIndex()) + 1;

        float l_MaxPositionX = l_GridX == 0 ? GetMap()->GetGridRadiusX() : l_GridX * GetMap()->GetGridRadiusX();
        float l_MaxPositionY = l_GridY == 0 ? GetMap()->GetGridRadiusY() : l_GridY * GetMap()->GetGridRadiusY();

        float l_MinPositionX = l_MaxPositionX - DISTANCE_AWAY_FROM_BORDER;
        float l_MinPositionY = l_MaxPositionY - DISTANCE_AWAY_FROM_BORDER;
        
        l_MinPositionX = (l_MinPositionX + sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MAX_ROAMING_DISTANCE) - l_MinPositionX);
        l_MinPositionY = (l_MinPositionY + sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MAX_ROAMING_DISTANCE) - l_MinPositionY);

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