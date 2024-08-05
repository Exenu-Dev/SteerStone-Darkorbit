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
#include <random>
#include "ClanManager.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Mob::Mob()
    {
        m_Entry         = 0;
        m_ShipId		= 0;

        m_HomePositionX = 0.0f;
        m_HomePositionY = 0.0f;

        m_PlayerTagger = nullptr;

        m_LastTargetPositionX = 0.0f;
        m_LastTargetPositionY = 0.0f;

        m_RandomDistanceFromPlayerX = 0;
        m_RandomDistanceFromPlayerY = 0;

        m_Fleeing = false;

        m_AttackRange = sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MOB_ATTACK_RANGE);

        m_Clan = sClanManager->MobClan();

        SetType(Type::OBJECT_TYPE_MOB);
        SetGUID(ObjectGUID(GUIDType::MOB));
    }
    /// Deconstructor
    Mob::~Mob()
    {
        Unit::RemoveAllAttackers();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update
    /// @p_Diff : Execution Time
    bool Mob::Update(uint32 const p_Diff)
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
                    return false;

                m_DeathState = DeathState::ALIVE;

                sZoneManager->AddToMap(this);
            }
            break;
            default:
                break;
        }
       
        return true;
    }

    /// Reward Credit/Uridium...
    /// @p_Player : Player is being rewarded
    void Mob::RewardKillCredit(Player* p_Player)
    {
        p_Player->RewardCredits(m_Credits);
        p_Player->RewardUridium(m_Uridium);
        p_Player->RewardExperience(m_Experience);
        p_Player->UpdateDroneExperience(this);
        p_Player->RewardHonor(m_Honor);
        p_Player->UpdateLogBook("<div class=\"logdata\">You have destroyed " + GetName() + " alien ship.</div>" +
            "<div class=\"logdata\">You received " + std::to_string(m_Entry) + " Experience Points." + "</div>" +
            "<div class=\"logdata\">You received " + std::to_string(m_Credits) + " Credits." + "</div>" +
            "<div class=\"logdata\">You received " + std::to_string(m_Uridium) + " Uridium." + "</div>", LogBookType::LOG_BOOK_TYPE_OVERVIEW);

        p_Player->UpdateLogBook("You received " + std::to_string(m_Experience) + " EP.");
        p_Player->UpdateLogBook("You received " + std::to_string(m_Honor) + " honor.");
        p_Player->UpdateLogBook("You destroyed alien ship " + GetName());
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update Movement
    /// @p_Diff : Execution Time
    void Mob::UpdateMovement(uint32 const p_Diff)
    {
        ///< If unit is 15% health or below, then move to nearest corner of the map
        if (GetHitPoints() <= Core::Utils::CalculatePercentage(GetHitMaxPoints(), 15))
            SetIsFleeing(true);

        if (IsFleeing())
        {
            GetSpline()->Move();
            return;
        }

        switch (m_Behaviour)
        {
            /// Find closest player, and move to player
            case Behaviour::BEHAVIOUR_PASSIVE:
            {
                Player* l_Target = ScanForPlayers();

                if (l_Target)
                    ChasePlayer(l_Target);
                else
                    HandleRoaming(p_Diff);
            }
            break;
            /// Find closest player, and move to player
            case Behaviour::BEHAVIOUR_AGGESSIVE:
            {
                Player* l_Target = ScanForPlayers();

                if (l_Target)
                    ChasePlayer(l_Target);
                else
                    HandleRoaming(p_Diff);
            }
            break;
        }
    }
    /// Handle Roaming
    /// @p_Diff : Execution Time
    void Mob::HandleRoaming(uint32 const p_Diff)
    {
        /// Movement Update
        m_IntervalMoveTimer.Update(p_Diff);
        if (!m_IntervalMoveTimer.Passed())
            return;

        /// Roam 300 units away from current position
        float l_PositionX = GetSpline()->GetPlannedPositionX() + Core::Utils::Int32Random(-300, 300);
        float l_PositionY = GetSpline()->GetPlannedPositionY() + Core::Utils::Int32Random(-300, 300);

        GetSpline()->Move(l_PositionX, l_PositionY);
    }
    /// Scan for Players
    /// Mob will attempt to find a nearby player
    Entity::Player* Mob::ScanForPlayers()
    {
        Entity::Player* l_Player = nullptr;

        if (HasTarget())
            l_Player = GetTarget()->ToPlayer();
        else
            l_Player = GetGrid()->FindNearestPlayer(this, sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_FIND_PLAYER_DISTANCE));

        if (!l_Player)
        {
            CancelAttack();
            return nullptr;
        }

        /// Cannot target players who are at a portal or station, or cloaked
        if (!l_Player ||
            l_Player->ToPlayer()->GetEvent() == EventType::EVENT_TYPE_PORTAL || l_Player->ToPlayer()->GetEvent() == EventType::EVENT_TYPE_STATION ||
            l_Player->IsCloaked()
        )
        {
            CancelAttack();
            return nullptr;
        }

        /// If player last time shot is more than 10 seconds ago (MAX_LAST_TIME_ATTACKED), then cancel combat
        if (GetTaggedPlayer())
        {
            LOG_ASSERT(GetTarget()->GetGUID() == GetTaggedPlayer()->GetGUID(), "Mob", "Target and tagged player does not match!");

            if (!GetTaggedPlayer()->IsAttacking())
            {
                CancelAttack();
                return nullptr;
            }
        }

        if (!HasTarget())
            SetTarget(l_Player);

        return l_Player;
    }
    /// Chase Player
    /// @p_Player : Player to chase
    /// @p_Attack : Attack Player if within range
    bool Mob::ChasePlayer(Player* p_Player, bool p_Attack/* = false*/)
    {
        if (!p_Player)
			return false;

        float l_PositionX = p_Player->GetSpline()->GetPositionX();
        float l_PositionY = p_Player->GetSpline()->GetPositionY();
        float l_Distance = Core::Utils::DistanceSquared(GetSpline()->GetPositionX(), GetSpline()->GetPositionY(), l_PositionX, l_PositionY);

        if (l_Distance > sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MAX_FOLLOW_DISTANCE))
        {
            CancelAttack();
            return false;
        }

        if (p_Attack && !IsAttacking())
			Attack(p_Player);

        if (l_Distance > 150)
        {
            /// If player is more than our min distance, start following player
            if (l_Distance > sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_MIN_FOLLOW_DISTANCE))
                GetSpline()->Move(p_Player->GetSpline()->GetPositionX(), p_Player->GetSpline()->GetPositionY());
		}
        else
        {

            if (l_Distance < 400)
                return false;

            std::pair<float, float> l_Position = GetSpline()->PositionInCircleRadius(360);

            GetSpline()->Move(l_Position.first, l_Position.second);
        }

        m_LastTargetPositionX = l_PositionX;
        m_LastTargetPositionY = l_PositionY;
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