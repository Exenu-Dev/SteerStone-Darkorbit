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
#include "Socket.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets {

    /// SERVER_PACKET_DISPLAY_SUCC_DEBUG packet builder
    class DisplaySuccDebug final : public ServerPacket
    {
    public:
        /// Constructor 
        DisplaySuccDebug() : ServerPacket(ServerOpCodes::SERVER_PACKET_DISPLAY_SUCC_DEBUG)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    /// SERVER_PACKET_SEND_MESSAGE packet builder
    class Message final : public ServerPacket
    {
    public:
        /// Constructor 
        Message() : ServerPacket(ServerOpCodes::SERVER_PACKET_SEND_MESSAGE)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        std::string Type;
        std::string Text;
    };

}   ///< Packets
}   ///< Server
}   ///< Game
}   ///< SteerStone