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
#include "Diagnostic/DiaServerWatch.hpp"
#include "Utility/UtilRandom.hpp"
#include "Utility/UtilMaths.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Mob::Mob()
    {
        m_Entry         = 0;

        m_HomePositionX = 0.0f;
        m_HomePositionY = 0.0f;

        m_PlayerTagger = nullptr;

        m_MoveTimeMax   = 0;
        m_MoveTimeMin   = 0;

        m_RandomDistanceFromPlayerX = 0;
        m_RandomDistanceFromPlayerY = 0;

        m_AttackRange = sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MOB_ATTACK_RANGE);

        SetType(Type::OBJECT_TYPE_MOB);
        SetGUID(ObjectGUID(GUIDType::MOB));
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
                    Entity::Player* l_Player = GetGrid()->FindNearestPlayer(this, sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_FIND_PLAYER_DISTANCE));

                    if (!l_Player)
                        break;

                    /// Cannot target players who are at a portal or station
                    if (l_Player->ToPlayer()->GetEvent() == EventType::EVENT_TYPE_PORTAL || l_Player->ToPlayer()->GetEvent() == EventType::EVENT_TYPE_STATION)
                        break;

                    /// Set our target
                    SetTarget(l_Player);

                    m_RandomDistanceFromPlayerX = Core::Utils::Int32Random(-400, 200);
                    m_RandomDistanceFromPlayerY = Core::Utils::Int32Random(-100, 200);
                }

                Player* l_Target = GetTarget()->ToPlayer();

                /// Cancel attack if target does not exist
                if (!l_Target)
                {
                    CancelAttack();
                    break;
                }

                if (GetTaggedPlayer())
                    LOG_ASSERT(GetTarget()->GetGUID() == GetTaggedPlayer()->GetGUID(), "Mob", "Target and tagged player does not match!");

                float l_PositionX = l_Target->GetSpline()->GetPlannedPositionX();
                float l_PositionY = l_Target->GetSpline()->GetPlannedPositionY();
                float l_Distance = Core::Utils::DistanceSquared(GetSpline()->GetPlannedPositionX(), GetSpline()->GetPlannedPositionY(), l_PositionX, l_PositionY);

                /// Don't follow player if player is at an event, we only attack player if player has tagged us
                if (l_Target->GetEvent() == EventType::EVENT_TYPE_PORTAL || l_Target->GetEvent() == EventType::EVENT_TYPE_STATION)
                {
                    if (!GetTaggedPlayer())
                    {
                        CancelAttack();
                        return;
                    }
                }

                /// If player last time shot is more than 10 seconds ago, then cancel combat
                if (GetTaggedPlayer())
                {
                    if (sServerTimeManager->GetTimeDifference(m_LastTimeAttacked, sServerTimeManager->GetServerTime()) > MAX_LAST_TIME_ATTACKED)
                    {
                        CancelAttack();
                        break;
                    }
                }
                /// else if player is farther than max distance, then cancel attack
                else if (l_Distance > sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MAX_FOLLOW_DISTANCE))
                {
                    CancelAttack();
                    break;
                }

                /// If player is more than our min distance, start following player
                if (l_Distance > sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MIN_FOLLOW_DISTANCE))
                {
                    GetSpline()->Move(l_PositionX, l_PositionY, 0, 0);

                    /// We want to update the movement, as soon as possible to ensure we are keeping up to the player
                    m_IntervalMoveTimer.SetInterval(m_MoveTimeMax / 2);
                }
                /// Check if we are too close to target, if so - move back
                else if (l_Distance < sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MIN_FOLLOW_DISTANCE))
                {
                    float l_Degree = Core::Utils::FloatRandom(0, 360) * M_PI / 180;

                    l_PositionX += std::abs(m_RandomDistanceFromPlayerX) * std::cos(l_Degree);
                    l_PositionY += std::abs(m_RandomDistanceFromPlayerY) * std::sin(l_Degree);

                    GetSpline()->Move(l_PositionX, l_PositionY, 0, 0);

                    m_IntervalMoveTimer.SetInterval(m_MoveTimeMax);
                }

                /// If we are not attacking, roam around the player
                if (!IsAttacking())
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
                    Entity::Player* l_Player = GetGrid()->FindNearestPlayer(this, sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_FIND_PLAYER_DISTANCE));

                    if (!l_Player)
                        break;

                    /// Cannot target players who are at a portal or station
                    if (l_Player->ToPlayer()->GetEvent() == EventType::EVENT_TYPE_PORTAL || l_Player->ToPlayer()->GetEvent() == EventType::EVENT_TYPE_STATION)
                        break;

                    /// Set our target
                    SetTarget(l_Player);

                    m_RandomDistanceFromPlayerX = Core::Utils::Int32Random(-400, 200);
                    m_RandomDistanceFromPlayerY = Core::Utils::Int32Random(-100, 200);
                }

                Player* l_Target = GetTarget()->ToPlayer();

                /// Cancel attack if target does not exist
                if (!l_Target)
                {
                    CancelAttack();
                    break;
                }

                if (GetTaggedPlayer())
                    LOG_ASSERT(GetTarget()->GetGUID() == GetTaggedPlayer()->GetGUID(), "Mob", "Target and tagged player does not match!");

                float l_PositionX = l_Target->GetSpline()->GetPlannedPositionX();
                float l_PositionY = l_Target->GetSpline()->GetPlannedPositionY();
                float l_Distance = Core::Utils::DistanceSquared(GetSpline()->GetPlannedPositionX(), GetSpline()->GetPlannedPositionY(), l_PositionX, l_PositionY);

                /// Clear target if target is jumping
                if (l_Target->ToPlayer()->IsJumping())
                {
                    CancelAttack();
                    break;
                }

                /// Don't attack player if player is at an event, we only attack player if player has tagged us
                if (l_Target->GetEvent() == EventType::EVENT_TYPE_PORTAL || l_Target->GetEvent() == EventType::EVENT_TYPE_STATION)
                {
                    if (!GetTaggedPlayer())
                    {
                        CancelAttack();
                        return;
                    }
                }

                /// If player last time shot is more than 10 seconds ago, then cancel combat
                if (GetTaggedPlayer())
                {
                    if (sServerTimeManager->GetTimeDifference(m_LastTimeAttacked, sServerTimeManager->GetServerTime()) > MAX_LAST_TIME_ATTACKED)
                    {
                        CancelAttack();
                        break;
                    }
                }
                /// else if player is farther than max distance, then cancel attack
                else if (l_Distance > sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MAX_FOLLOW_DISTANCE))
                {
                    CancelAttack();
                    break;
                }

                /// If player is more than our min distance, start following player
                if (l_Distance > sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MIN_FOLLOW_DISTANCE))
                {
                    GetSpline()->Move(l_PositionX, l_PositionY, 0, 0);

                    /// We want to update the movement, as soon as possible to ensure we are keeping up to the player
                    m_IntervalMoveTimer.SetInterval(m_MoveTimeMax / 2);
                }
                /// Check if we are too close to target, if so - move back
                else if (l_Distance < sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MIN_FOLLOW_DISTANCE))
                {
                    float l_Degree = Core::Utils::FloatRandom(0, 360) * M_PI / 180;

                    l_PositionX += std::abs(m_RandomDistanceFromPlayerX) * std::cos(l_Degree);
                    l_PositionY += std::abs(m_RandomDistanceFromPlayerY) * std::sin(l_Degree);

                    GetSpline()->Move(l_PositionX, l_PositionY, 0, 0);

                    m_IntervalMoveTimer.SetInterval(m_MoveTimeMax);
                }

                if (!IsAttacking())
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