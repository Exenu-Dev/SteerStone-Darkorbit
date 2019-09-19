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

    /// SERVER_PACKET_SPAWN_SHIP packet builder
    class SpawnShip final : public ServerPacket
    {
    public:
        /// Constructor 
        SpawnShip() : ServerPacket(ServerOpCodes::SERVER_PACKET_SPAWN_SHIP)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 UserId;
        uint16 ShipId;
        uint16 WeaponState;
        std::string Clan;
        std::string Name;
        float PositionX;
        float PositionY;
        uint16 CompanyId;
        uint32 ClanId;
        uint16 Rank;
        bool ShowRedSquareOnMiniMap;
        uint16 ClanDiplomat;
        uint16 GalaxyGatesAchieved;
        bool UseBigFont;
    };

    /// SERVER_PACKET_DESPAWN_SHIP packet builder
    class DespawnShip final : public ServerPacket
    {
    public:
        /// Constructor 
        DespawnShip() : ServerPacket(ServerOpCodes::SERVER_PACKET_DESPAWN_SHIP)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 Id;
    };

    /// SERVER_PACKET_OBJECT_MOVE packet builder
    class ObjectMove final : public ServerPacket
    {
    public:
        /// Constructor 
        ObjectMove() : ServerPacket(ServerOpCodes::SERVER_PACKET_OBJECT_MOVE)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 Id;
        float PositionX;
        float PositionY;
        uint32 Time;
    };

}   ///< Packets
}   ///< Server
}   ///< Game
}   ///< SteerStone