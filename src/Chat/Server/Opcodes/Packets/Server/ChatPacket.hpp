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

#pragma once
#include "Socket.hpp"

namespace SteerStone { namespace Chat { namespace Server { namespace Packets {
    class SendMessageToRoom final : public ServerPacket
    {
    public:
        /// Constructor 
        SendMessageToRoom() : ServerPacket("a%")
        {
            RoomId = 0;
            Message = "";
            Username = "";
            Clan = "";
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint16 RoomId;
        std::string Message;
        std::string Username;
        std::string Clan;
    };
}   ///< Packets
}   ///< Server
}   ///< Chat
}   ///< SteerStone