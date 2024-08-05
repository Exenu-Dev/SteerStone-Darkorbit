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

#include "Server/Opcodes/Packets/Server/MapPackets.hpp"
#include "Server/Opcodes/Packets/Server/ShipPackets.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "Utility/UtilMaths.hpp"
#include "SurroundingObject.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    /// @p_Object : Object
    /// @p_PlayerObject : Player
    SurroundingObject::SurroundingObject(Object* p_Object, Object* p_PlayerObject)
        : m_Object(p_Object), m_PlayerObject(p_PlayerObject)
    {
        m_ScheduleForDespawn = false;
        m_IntervalDelayRemoval.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_DELAY_REMOVAL));
    }
    /// Deconstructor
    SurroundingObject::~SurroundingObject()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update
    /// @p_Diff : Execution Time
    bool SurroundingObject::Update(uint32 const p_Diff)
    {
        LOG_ASSERT(m_Object, "SurroundingObject", "Object is null!");

        /// If unit is dead, remove from client
        if (!m_Object->IsBonusBox() && !m_Object->IsMine() && m_Object->ToUnit()->GetDeathState() == Entity::DeathState::DEAD)
        {
            Despawn();
            return false;
        }
        
        /// Object is scheduled to be despawned from client
        if (IsScheduledForDespawn())
        {
            m_IntervalDelayRemoval.Update(p_Diff);
            if (m_IntervalDelayRemoval.Passed())
            {
                if (Despawn())
                    return false;
                else
                    return true;
            }
        }
        else
        {
            /// Check whether the object is still in our radius view
            if (!Core::Utils::IsInCircleRadius(m_PlayerObject->GetSpline()->GetPositionX(), m_PlayerObject->GetSpline()->GetPositionY(),
                m_Object->GetSpline()->GetPositionX(), m_Object->GetSpline()->GetPositionY(), PLAYER_RADIUS_SCAN))
                ScheduleForDespawn();
        }

        return true;
    }

    /// Schedule to despawn object
    void SurroundingObject::ScheduleForDespawn()
    {
        if (m_ScheduleForDespawn)
            return;

        m_ScheduleForDespawn = true;
    }

    /// Remove object from being despawned
    void SurroundingObject::RemoveScheduleFromDespawn()
    {
        m_ScheduleForDespawn = false;
        m_IntervalDelayRemoval.Reset();
    }

    /// Send Packet
    /// @p_PacketBuffer : Packet being sent
    void SurroundingObject::SendPacket(Server::PacketBuffer const* p_PacketBuffer)
    {
        if (m_Object->IsPlayer())
        {
            if (m_Object->ToPlayer()->IsInSurrounding(m_PlayerObject))
                m_Object->ToPlayer()->SendPacket(p_PacketBuffer);
        }
    }

    /// Whether object is set to be despawned
    bool SurroundingObject::IsScheduledForDespawn() const
    {
        return m_ScheduleForDespawn;
    }

    /// Despawn Object
    /// @p_Force : Force Despawn
    bool SurroundingObject::Despawn(bool p_Force/* = false*/)
    {
        if (m_Object->IsMob() || m_Object->IsPlayer())
        {
            /// If we are still targetting the object, keep it in client
            if ((m_PlayerObject->ToUnit()->GetTarget() && m_PlayerObject->ToUnit()->GetTarget()->GetObjectGUID().GetCounter() ==
                m_Object->GetObjectGUID().GetCounter()) && !p_Force)
            {
                /// If its a mob, then clear the target of the mob
                if (m_Object->GetType() == Type::OBJECT_TYPE_MOB)
                    m_Object->ToUnit()->CancelAttack();

                /// And reset timer
                m_IntervalDelayRemoval.Reset();

                return false;
            }

            Server::Packets::Ship::DespawnShip l_Packet;
            l_Packet.Id = m_Object->GetObjectGUID().GetCounter();
            m_PlayerObject->ToPlayer()->SendPacket(l_Packet.Write());
        }
        else if (m_Object->IsBonusBox()) ///< Is Cargo
        {
            Server::Packets::RemoveCargo l_Packet;
            l_Packet.Id = m_Object->GetObjectGUID().GetCounter();
            m_PlayerObject->ToPlayer()->SendPacket(l_Packet.Write());
        }
        else if (m_Object->IsOre())
        {
			Server::Packets::DespawnOre l_Packet;
			l_Packet.Id = m_Object->GetObjectGUID().GetCounter();
			m_PlayerObject->ToPlayer()->SendPacket(l_Packet.Write());
		}
        else
            LOG_WARNING("SurroundingObject", "Object type not handled for despawn! %0, %1", static_cast<uint8>(m_Object->GetType()), m_Object->GetObjectGUID().GetCounter());

        return true;
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone