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
#include <variant>

namespace SteerStone { namespace Game { namespace Server { namespace Packets { namespace Login {

    enum class InfoType
    {
        INFO_TYPE_SET_SETTINGS,
        INFO_TYPE_SET_ADMIN,
        INFO_TYPE_SET_CARGO_SPACE,
    };

    /// SERVER_PACKET_PLAYER_INFO packet builder
    class PlayerInfo final : public ServerPacket
    {
        public:
            /// Constructor 
            PlayerInfo() : ServerPacket(ServerOpCodes::SERVER_PACKET_PLAYER_INFO)
            {
            }

            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////

            /// Write the packet
            PacketBuffer const* Write(InfoType p_InfoType, std::initializer_list<std::variant<uint32, bool>> p_Storage);
    };

    /// SERVER_PACKET_INITIALIZE_SHIP packet builder
    class InitializeShip final : public ServerPacket
    {
    public:
        /// Constructor 
        InitializeShip() : ServerPacket(ServerOpCodes::SERVER_PACKET_INITIALIZE_SHIP)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
        
        uint32 Id;
        std::string Username;
        uint16 ShipId;
        uint32 Speed;
        int32 Shield;
        uint32 MaxShield;
        int32 HitPoints;
        uint32 MaxHitPoints;
        uint32 CargoSpace;
        uint32 MaxCargoSpace;
        float PositionX;
        float PositionY;
        uint32 MapId;
        uint16 CompanyId;
        uint32 ClanId;
        uint32 MaxBattery;
        uint32 MaxRockets;
        uint16 WeaponState;
        bool IsPremium;
        uint32 Experience;
        int32 Honour;
        uint16 Level;
        uint32 Credits;
        uint32 Uridium;
        uint32 JackPot;
        uint16 Rank;
        std::string ClanTag;
        uint16 GatesAchieved;
        bool UseSystemFont;
    };

    /// SERVER_PACKET_LOGGED_IN packet builder
    class LoggedIn final : public ServerPacket
    {
    public:
        /// Constructor 
        LoggedIn() : ServerPacket(ServerOpCodes::SERVER_PACKET_LOGGED_IN)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    /// SERVER_PACKET_LOG_OUT packet builder
    class LogOut final : public ServerPacket
    {
    public:
        /// Constructor 
        LogOut() : ServerPacket(ServerOpCodes::SERVER_PACKET_LOG_OUT)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

}   ///< namespace Login
}   ///< namespace Packets
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone