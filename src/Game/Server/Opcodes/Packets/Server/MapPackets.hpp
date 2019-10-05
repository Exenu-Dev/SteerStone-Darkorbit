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

    /// SERVER_PACKET_CREATE_PORTAL packet builder
    class CreatePortal final : public ServerPacket
    {
    public:
        /// Constructor 
        CreatePortal() : ServerPacket(ServerOpCodes::SERVER_PACKET_CREATE_PORTAL)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 Id;
        uint16 Type;
        float PositionX;
        float PositionY;
    };

    /// SERVER_PACKET_CREATE_STATION packet builder
    class CreateStation final : public ServerPacket
    {
    public:
        /// Constructor 
        CreateStation() : ServerPacket(ServerOpCodes::SERVER_PACKET_CREATE_STATION)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 Id;
        uint16 CompanyId;
        uint16 Type;
        std::string Name;
        bool Peace;
        float PositionX;
        float PositionY;
    };

    /// SERVER_PACKET_EVENT packet builder
    class Event final : public ServerPacket
    {
    public:
        /// Constructor 
        Event() : ServerPacket(ServerOpCodes::SERVER_PACKET_EVENT)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        bool InDemolitionZone;
        bool PlayRepairAnimation;
        bool InTradeZone;
        bool InRadiationZone;
        bool InJumpZone;
        bool Repair;
        float PositionX;
        float PositionY;
    };

    /// SERVER_PACKET_JUMP_PORTAL packet builder
    class JumpPortal final : public ServerPacket
    {
    public:
        /// Constructor 
        JumpPortal() : ServerPacket(ServerOpCodes::SERVER_PACKET_JUMP_PORTAL)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 MapId;
        uint32 PortalId;
    };

    /// SERVER_PACKET_DISPLAY_STAR_SYSTEM packet builder
    class DisplayStarSystem final : public ServerPacket
    {
    public:
        /// Constructor 
        DisplayStarSystem() : ServerPacket(ServerOpCodes::SERVER_PACKET_DISPLAY_STAR_SYSTEM)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

}   ///< Packets
}   ///< Server
}   ///< Game
}   ///< SteerStone