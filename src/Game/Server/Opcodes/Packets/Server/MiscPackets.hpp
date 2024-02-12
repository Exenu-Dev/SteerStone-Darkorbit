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
#include <variant>

namespace SteerStone { namespace Game { namespace Server { namespace Packets { namespace Misc {

    enum class RewardType
    {
        REWARD_TYPE_CREDIT,
        REWARD_TYPE_URIDIUM,
        REWARD_TYPE_HONOUR,
        REWARD_TYPE_EXPERIENCE,
        REWARD_TYPE_CARGO,
        REWARD_TYPE_BATTERY,
    };

    enum class InfoType
    {
        INFO_TYPE_DRONES,
        INFO_TYPE_GREY_OPPONENT,
        INFO_TYPE_UNGREY_OPPONENT,
    };

    enum class InfoUpdate
    {
        INFO_UPDATE_MESSAGE,
        INFO_UPDATE_HEALTH,
        INFO_UPDATE_LEVEL_UP,
        INFO_UPDATE_CLEAR_ROCKET,
        INFO_UPDATE_BOOSTERS
    };

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

    /// SERVER_PACKET_MISC_UPDATE packet builder
    class Update final : public ServerPacket
    {
    public:
        /// Constructor 
        Update() : ServerPacket(ServerOpCodes::SERVER_PACKET_MISC_UPDATE)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        /// @p_Storage : Storage
        /// @p_InfoUpdate : Type of Info
        PacketBuffer const* Write(InfoUpdate p_InfoUpdate, std::initializer_list<std::variant<int32, std::string>> p_Storage = std::initializer_list<std::variant<int32, std::string>>());
    };

    /// SERVER_PACKET_MISC_INFO packet builder
    class Info final : public ServerPacket
    {
    public:
        /// Constructor 
        Info() : ServerPacket(ServerOpCodes::SERVER_PACKET_MISC_INFO)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        /// @p_Storage : Storage
        /// @p_InfoType : Type of Info
        PacketBuffer const* Write(InfoType p_InfoType, std::initializer_list<std::variant<int32, std::string>> p_Storage);
    };

    /// SERVER_PACKET_REWARD packet builder
    class Reward final : public ServerPacket
    {
    public:
        /// Constructor 
        Reward() : ServerPacket(ServerOpCodes::SERVER_PACKET_REWARD)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        /// @p_Storage : Storage of UInt32
        /// @p_RewardType : Type of Reward
        PacketBuffer const* Write(RewardType p_RewardType, std::initializer_list<std::variant<uint32, int32>> p_Storage);
    };

    /// SERVER_PACKET_CROSS_HAIR packet builder
    class CrossHair final : public ServerPacket
    {
    public:
        /// Constructor 
        CrossHair() : ServerPacket(ServerOpCodes::SERVER_PACKET_CROSS_HAIR)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

}   ///< namespace Misc
}   ///< namespace Packets
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone