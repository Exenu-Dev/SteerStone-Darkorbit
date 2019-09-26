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
#include "Player.hpp"
#include "ZoneManager.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Unit::Unit()
    {
        m_Shield         = 0;
        m_MaxShield      = 0;
        m_HitPoints      = 0;
        m_MaxHitPoints   = 0;
        m_GatesAchieved  = 0;
        m_ClanId         = 0;
        m_ClanName.clear();
        m_Company        = Company::NOMAD;
        m_Rank           = 0;
        m_WeaponState    = 0;
        m_DeathState     = DeathState::ALIVE;
        m_LaserType      = 1;
        m_RocketType     = 1;
        m_Attacking      = false;

        m_Target         = nullptr;
        m_TargetGUID     = 0;

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

        Server::Packets::LaserShoot l_Packet;
        l_Packet.FromId     = GetObjectGUID().GetCounter();
        l_Packet.ToId       = GetTarget()->GetObjectGUID().GetCounter();
        l_Packet.LaserId    = m_LaserType;
        GetMap()->SendPacketToMap(l_Packet.Write());

        m_Attacking = true;
    }

    /// Update Attack
    /// @p_Diff : Execution Time
    void Unit::AttackerStateUpdate(uint32 const p_Diff)
    {
        /// If we are not attacking, then don't update
        if (!m_Attacking || !GetTargetGUID())
            return;

    }

    /// Update
    /// @p_Diff : Execution Time
    void Unit::Update(uint32 const p_Diff)
    {
        if (GetSpline()->IsMoving())
            if (GetSpline()->GetLastTimeCalled() > 4000)
                GetSpline()->SetIsMoving(false);

        AttackerStateUpdate(p_Diff);
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone