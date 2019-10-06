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

#include "Opcodes/Packets/Server/MapPackets.hpp"
#include "Opcodes/Packets/Server/MiscPackets.hpp"
#include "Opcodes/Packets/Server/AttackPackets.hpp"
#include "Player.hpp"
#include "Mob.hpp"
#include "ZoneManager.hpp"
#include "Utility/UtilRandom.hpp"

#include "Utility/UtilMaths.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Unit::Unit()
    {
        m_Shield         = 0;
        m_MaxShield      = 0;
        m_HitPoints      = 0;
        m_MinDamage      = 0;
        m_MaxDamage      = 0;
        m_MaxHitPoints   = 0;
        m_GatesAchieved  = 0;
        m_ClanId         = 0;
        m_ClanName.clear();
        m_Company        = Company::NOMAD;
        m_Rank           = 0;
        m_WeaponState    = 0;
        m_DeathState     = DeathState::ALIVE;
        m_LaserType      = 0;
        m_RocketType     = 0;
        m_Attacking      = false;
        m_AttackRange    = 0;
        m_AttackState    = AttackState::ATTACK_STATE_NONE;
        m_SelectedLaser  = 1;
        m_SelectedRocket = 1;

        m_Target         = nullptr;
        m_TargetGUID     = 0;

        m_IntervalAttackUpdate.SetInterval(ATTACK_UPDATE_INTERVAL);

        SetType(Type::OBJECT_TYPE_NPC);
        SetGUID(ObjectGUID(GUIDType::NPC));
    }
    /// Deconstructor
    Unit::~Unit()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////
    //                GENERAL
    ///////////////////////////////////////////

    /// Attack
    /// @p_Victim : Victim we are attacking
    void Unit::Attack(Object* p_Victim)
    {
        /// Set target if not set
        if (GetTargetGUID() != p_Victim->GetGUID())
            SetTarget(p_Victim);

        m_AttackState = AttackState::ATTACK_STATE_IN_RANGE;

        /// Send Attack
        Server::Packets::Attack::LaserShoot l_Packet;
        l_Packet.FromId     = GetObjectGUID().GetCounter();
        l_Packet.ToId       = GetTarget()->GetObjectGUID().GetCounter();
        l_Packet.LaserId    = m_LaserType;
        GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), this, true);

        /// Grey Opponent
        if (!GetTarget()->ToUnit()->IsAttacking())
            GetGrid()->SendPacketIfInSurrounding(Server::Packets::Misc::Info().Write(Server::Packets::Misc::InfoType::INFO_TYPE_GREY_OPPONENT, { GetTarget()->GetObjectGUID().GetCounter(), GetObjectGUID().GetCounter() }), this, false);

        m_Attacking = true;
    }

    /// Update Attack
    /// @p_Diff : Execution Time
    void Unit::AttackerStateUpdate(uint32 const p_Diff)
    {
        /// If we are not attacking, then don't update
        if (!m_Attacking || !GetTargetGUID())
            return;

        m_IntervalAttackUpdate.Update(p_Diff);
        if (!m_IntervalAttackUpdate.Passed())
            return;

        /// Check if we are in range
        if (m_AttackState == AttackState::ATTACK_STATE_IN_RANGE)
        {
            /// Check if we are still in range
            if (Core::Utils::DistanceSquared(GetSpline()->GetPositionX(), GetSpline()->GetPositionY(),
                GetTarget()->GetSpline()->GetPositionX(), GetTarget()->GetSpline()->GetPositionY())
            > m_AttackRange)
            {
                if (GetType() == Type::OBJECT_TYPE_PLAYER)
                {
                    Server::Packets::Attack::AttackOutOfRange l_Packet;
                    ToPlayer()->SendPacket(l_Packet.Write());
                }

                if (GetTarget()->GetType() == Type::OBJECT_TYPE_PLAYER)
                {
                    Server::Packets::Attack::EscapedTheAttack l_Packet;
                    GetTarget()->ToPlayer()->SendPacket(l_Packet.Write());
                }

                /// The client automatically cancels the shooting lasers
                /// but the other client doesn't - weird.
                Server::Packets::Attack::CancelLaserShoot l_Packet;
                l_Packet.FromId = GetObjectGUID().GetCounter();
                l_Packet.ToId   = GetTarget()->GetObjectGUID().GetCounter();
                GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), this);

                m_AttackState = AttackState::ATTACK_STATE_OUT_OF_RANGE;
            }
            else
            {
                /// If we are attacking a mob, then assign mob to attack us
                if (GetTarget()->GetType() == Type::OBJECT_TYPE_NPC)
                    if (!GetTarget()->ToMob()->IsAttacking())
                        GetTarget()->ToUnit()->Attack(this);

                /// Can we hit target?
                if (CalculateHitChance())
                {
                    /// Now calculate damage
                    int32 l_Damage = CalculateDamageDone();
                    int32 l_ShieldDamage = 0;
                    CalculateDamageTaken(l_Damage, l_ShieldDamage);

                    int32 l_HitPoints = GetTarget()->ToUnit()->GetHitPoints() - l_Damage;
                    int32 l_Shield = GetTarget()->ToUnit()->GetShield() - l_ShieldDamage;

                    if (l_HitPoints <= 0)
                    {
                        Kill(GetTarget()->ToUnit());
                        return;
                    }

                    if (l_Shield < 0)
                        l_Shield = 0;

                    GetTarget()->ToUnit()->SetHitPoints(l_HitPoints);
                    GetTarget()->ToUnit()->SetShield(l_Shield);

                    if (GetType() == Type::OBJECT_TYPE_PLAYER)
                    {
                        Server::Packets::Attack::MakeDamage l_MakeDamage;
                        l_MakeDamage.UpdateAmmo = false;
                        l_MakeDamage.HitPoints  = GetTarget()->ToUnit()->GetHitPoints();
                        l_MakeDamage.Shield     = GetTarget()->ToUnit()->GetShield();
                        l_MakeDamage.Damage     = l_ShieldDamage + l_Damage; ///< Total Damage
                        ToPlayer()->SendPacket(l_MakeDamage.Write());
                    }

                    if (GetTarget()->GetType() == Entity::Type::OBJECT_TYPE_PLAYER)
                    {
                        Server::Packets::Attack::RecievedDamage l_ReceivedDamagePacket;
                        l_ReceivedDamagePacket.HitPoints = GetTarget()->ToUnit()->GetHitPoints();
                        l_ReceivedDamagePacket.Shield    = GetTarget()->ToUnit()->GetShield();
                        l_ReceivedDamagePacket.Damage    = l_ShieldDamage + l_Damage; ///< Total Damage
                        GetTarget()->ToPlayer()->SendPacket(l_ReceivedDamagePacket.Write());
                    }       
                }
                else ///< Send Miss Packet
                {
                    if (GetType() == Type::OBJECT_TYPE_PLAYER)
                    {
                        Server::Packets::Attack::MissSelf l_MissPacket;
                        ToPlayer()->SendPacket(l_MissPacket.Write());
                    }

                    if (GetTarget()->GetType() == Type::OBJECT_TYPE_PLAYER)
                    {
                        Server::Packets::Attack::MissTarget l_MissPacket;
                        GetTarget()->ToPlayer()->SendPacket(l_MissPacket.Write());
                    }
                }
            }
        }
        else if (m_AttackState == AttackState::ATTACK_STATE_OUT_OF_RANGE)
        {
            if (Core::Utils::DistanceSquared(GetSpline()->GetPositionX(), GetSpline()->GetPositionY(),
                GetTarget()->GetSpline()->GetPositionX(), GetTarget()->GetSpline()->GetPositionY())
            <= m_AttackRange)
            {
                if (GetType() == Type::OBJECT_TYPE_PLAYER)
                {
                    Server::Packets::Attack::AttackInRange l_Packet;
                    ToPlayer()->SendPacket(l_Packet.Write());
                }

                /// If it's a mob, then reassign mob to attack again
                if (GetTarget()->GetType() == Type::OBJECT_TYPE_NPC)
                    if (!GetTarget()->ToMob()->IsAttacking())
                        GetTarget()->ToUnit()->Attack(this);

                /// Send Attack
                Server::Packets::Attack::LaserShoot l_Packet;
                l_Packet.FromId     = GetObjectGUID().GetCounter();
                l_Packet.ToId       = GetTarget()->GetObjectGUID().GetCounter();
                l_Packet.LaserId    = m_LaserType;
                GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), this, true);

                m_AttackState = AttackState::ATTACK_STATE_IN_RANGE;
            }
        }
    }

    void Unit::CancelAttack()
    {
        if (!m_Attacking || m_AttackState == AttackState::ATTACK_STATE_NONE)
            return;

        LOG_ASSERT(GetTarget(), "Unit", "Attempted to cancel attack but target does not exist!");

        Server::Packets::Attack::CancelLaserShoot l_Packet;
        l_Packet.FromId = GetObjectGUID().GetCounter();
        l_Packet.ToId   = GetTarget()->GetObjectGUID().GetCounter();
        GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), this, true);

        /// Clear Gray Target
        if (GetType() == Type::OBJECT_TYPE_NPC)
        {
            GetGrid()->SendPacketIfInSurrounding(Server::Packets::Misc::Info().Write(Server::Packets::Misc::InfoType::INFO_TYPE_UNGREY_OPPONENT, { GetObjectGUID().GetCounter() }), this, false);

            /// The target may be on other side of the map if still targetting, so send packet specifically to target
            if (GetTarget()->ToPlayer())
                GetTarget()->ToPlayer()->SendPacket(Server::Packets::Misc::Info().Write(Server::Packets::Misc::InfoType::INFO_TYPE_UNGREY_OPPONENT, { GetObjectGUID().GetCounter() }));
        }

        m_Attacking     = false;
        m_AttackState   = AttackState::ATTACK_STATE_NONE;
        ClearTarget();
    }

    /// Calculate Hit chance whether we can hit target
    bool Unit::CalculateHitChance()
    {
        /// 80% chance we will hit target
        return Core::Utils::RollChanceInterger32(80);
    }

    /// Calculate Damage done for target
    uint32 Unit::CalculateDamageDone()
    {
        uint32 l_MinDamage = 0;
        uint32 l_MaxDamage = 0;
        switch (m_SelectedLaser)
        {
            case BatteryType::BATTERY_TYPE_LCB10:
            case BatteryType::BATTERY_TYPE_MCB25:
            case BatteryType::BATTERY_TYPE_MCB50:
            case BatteryType::BATTERY_TYPE_UCB100:
                l_MinDamage = m_MinDamage * m_SelectedLaser;
                l_MaxDamage = m_MaxDamage * m_SelectedLaser;
                break;
            default:
                break;
        }

        return Core::Utils::UInt32Random(l_MinDamage, l_MaxDamage);
    }
    /// Calculate Damage takem for target
    void Unit::CalculateDamageTaken(int32& p_Damage, int32& p_ShieldDamage)
    {
        if (GetTarget()->ToUnit()->GetShield() != 0)
            p_ShieldDamage = Core::Utils::CalculatePercentage(p_Damage, GetTarget()->ToUnit()->GetShieldResistance());

        p_Damage = p_Damage - p_ShieldDamage;
    }

    /// Update
    /// @p_Diff : Execution Time
    void Unit::Update(uint32 const p_Diff)
    {
        if (GetSpline()->IsMoving())
            if (GetSpline()->GetLastTimeCalled() > 2000)
                GetSpline()->SetIsMoving(false);

        AttackerStateUpdate(p_Diff);
    }

    /// Kill
    /// @p_Unit : Unit being killed
    void Unit::Kill(Unit* p_Unit)
    {
        Server::Packets::Attack::Kill l_Packet;
        l_Packet.Id = p_Unit->GetObjectGUID().GetCounter();
        GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), p_Unit, true);

        Server::Packets::Attack::CancelLaserShoot l_CancelLaserPacket;
        l_CancelLaserPacket.FromId  = GetObjectGUID().GetCounter();
        l_CancelLaserPacket.ToId    = GetTarget()->GetObjectGUID().GetCounter();
        GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), this);

        if (p_Unit->GetType() == Type::OBJECT_TYPE_NPC)
            p_Unit->ToMob()->RewardKillCredit(ToPlayer());

        p_Unit->m_DeathState = DeathState::JUST_DIED;
        CancelAttack();
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone
