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

    /// SERVER_PACKET_MAP_UPDATE packet builder
    class MapUpdate final : public ServerPacket
    {
    public:
        /// Constructor 
        MapUpdate() : ServerPacket(ServerOpCodes::SERVER_PACKET_MAP_UPDATE)
        {
            MapId = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 MapId;
    };

    /// SERVER_PACKET_MINI_MAP_UPDATE packet builder
    class MiniMapUpdate final : public ServerPacket
    {
    public:
        /// Constructor 
        MiniMapUpdate() : ServerPacket(ServerOpCodes::SERVER_PACKET_MINI_MAP_UPDATE)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 MapId;
    };


    /// SERVER_PACKET_UPDATE_ROCKET_MINE packet builder
    class UpdateRocketMineAmmo final : public ServerPacket
    {
    public:
        /// Constructor 
        UpdateRocketMineAmmo() : ServerPacket(ServerOpCodes::SERVER_PACKET_UPDATE_ROCKET_MINE)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 RocketR310;
        uint32 RocketPLT2026;
        uint32 RocketPLT2021;
        uint32 Mines;
        uint32 SmartBombs;
        uint32 InstantShields;
    };

    /// SERVER_PACKET_UPDATE_BATTERY packet builder
    class UpdateBatteryAmmo final : public ServerPacket
    {
    public:
        /// Constructor 
        UpdateBatteryAmmo() : ServerPacket(ServerOpCodes::SERVER_PACKET_UPDATE_BATTERY)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        int32 BatteryLCB10;
        int32 BatteryMCB25;
        int32 BatteryMCB50;
        int32 BatteryUCB100;
        int32 BatterySAB50;
    };

}   ///< Packets
}   ///< Server
}   ///< Game
}   ///< SteerStone