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

#include "MiscPackets.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets { namespace Misc {
    
    /// SERVER_PACKET_DISPLAY_SUCC_DEBUG
    PacketBuffer const* DisplaySuccDebug::Write()
    {
        m_Buffer.Clear();

        m_Buffer.Append("SUCC", strlen("SUCC"));
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// Write the packet
    /// @p_Storage : Storage
    /// @p_InfoUpdate : Type of Info
    PacketBuffer const* Update::Write(InfoUpdate p_InfoUpdate, std::initializer_list<std::variant<int32, std::string>> p_Storage)
    {
        std::string l_Type;

        switch (p_InfoUpdate)
        {
        case InfoUpdate::INFO_UPDATE_MESSAGE:
            l_Type = "STD";
            break;
        case InfoUpdate::INFO_UPDATE_SYSTEM_MESSAGE:
			l_Type = "STM";
			break;
        case InfoUpdate::INFO_UPDATE_SET_SHIELD:
            l_Type = "SHD";
			break;
        case InfoUpdate::INFO_UPDATE_SET_SHIELD_HEALTH:
            l_Type = "HLT";
            break;
        case InfoUpdate::INFO_UPDATE_LEVEL_UP:
            l_Type = "LUP";
            break;
        case InfoUpdate::INFO_UPDATE_CLEAR_ROCKET:
            l_Type = "RCD";
            break;
        case InfoUpdate::INFO_UPDATE_BOOSTERS:
            l_Type = "BS";
            break;
        case InfoUpdate::INFO_UPDATE_EXTRAS_INFO:
            l_Type = "ITM";
			break;
        case InfoUpdate::INFO_UPDATE_CPU_JUMP_CHIP:
            l_Type = "CPU";
			break;
        case InfoUpdate::INFO_UPDATE_MINE_SMB:
            l_Type = "SMB";
            break;
        case InfoUpdate::INFO_UPDATE_SPEED:
            l_Type = "v";
			break;
        case InfoUpdate::INFO_UPDATE_COOLDOWNS:
            l_Type = "CLD";
			break;
        case InfoUpdate::INFO_CLEAR_COOLDOWNS:
            l_Type = "CLR";
			break;
        }

        m_Buffer.AppendChar(l_Type.c_str());

        /// Some Updates require another additional character to specify
        /// the type of update
        switch (p_InfoUpdate)
        {
        case InfoUpdate::INFO_UPDATE_CPU_JUMP_CHIP:
            m_Buffer.AppendChar("J");
            break;
        }

        for (auto& l_Itr : p_Storage)
        {
            if (auto l_Value = std::get_if<std::string>(&l_Itr))
                m_Buffer.AppendChar(l_Value->c_str());
            else if (auto l_Value = std::get_if<int32>(&l_Itr))
                m_Buffer.AppendInt32(*l_Value);
        }

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_REWARD
    /// @p_Storage : Storage of UInt32
    /// @p_RewardType : Type of Reward
    PacketBuffer const* Reward::Write(RewardType p_RewardType, std::initializer_list<std::variant<uint32, int32>> p_Storage)
    {
        std::string l_Type;

        switch (p_RewardType)
        {
        case RewardType::REWARD_TYPE_CREDIT:
            l_Type = "CRE";
            break;
        case RewardType::REWARD_TYPE_URIDIUM:
            l_Type = "URI";
            break;
        case RewardType::REWARD_TYPE_EXPERIENCE:
            l_Type = "EP";
            break;
        case RewardType::REWARD_TYPE_HONOUR:
            l_Type = "HON";
            break;
        case RewardType::REWARD_TYPE_CARGO:
            l_Type = "CAR";
            break;
        case RewardType::REWARD_TYPE_BATTERY:
            l_Type = "BAT";
			break;
        case RewardType::REWARD_TYPE_MINE:
			l_Type = "MIN";
			break;
        }

        m_Buffer.AppendChar(l_Type.c_str());
        
        for (auto& l_Itr : p_Storage)
        {
            if (auto l_Value = std::get_if<uint32>(&l_Itr))
                m_Buffer.AppendUInt32(*l_Value);
            else if (auto l_Value = std::get_if<int32>(&l_Itr))
                m_Buffer.AppendInt32(*l_Value);
        }

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_CROSS_HAIR
    PacketBuffer const* CrossHair::Write()
    {
        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_MISC_INFO
    PacketBuffer const* Info::Write(InfoType p_InfoType, std::initializer_list<std::variant<int32, std::string>> p_Storage)
    {
        std::string l_Type;

        switch (p_InfoType)
        {
        case InfoType::INFO_TYPE_DRONES:
			l_Type = "d";
			break;
        case InfoType::INFO_TYPE_GREY_OPPONENT:
            l_Type = "LSH";
            break;
        case InfoType::INFO_TYPE_UNGREY_OPPONENT:
            l_Type = "USH";
            break;
        case InfoType::INFO_TYPE_RECEIVE_BOOSTER:
			l_Type = "fbo";
			break;
        case InfoType::INFO_TYPE_INVISIBLE:
			l_Type = "INV";
			break;
        case InfoType::INFO_TYPE_SMART_MINE:
            l_Type = "SMB";
            break;
        case InfoType::INFO_TYPE_INSTANT_SHIELD:
            l_Type = "ISH";
			break;
        }

        m_Buffer.AppendChar(l_Type.c_str());

        for (auto& l_Itr : p_Storage)
        {
            if (auto l_Value = std::get_if<std::string>(&l_Itr))
                m_Buffer.AppendChar(l_Value->c_str());
            else if (auto l_Value = std::get_if<int32>(&l_Itr))
                m_Buffer.AppendInt32(*l_Value);
        }

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

}   ///< namespace Misc
}   ///< namespace Packets
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone