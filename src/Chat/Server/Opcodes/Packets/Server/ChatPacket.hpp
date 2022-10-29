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

    class SystemMessage final : public ServerPacket
    {
    public:
        /// Constructor 
        SystemMessage() : ServerPacket("dq%")
        {
            Message = "";
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        std::string Message;
    };

    class SendAdminMessage final : public ServerPacket
    {
    public:
        /// Constructor 
        SendAdminMessage() : ServerPacket("j%")
        {
            RoomId = 0;
            Message = "";
            Username = "";
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint16 RoomId;
        std::string Message;
        std::string Username;
    };

    class UserCount final : public ServerPacket
    {
    public:
        /// Constructor 
        UserCount() : ServerPacket("dq%")
        {
            Total = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 Total;
    };

    class BanUser final : public ServerPacket
    {
    public:
        /// Constructor 
        BanUser() : ServerPacket("at%")
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class IPBan final : public ServerPacket
    {
    public:
        /// Constructor 
        IPBan() : ServerPacket("ey%")
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class BanMessage final : public ServerPacket
    {
    public:
        /// Constructor 
        BanMessage() : ServerPacket("db%")
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        std::string StartedAt;
        std::string EndsAt;
    };

    class Ping final : public ServerPacket
    {
    public:
        /// Constructor 
        Ping() : ServerPacket("ek%")
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };
}   ///< Packets
}   ///< Server
}   ///< Chat
}   ///< SteerStone