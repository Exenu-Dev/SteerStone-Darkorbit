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

#include "Opcodes/Packets/Server/MapPackets.hpp"
#include "Opcodes/Packets/Server/MiscPackets.hpp"
#include "Opcodes/Packets/Server/AttackPackets.hpp"
#include "Diagnostic/DiaServerWatch.hpp"
#include "Player.hpp"
#include "Mob.hpp"
#include "ZoneManager.hpp"
#include "Utility/UtilRandom.hpp"

#include "Utility/UtilMaths.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Unit::Unit()
    {
        m_Shield            = 0;
        m_MaxShield         = 0;
        m_HitPoints         = 0;
        m_MinDamage         = 0;
        m_MaxDamage         = 0;
        m_MaxHitPoints      = 0;
        m_GatesAchieved     = 0;
        m_ClanId            = 0;
        m_ClanName.clear();
        m_Company           = Company::NOMAD;
        m_Rank              = 0;
        m_WeaponState       = 0;
        m_DeathState        = DeathState::ALIVE;
        m_LaserType         = 1;
        m_RocketType        = 0;
        m_Attacking         = false;
        m_AttackRange       = 0;
        m_AttackState       = AttackState::ATTACK_STATE_NONE;
        m_SelectedLaser     = 0;
        m_SelectedRocket    = 0;
        m_LastTimeAttacked  = 0;
        m_Experience        = 0;
        m_Behaviour         = Behaviour::BEHAVIOUR_PASSIVE;
        m_Honor             = 0;
        m_RespawnTimer      = 0;
        m_Credits           = 0;
        m_Uridium           = 0;
        m_InRadiationZone   = false;
        m_AttackType        = AttackType::ATTACK_TYPE_NONE;
        
        for (uint32 l_I = 0; l_I < MAX_RESOURCE_COUNTER; l_I++)
            m_Resources[l_I] = 0;
   
        m_Target            = nullptr;
        m_TargetGUID        = 0;

        m_IntervalAttackUpdate.SetInterval(ATTACK_UPDATE_INTERVAL);
        m_IntervalRocketAttackUpdate.SetInterval(ROCKET_ATTACK_UPDATE);
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

    /// Update
    /// @p_Diff : Execution Time
    void Unit::Update(uint32 const p_Diff)
    {
        if (GetSpline()->IsMoving())
            if (GetSpline()->GetLastTimeCalled() > 2000)
                GetSpline()->SetIsMoving(false);

        AttackerStateUpdate(p_Diff);
    }

    ///////////////////////////////////////////
    //             ATTACK SYSTEM
    ///////////////////////////////////////////

    /// Attack
    /// @p_Victim : Victim we are attacking
    void Unit::Attack(Unit* p_Victim, AttackType const p_AttackType /*= AttackType::ATTACK_TYPE_LASER*/)
    {
        if (!CanAttackTarget(p_Victim))
            return;

        /// Set target if not set
        if (GetTargetGUID() != p_Victim->GetGUID())
            SetTarget(p_Victim);

        if ((p_AttackType & (AttackType::ATTACK_TYPE_LASER | AttackType::ATTACK_TYPE_BOTH)))
        {
            /// Send Attack
            Server::Packets::Attack::LaserShoot l_Packet;
            l_Packet.FromId = GetObjectGUID().GetCounter();
            l_Packet.ToId = GetTarget()->GetObjectGUID().GetCounter();
            l_Packet.LaserId = m_WeaponState >= WeaponState::WEAPON_STATE_FULLY_EQUIPPED ? GetLaserColourId() : WeaponState::WEAPON_STATE_NOT_EQUIPPED;
            GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), this, true);
        }

        /// If target is mob, then assign mob to attack us if mob is not already tagged
        if (GetTarget()->IsMob())
        {
            if (!GetTarget()->ToMob()->IsTaggedByPlayer())
            {
                GetTarget()->ToMob()->SetTaggedPlayer(ToPlayer());
                GetTarget()->ToMob()->Attack(this);

                GetGrid()->SendPacketIfInSurrounding(Server::Packets::Misc::Info().Write(Server::Packets::Misc::InfoType::INFO_TYPE_GREY_OPPONENT, { GetTarget()->GetObjectGUID().GetCounter(), GetObjectGUID().GetCounter() }), this, false);
            }
        }

        m_LastTimeAttacked  = sServerTimeManager->GetServerTime();
        m_Attacking         = true;
        m_AttackType        |= p_AttackType;
        m_AttackState       = AttackState::ATTACK_STATE_IN_RANGE;
    }
    /// Update Rocket Attack
    /// @p_Diff : Diff counter
    void Unit::UpdateRocketAttack(uint32 const p_Diff)
    {
        if (!(m_AttackType & (AttackType::ATTACK_TYPE_ROCKET | AttackType::ATTACK_TYPE_BOTH)))
            return;

        if (!m_IntervalRocketAttackUpdate.Passed())
            return;

        if (IsPlayer())
            ToPlayer()->SetRocketAmmo(static_cast<RocketType>(m_RocketType), -1);

        if (CalculateHitChance())
        {
            /// Now calculate damage
            int32 l_Damage = CalculateDamageDone(AttackType::ATTACK_TYPE_ROCKET);
            int32 l_ShieldDamage = 0;
            CalculateDamageTaken(GetTarget(), l_Damage, l_ShieldDamage);

            int32 l_HitPoints = GetTarget()->ToUnit()->GetHitPoints() - l_Damage;
            int32 l_Shield = GetTarget()->ToUnit()->GetShield() - l_ShieldDamage;

            /// If Hitpoints is 0 or less, then target is dead
            if (l_HitPoints <= 0)
            {
                Kill(GetTarget()->ToUnit());
                return;
            }

            if (l_Shield < 0)
                l_Shield = 0;

            /// Set new hitpoints and shield points
            GetTarget()->ToUnit()->SetHitPoints(l_HitPoints);
            GetTarget()->ToUnit()->SetShield(l_Shield);

            /// Send damage effect to attacker
            if (IsPlayer())
            {
                Server::Packets::Attack::MakeDamage l_MakeDamage;
                l_MakeDamage.UpdateAmmo = false;
                l_MakeDamage.HitPoints = GetTarget()->ToUnit()->GetHitPoints();
                l_MakeDamage.Shield = GetTarget()->ToUnit()->GetShield();
                l_MakeDamage.Damage = l_ShieldDamage + l_Damage; ///< Total Damage
                ToPlayer()->SendPacket(l_MakeDamage.Write());

                Server::Packets::Attack::TargetHealth l_TargetHealthPacket;
                l_TargetHealthPacket.Shield = GetTarget()->GetShield();
                l_TargetHealthPacket.MaxShield = GetTarget()->GetMaxShield();
                l_TargetHealthPacket.HitPoints = GetTarget()->GetHitPoints();
                l_TargetHealthPacket.MaxHitPoints = GetTarget()->GetHitMaxPoints();
                ToPlayer()->SendPacket(l_TargetHealthPacket.Write());
            }

            /// Send recieved damage effect to target
            if (GetTarget()->IsPlayer())
            {
                Server::Packets::Attack::RecievedDamage l_ReceivedDamagePacket;
                l_ReceivedDamagePacket.HitPoints = GetTarget()->ToUnit()->GetHitPoints();
                l_ReceivedDamagePacket.Shield = GetTarget()->ToUnit()->GetShield();
                l_ReceivedDamagePacket.Damage = l_ShieldDamage + l_Damage; ///< Total Damage
                GetTarget()->ToPlayer()->SendPacket(l_ReceivedDamagePacket.Write());
            }
            else if (GetTarget()->IsMob())
            {
                if (GetTarget()->GetHitPoints() <= Core::Utils::CalculatePercentage(GetTarget()->GetHitMaxPoints(), 15) && !GetTarget()->ToMob()->IsFleeing())
                    GetTarget()->ToMob()->SetIsFleeing(true);
            }

            SendRocketAttack(true);
        }
        else
            SendRocketAttack(false);

        if (IsPlayer())
            ToPlayer()->SendClearRocketCooldown();

        CancelAttack(AttackType::ATTACK_TYPE_ROCKET);
    }
    /// Update Laser Attack
    void Unit::UpdateLaserAttack()
    {
        if (!(m_AttackType & (AttackType::ATTACK_TYPE_LASER | AttackType::ATTACK_TYPE_BOTH)))
            return;

        // If is player, we need to decrease the ammo by weapon count
        if (IsPlayer())
        {
            // Check if player still has any ammo left
            if (ToPlayer()->GetSelectedBatteryAmmo() <= 0)
            {
                ToPlayer()->CancelAttack(AttackType::ATTACK_TYPE_LASER);
                ToPlayer()->SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_MESSAGE, { "No more ammo." }));
                return;
            }

            uint32 l_WeaponCount = ToPlayer()->GetInventory()->GetWeaponCount();

            switch (ToPlayer()->GetLaserType())
            {
            case BatteryType::BATTERY_TYPE_LCB10:
                ToPlayer()->SetBatteryAmmo(BatteryType::BATTERY_TYPE_LCB10, -l_WeaponCount);
                break;
            case BatteryType::BATTERY_TYPE_MCB25:
                ToPlayer()->SetBatteryAmmo(BatteryType::BATTERY_TYPE_MCB25, -l_WeaponCount);
                break;
            case BatteryType::BATTERY_TYPE_MCB50:
                ToPlayer()->SetBatteryAmmo(BatteryType::BATTERY_TYPE_MCB50, -l_WeaponCount);
                break;
            case BatteryType::BATTERY_TYPE_SAB50:
                ToPlayer()->SetBatteryAmmo(BatteryType::BATTERY_TYPE_SAB50, -l_WeaponCount);
                break;
            case BatteryType::BATTERY_TYPE_UCB100:
                ToPlayer()->SetBatteryAmmo(BatteryType::BATTERY_TYPE_UCB100, -l_WeaponCount);
                break;
            default:
                LOG_ASSERT(false, "Unit", "Cannot update player ammo, type doesn't exist %0", ToPlayer()->GetLaserType());
                break;
            }
        }

        /// Can we hit target?
        if (CalculateHitChance())
        {
            /// Now calculate damage
            int32 l_Damage = CalculateDamageDone(AttackType::ATTACK_TYPE_LASER);
            int32 l_ShieldDamage = 0;
            CalculateDamageTaken(GetTarget(), l_Damage, l_ShieldDamage);

            int32 l_HitPoints = GetTarget()->ToUnit()->GetHitPoints() - l_Damage;
            int32 l_Shield = GetTarget()->ToUnit()->GetShield() - l_ShieldDamage;

            /// If Hitpoints is 0 or less, then target is dead
            if (l_HitPoints <= 0)
            {
                Kill(GetTarget()->ToUnit());
                return;
            }

            if (l_Shield < 0)
                l_Shield = 0;

            /// Set new hitpoints and shield points
            GetTarget()->ToUnit()->SetHitPoints(l_HitPoints);
            GetTarget()->ToUnit()->SetShield(l_Shield);

            /// Send damage effect to attacker
            if (IsPlayer())
            {
                Server::Packets::Attack::MakeDamage l_MakeDamage;
                l_MakeDamage.UpdateAmmo = false;
                l_MakeDamage.HitPoints = GetTarget()->ToUnit()->GetHitPoints();
                l_MakeDamage.Shield = GetTarget()->ToUnit()->GetShield();
                l_MakeDamage.Damage = l_ShieldDamage + l_Damage; ///< Total Damage
                ToPlayer()->SendPacket(l_MakeDamage.Write());

                Server::Packets::Attack::TargetHealth l_TargetHealthPacket;
                l_TargetHealthPacket.Shield = GetTarget()->GetShield();
                l_TargetHealthPacket.MaxShield = GetTarget()->GetMaxShield();
                l_TargetHealthPacket.HitPoints = GetTarget()->GetHitPoints();
                l_TargetHealthPacket.MaxHitPoints = GetTarget()->GetHitMaxPoints();
                ToPlayer()->SendPacket(l_TargetHealthPacket.Write());
            }

            /// Send recieved damage effect to target
            if (GetTarget()->IsPlayer())
            {
                Server::Packets::Attack::RecievedDamage l_ReceivedDamagePacket;
                l_ReceivedDamagePacket.HitPoints = GetTarget()->ToUnit()->GetHitPoints();
                l_ReceivedDamagePacket.Shield = GetTarget()->ToUnit()->GetShield();
                l_ReceivedDamagePacket.Damage = l_ShieldDamage + l_Damage; ///< Total Damage
                GetTarget()->ToPlayer()->SendPacket(l_ReceivedDamagePacket.Write());
            }
            else if (GetTarget()->IsMob())
            {
                if (GetTarget()->GetHitPoints() <= Core::Utils::CalculatePercentage(GetTarget()->GetHitMaxPoints(), 15) && !GetTarget()->ToMob()->IsFleeing())
                    GetTarget()->ToMob()->SetIsFleeing(true);
            }
        }
        else ///< Send Miss Packet
        {
            if (IsPlayer())
                ToPlayer()->SendPacket(Server::Packets::Attack::MissSelf().Write());

            if (GetTarget()->IsPlayer())
                GetTarget()->ToPlayer()->SendPacket(Server::Packets::Attack::MissTarget().Write());
        }
    }
    /// Can Attack Target
    /// @p_Victim : Victim
    bool Unit::CanAttackTarget(Unit* p_Victim)
    {
        /// Cannot attack player if player is in a company map and is near a portal or station
        if (p_Victim->IsPlayer() && p_Victim->GetMap()->IsInCompanyMap(p_Victim) && IsPlayer())
        {
            if ((p_Victim->ToPlayer()->GetEvent() == EventType::EVENT_TYPE_PORTAL || p_Victim->ToPlayer()->GetEvent() == EventType::EVENT_TYPE_STATION)
                && !p_Victim->IsAttacking())
            {
                ToPlayer()->SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_MESSAGE, { "Target is in Demolition Zone!" }));
                return false;
            }
        }

        /// Can't attack target if target is dead
        if (p_Victim->GetDeathState() == DeathState::DEAD)
        {
            CancelAttack();
            return false;
        }

        return true;
    }
    /// Is In Combat
    bool Entity::Unit::IsInCombat()
    {
        if (sServerTimeManager->GetTimeDifference(m_LastTimeAttacked, sServerTimeManager->GetServerTime()) > MAX_LAST_TIME_ATTACKED)
            return false;

        return true;
    }
    /// Update Attack
    /// @p_Diff : Execution Time
    void Unit::AttackerStateUpdate(uint32 const p_Diff)
    {
        m_IntervalRocketAttackUpdate.Update(p_Diff);

        m_IntervalAttackUpdate.Update(p_Diff);
        if (!m_IntervalAttackUpdate.Passed())
            return;

        /// If we are not attacking, then don't update
        if (!m_Attacking || !GetTargetGUID())
            return;

        /// Cancel attack if target is dead
        if (GetTarget()->ToUnit()->GetDeathState() == DeathState::DEAD)
        {
            CancelAttack();
            return;
        }

        /// Check if we are in range
        if (m_AttackState == AttackState::ATTACK_STATE_IN_RANGE)
        {
            /// Check if we are still in range
            if (Core::Utils::DistanceSquared(GetSpline()->GetPositionX(), GetSpline()->GetPositionY(),
                GetTarget()->GetSpline()->GetPositionX(), GetTarget()->GetSpline()->GetPositionY())
            > m_AttackRange)
            {
                if (IsPlayer())
                    ToPlayer()->SendPacket(Server::Packets::Attack::AttackOutOfRange().Write());

                if (GetTarget()->IsPlayer())
                    GetTarget()->ToPlayer()->SendPacket(Server::Packets::Attack::EscapedTheAttack().Write());

                if (IsAttackingWithLaser())
                {
                    /// Cancel laser shoot effect
                    Server::Packets::Attack::CancelLaserShoot l_Packet;
                    l_Packet.FromId = GetObjectGUID().GetCounter();
                    l_Packet.ToId = GetTarget()->GetObjectGUID().GetCounter();
                    GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), this);
                }

                m_AttackState = AttackState::ATTACK_STATE_OUT_OF_RANGE;
            }
            else
            {
                /// Update last time attacked
                m_LastTimeAttacked = sServerTimeManager->GetServerTime();

                UpdateLaserAttack();
                UpdateRocketAttack(p_Diff);
            }
        }
        else if (m_AttackState == AttackState::ATTACK_STATE_OUT_OF_RANGE)
        {
            if (Core::Utils::DistanceSquared(GetSpline()->GetPositionX(), GetSpline()->GetPositionY(),
                GetTarget()->GetSpline()->GetPositionX(), GetTarget()->GetSpline()->GetPositionY())
            <= m_AttackRange)
            {
                if (IsPlayer())
                    ToPlayer()->SendPacket(Server::Packets::Attack::AttackInRange().Write());

                if (IsAttackingWithLaser())
                {
                    /// Send Attack
                    Server::Packets::Attack::LaserShoot l_Packet;
                    l_Packet.FromId = GetObjectGUID().GetCounter();
                    l_Packet.ToId = GetTarget()->GetObjectGUID().GetCounter();
                    l_Packet.LaserId = GetLaserColourId();
                    GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), this, true);
                }

                m_AttackState = AttackState::ATTACK_STATE_IN_RANGE;
            }
        }
    }
    /// Cancel Attack
    /// @p_AttackType : Type of attack
    void Unit::CancelAttack(AttackType const p_AttackType)
    {
        if (!m_Attacking || m_AttackState == AttackState::ATTACK_STATE_NONE)
            return;

        LOG_ASSERT(GetTarget(), "Unit", "Attempted to cancel attack but target does not exist!");

        if (p_AttackType & (AttackType::ATTACK_TYPE_LASER | AttackType::ATTACK_TYPE_BOTH))
        {
            Server::Packets::Attack::CancelLaserShoot l_Packet;
            l_Packet.FromId = GetObjectGUID().GetCounter();
            l_Packet.ToId = GetTarget()->GetObjectGUID().GetCounter();
            GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), this, true);
        }

        if (IsMob())
        {
            GetGrid()->SendPacketIfInSurrounding(Server::Packets::Misc::Info().Write(Server::Packets::Misc::InfoType::INFO_TYPE_UNGREY_OPPONENT, { GetObjectGUID().GetCounter() }), this, false);

            /// The target may be on other side of the map if still targetting, so send packet specifically to target
            if (GetTarget()->ToPlayer())
                GetTarget()->ToPlayer()->SendPacket(Server::Packets::Misc::Info().Write(Server::Packets::Misc::InfoType::INFO_TYPE_UNGREY_OPPONENT, { GetObjectGUID().GetCounter() }));
        
            ToMob()->SetTaggedPlayer(nullptr);
        }

        if (p_AttackType == AttackType::ATTACK_TYPE_BOTH)
            m_AttackType = AttackType::ATTACK_TYPE_NONE;
        else
            m_AttackType &= ~p_AttackType;

        if (!m_AttackType)
        {
            m_Attacking     = false;
            m_AttackState = AttackState::ATTACK_STATE_NONE;
            ClearTarget();
        }
    }
    /// Calculate Hit chance whether we can hit target
    bool Unit::CalculateHitChance()
    {
        /// 80% chance we will hit target
        return Core::Utils::RollChanceInterger32(80);
    }
    /// Calculate Damage done for target
    /// @p_AttackType : Attack Type
    uint32 Unit::CalculateDamageDone(AttackType const p_AttackType)
    {   
        uint32 l_MinDamage = 0;
        uint32 l_MaxDamage = 0;

        if (p_AttackType & (AttackType::ATTACK_TYPE_LASER))
        {
            switch (m_LaserType)
            {
            case BatteryType::BATTERY_TYPE_LCB10:
            case BatteryType::BATTERY_TYPE_MCB25:
            case BatteryType::BATTERY_TYPE_MCB50:
            case BatteryType::BATTERY_TYPE_UCB100:
                l_MinDamage = m_MinDamage * m_LaserType;
                l_MaxDamage = m_MaxDamage * m_LaserType;
                break;
            default:
                break;
            }
        }
        else if (p_AttackType & (AttackType::ATTACK_TYPE_ROCKET))
        {
            switch (GetRocketId())
            {
                case RocketType::ROCKET_TYPE_R310:
                    l_MaxDamage = RocketDamage::ROCKET_DAMAGE_R310;
                    break;
                case RocketType::ROCKET_TYPE_PLT_2026:
                    l_MaxDamage = RocketDamage::ROCKET_DAMAGE_PLT_2026;
                    break;
                case RocketType::ROCKET_TYPE_PLT_2021:
                    l_MaxDamage = RocketDamage::ROCKET_DAMAGE_PLT_2021;
                    break;
                default:
                    LOG_ASSERT(false, "Unit", "Unknown Rocket Type %0", GetRocketId());
            }
        }
        else
            LOG_WARNING("Unit", "Unknown attack type %0", static_cast<uint16>(p_AttackType));

        return Core::Utils::UInt32Random(l_MinDamage, l_MaxDamage);
    }
    /// Deal Damage to target
    /// @p_Target : Target
    /// @p_Damage : Damage
    /// @p_CleanDamage : Deal damage neglecting shield
    void Unit::DealDamage(Unit* p_Target, int32 p_Damage, bool p_CleanDamage)
    {
        int32 l_ShieldDamage = 0;

        if (!p_CleanDamage)
            CalculateDamageTaken(p_Target, p_Damage, l_ShieldDamage);

        int32 l_HitPoints = p_Target->GetHitPoints() - p_Damage;
        int32 l_Shield = p_Target->GetShield() - l_ShieldDamage;

        /// If Hitpoints is 0 or less, then target is dead
        if (l_HitPoints <= 0)
        {
            Kill(p_Target);
            return;
        }

        if (l_Shield < 0)
            l_Shield = 0;

        /// Set new hitpoints and shield points
        p_Target->SetHitPoints(l_HitPoints);
        p_Target->SetShield(l_Shield);

        /// Send recieved damage effect to target
        if (p_Target->IsPlayer())
        {
            Server::Packets::Attack::RecievedDamage l_ReceivedDamagePacket;
            l_ReceivedDamagePacket.HitPoints = p_Target->GetHitPoints();
            l_ReceivedDamagePacket.Shield    = p_Target->GetShield();
            l_ReceivedDamagePacket.Damage    = l_ShieldDamage + p_Damage; ///< Total Damage
            p_Target->ToPlayer()->SendPacket(l_ReceivedDamagePacket.Write());
        }
    }
    /// Calculate Damage takem for target
    /// @p_Target : Target
    /// @p_Damage : Damage taken
    /// @p_ShieldDamage : Shield Damage taken
    void Unit::CalculateDamageTaken(Unit* p_Target, int32& p_Damage, int32& p_ShieldDamage)
    {
        if (p_Target->GetShield() != 0)
            p_ShieldDamage = Core::Utils::CalculatePercentage(p_Damage, p_Target->GetShieldResistance());

        p_Damage = p_Damage - p_ShieldDamage;
    }
    /// Returns whether unit is attacking us
    /// @p_Unit : Unit attacking us
    bool SteerStone::Game::Entity::Unit::IsAttackingMe(Unit* p_Unit) const
    {
        if (p_Unit->GetTarget())
            if (GetObjectGUID().GetCounter() == p_Unit->GetTarget()->GetObjectGUID().GetCounter())
                return true;

        return false;
    }
    /// Kill
    /// @p_Unit : Unit being killed
    void Unit::Kill(Unit* p_Unit)
    {
        Server::Packets::Attack::Kill l_Packet;
        l_Packet.Id = p_Unit->GetObjectGUID().GetCounter();
        GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), p_Unit, true);

        if (p_Unit->GetType() == Type::OBJECT_TYPE_MOB)
            p_Unit->ToMob()->RewardKillCredit(ToPlayer());

        /// Update position, the reason we do this is because if the unit is a mob, it does not send periodic movement updates,
        /// so update the current position so cargo box correctly spawns at the position where unit dies
        p_Unit->GetSpline()->UpdatePosition();

        p_Unit->GetMap()->GetPoolManager()->AddBonuxBox(p_Unit, BonusBoxType::BONUS_BOX_TYPE_CARGO, this);

        p_Unit->m_DeathState = DeathState::JUST_DIED;
        CancelAttack();
    }
    /// Send Rocket Attack
    /// @p_Hit : Whether the attack is a hit or not
    void Unit::SendRocketAttack(bool const p_Hit)
    {
        /// If we are not attacking, then don't update
        if (!m_Attacking || !GetTargetGUID())
            return;

        /// Cancel attack if target is dead
        if (GetTarget()->ToUnit()->GetDeathState() == DeathState::DEAD)
        {
            CancelAttack();
            return;
        }

        Server::Packets::Attack::RocketShoot l_Packet;
        l_Packet.FromId = GetObjectGUID().GetCounter();
        l_Packet.ToId = GetTarget()->GetObjectGUID().GetCounter();
        l_Packet.RocketId = m_RocketType;
        l_Packet.Hit = p_Hit;
        GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), this, true);
    }

    ///////////////////////////////////////////
    //            GETTERS/SETTERS
    ///////////////////////////////////////////

    void SteerStone::Game::Entity::Unit::SetResource(uint32 const p_Index, uint32 const p_Resource)
    {
        if (p_Index > MAX_RESOURCE_COUNTER)
            LOG_ASSERT(false, "Player", "Attempted to add resource but index is unknown! Index: %0", p_Index);

        if (p_Resource == 0)
            m_Resources[p_Index] = 0;
        else
            m_Resources[p_Index] += p_Resource;
        
        if (IsPlayer())
            ToPlayer()->SetCargoSpace(ToPlayer()->GetCargoSpace() + p_Resource);
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone
