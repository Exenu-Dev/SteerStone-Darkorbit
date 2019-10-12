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

#include "MapPackets.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets {

    /// SERVER_PACKET_CREATE_PORTAL
    PacketBuffer const* CreatePortal::Write()
    {
        m_Buffer.AppendUInt32(Id);
        m_Buffer.AppendUInt16(Type);
        m_Buffer.AppendChar("1"); ///< Unknown, not used by client
        m_Buffer.AppendUInt32(PositionX);
        m_Buffer.AppendUInt32(PositionY);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_CREATE_STATION
    PacketBuffer const* CreateStation::Write()
    {
        m_Buffer.AppendUInt32(Id);
        m_Buffer.AppendUInt32(Type);
        m_Buffer.AppendChar(Name.c_str());
        m_Buffer.AppendUInt16(CompanyId);
        m_Buffer.AppendBool(Peace);
        m_Buffer.AppendUInt32(PositionX);
        m_Buffer.AppendUInt32(PositionY);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_EVENT
    PacketBuffer const* Event::Write()
    {
        m_Buffer.AppendUInt32(PositionX);
        m_Buffer.AppendUInt32(PositionY);
        m_Buffer.AppendBool(InDemolitionZone);
        m_Buffer.AppendBool(PlayRepairAnimation);
        m_Buffer.AppendBool(InTradeZone);
        m_Buffer.AppendBool(InRadiationZone);
        m_Buffer.AppendBool(InJumpZone);
        m_Buffer.AppendBool(Repair);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_JUMP_PORTAL
    PacketBuffer const* JumpPortal::Write()
    {
        m_Buffer.AppendUInt32(MapId);
        m_Buffer.AppendUInt32(PortalId);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_JUMP_PORTAL
    PacketBuffer const* DisplayStarSystem::Write()
    {
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_CARGO
    PacketBuffer const* Cargo::Write()
    {
        m_Buffer.AppendUInt32(Id);
        m_Buffer.AppendUInt32(Type);
        m_Buffer.AppendInt32(PositionX);
        m_Buffer.AppendInt32(PositionY);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_REMOVE_CARGO
    PacketBuffer const* RemoveCargo::Write()
    {
        m_Buffer.AppendUInt32(Id);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

}   ///< Packets
}   ///< Server
}   ///< Game
}   ///< SteerStone