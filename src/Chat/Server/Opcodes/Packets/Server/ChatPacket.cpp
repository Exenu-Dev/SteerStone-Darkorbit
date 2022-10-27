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

#include "ChatPacket.hpp"

namespace SteerStone { namespace Chat { namespace Server { namespace Packets {

    /// Write the packet
	PacketBuffer const* SendMessageToRoom::Write()
	{
        m_Buffer.AppendUInt16(RoomId);
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(Username.c_str());
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(Message.c_str());
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
	}

    /// Write the packet
    PacketBuffer const* SystemMessage::Write()
    {
        m_Buffer.AppendChar(Message.c_str());
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }

    /// Write the packet
    PacketBuffer const* SendAdminMessage::Write()
    {
        m_Buffer.AppendUInt16(RoomId);
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(Username.c_str());
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(Message.c_str());
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar("1"); //< Admin message type? TODO; Research this
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }

    /// Write the packet
    PacketBuffer const* UserCount::Write()
    {
        std::string l_Message = "Users online " + std::to_string(Total);
        m_Buffer.AppendChar(l_Message.c_str());
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }

    /// Write the packet
    PacketBuffer const* Ping::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
}   ///< Packets
}   ///< Server
}   ///< Chat
}   ///< SteerStone