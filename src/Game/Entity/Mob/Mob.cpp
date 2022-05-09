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

#include "Server/MiscPackets.hpp"
#include "Server/ShipPackets.hpp"
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

        m_RandomDistanceFromPlayerX = 0;
        m_RandomDistanceFromPlayerY = 0;

        m_Fleeing = false;

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
        p_Player->UpdateLogBook("<div class=\"logdata\">You have destroyed " + GetName() + " alien ship.</div>" +
            "<div class=\"logdata\">You received " + std::to_string(m_Entry) + " Experience Points." + "</div>" +
            "<div class=\"logdata\">You received " + std::to_string(m_Credits) + " Credits." + "</div>" +
            "<div class=\"logdata\">You received " + std::to_string(m_Uridium) + " Uridium." + "</div>", LogBookType::LOG_BOOK_TYPE_OVERVIEW);

        p_Player->UpdateLogBook("You received " + std::to_string(m_Experience) + " EP.");
        p_Player->UpdateLogBook("You received " + std::to_string(m_Honor) + " honor.");
        p_Player->UpdateLogBook("You destroyed alien ship " + GetName());

        p_Player->ToPlayer()->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_CREDIT,     { m_Credits,    p_Player->GetCredits()                          }));
        p_Player->ToPlayer()->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_URIDIUM,    { m_Uridium,    p_Player->GetUridium()                          }));
        p_Player->ToPlayer()->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_HONOUR,     { m_Honor,      p_Player->GetHonor()                            }));
        p_Player->ToPlayer()->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_EXPERIENCE, { m_Experience, p_Player->GetExperience(), p_Player->GetLevel() }));
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update Movement
    /// @p_Diff : Execution Time
    void Mob::UpdateMovement(uint32 const p_Diff)
    {
        /// TODO; Do we need to update movement every 1 second?

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
                if (l_Target->GetEvent() == EventType::EVENT_TYPE_PORTAL || l_Target->GetEvent() == EventType::EVENT_TYPE_STATION 
                    || l_Target->GetEvent() == EventType::EVENT_TYPE_RADIATION_ZONE)
                {
                    if (l_Target->GetEvent() == EventType::EVENT_TYPE_RADIATION_ZONE || !GetTaggedPlayer())
                    {
                        CancelAttack();
                        return;
                    }
                }

                /// If player last time shot is more than 10 seconds ago, then cancel combat
                if (GetTaggedPlayer())
                {
                    if (!IsInCombat())
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

                if (IsFleeing())
                {
                    GetSpline()->Move();
                    break;
                } ///< If unit is 15% health or below, then move to nearest corner of the map
                else if (GetHitPoints() <= Core::Utils::CalculatePercentage(GetHitMaxPoints(), 15))
                {
                    SetIsFleeing(true);
                    break;
                }

                HandleRoaming(l_Distance, l_PositionX, l_PositionY, p_Diff);
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

                /// Don't follow player if player is at an event, we only attack player if player has tagged us
                if (l_Target->GetEvent() == EventType::EVENT_TYPE_PORTAL || l_Target->GetEvent() == EventType::EVENT_TYPE_STATION
                    || l_Target->GetEvent() == EventType::EVENT_TYPE_RADIATION_ZONE)
                {
                    if (l_Target->GetEvent() == EventType::EVENT_TYPE_RADIATION_ZONE || !GetTaggedPlayer())
                    {
                        CancelAttack();
                        return;
                    }
                }

                /// If player last time shot is more than 10 seconds ago, then cancel combat
                if (GetTaggedPlayer())
                {
                    if (!IsInCombat())
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

                if (!IsAttacking())
                    Attack(l_Target);

                if (IsFleeing())
                {
                    GetSpline()->Move();
                    break;
                } ///< If unit is 15% health or below, then move to nearest corner of the map
                else if (GetHitPoints() <= Core::Utils::CalculatePercentage(GetHitMaxPoints(), 15))
                {
                    SetIsFleeing(true);
                    break;
                }

                HandleRoaming(l_Distance, l_PositionX, l_PositionY, p_Diff);
            }
            break;
        }
    }
    /// Handle Roaming
    /// @p_Distance : Distance from target
    /// @p_PositionX : Position X
    /// @p_PositionY : Position Y
    /// @p_Diff : Execution Time
    void Mob::HandleRoaming(const float p_Distance, float p_PositionX, float p_PositionY, uint32 const p_Diff)
    {
        /// Movement Update
        m_IntervalMoveTimer.Update(p_Diff);
        if (!m_IntervalMoveTimer.Passed())
            return;

        /// If player is more than our min distance, start following player
        if (p_Distance > sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MIN_FOLLOW_DISTANCE))
            GetSpline()->Move(p_PositionX, p_PositionY);
        /// Check if we are too close to target, if so - move back
        else if (p_Distance < sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MIN_FOLLOW_DISTANCE))
        {
            float l_Degree = Core::Utils::FloatRandom(0, 360) * M_PI / 180;

            p_PositionX += std::abs(m_RandomDistanceFromPlayerX) * std::cos(l_Degree);
            p_PositionY += std::abs(m_RandomDistanceFromPlayerY) * std::sin(l_Degree);

            GetSpline()->Move(p_PositionX, p_PositionY);
        }
    }

    /// Set Is Fleeing
    /// @p_Fleeing : Fleeing
    void Mob::SetIsFleeing(bool p_Fleeing)
    {
        if (p_Fleeing)
        {
            uint32 l_PositionX = GetSpline()->GetPositionX();
            uint32 l_PositionY = GetSpline()->GetPositionY();

            uint32 l_MapDividedSizeX = GetMap()->GetMapSizeX() / 2;
            uint32 l_MapDividedSizeY = GetMap()->GetMapSizeY() / 2;

            if (l_PositionX >= l_MapDividedSizeX && l_PositionY >= l_MapDividedSizeY)
                GetSpline()->Move(GetMap()->GetMapSizeX() - 100, GetMap()->GetMapSizeY() - 100);
            else if (l_PositionX <= l_MapDividedSizeX && l_PositionY <= l_MapDividedSizeY)
                GetSpline()->Move(100, 100);
            else if (l_PositionX >= l_MapDividedSizeX && l_PositionY <= l_MapDividedSizeY)
                GetSpline()->Move(GetMap()->GetMapSizeX() - 100, 100);
            else
                GetSpline()->Move(0, GetMap()->GetMapSizeY() - 100);
        }

        m_Fleeing = p_Fleeing;
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone