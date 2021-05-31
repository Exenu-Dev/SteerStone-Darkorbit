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

#pragma once
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"
#include "Unit.hpp"

#include "Diagnostic/DiaIntervalTimer.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    class SurroundingObject
    {
        DISALLOW_COPY_AND_ASSIGN(SurroundingObject);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        /// @p_Object       : Object
        /// @p_PlayerObject : Player
        SurroundingObject(Object* p_Object, Object* p_PlayerObject);
        /// Deconstructor
        ~SurroundingObject();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    public:
        /// Update
        /// @p_Diff : Execution Time
        bool Update(uint32 const p_Diff);

        /// Schedule to despawn object
        void ScheduleForDespawn();

        /// Remove object from being despawned
        void RemoveScheduleFromDespawn();
        
        /// Send Packet
        /// @p_PacketBuffer : Packet being sent
        void SendPacket(Server::PacketBuffer const* p_PacketBuffer);

        /// Whether object is set to be despawned
        bool IsScheduledForDespawn() const;

        /// Despawn Object
        bool Despawn();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        Object* m_Object;
        Object* m_PlayerObject;
        Core::Diagnostic::IntervalTimer m_IntervalDelayRemoval;
        bool m_ScheduleForDespawn;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone