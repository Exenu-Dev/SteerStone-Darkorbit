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

#include "LoginPackets.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets { namespace Login {

    /// SERVER_PACKET_LOGIN
    PacketBuffer const* PlayerInfo::Write(InfoType p_InfoType, std::initializer_list<std::variant<uint32, bool>> p_Storage)
    {
        std::string l_Type;

        switch (p_InfoType)
        {
        case InfoType::INFO_TYPE_SET_ADMIN:
            l_Type = "ADM|CLI";
            break;
        case InfoType::INFO_TYPE_SET_SHIELD:
            l_Type = "SHD";
            break;
        case InfoType::INFO_TYPE_SET_SHIELD_HEALTH:
            l_Type = "HLT";
            break;
        case InfoType::INFO_TYPE_SET_CARGO_SPACE:
            l_Type = "c";
            break;
        case InfoType::INFO_TYPE_SET_SETTINGS:
            l_Type = "SET";
            break;
        case InfoType::INFO_TYPE_SET_MESSAGE:
            l_Type = "STM"; ///< TODO
            break;
        }

        m_Buffer.AppendChar(l_Type.c_str());

        for (auto& l_Itr : p_Storage)
        {
            if (auto l_Value = std::get_if<bool>(&l_Itr))
                m_Buffer.AppendBool(*l_Value);
            else if (auto l_Value = std::get_if<uint32>(&l_Itr))
                m_Buffer.AppendUInt32(*l_Value);
        }

        if (Message.length())
        {
            m_Buffer.AppendChar(Message.c_str());
        }

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        LOG_INFO("test", (const char*)m_Buffer.GetContents());

        return &m_Buffer;
    }

    /// SERVER_PACKET_INITIALIZE_SHIP
    PacketBuffer const* InitializeShip::Write()
    {
        m_Buffer.AppendUInt32(Id);
        m_Buffer.AppendChar(Username.c_str());
        m_Buffer.AppendUInt16(ShipId);
        m_Buffer.AppendUInt32(Speed);
        m_Buffer.AppendInt32(Shield);
        m_Buffer.AppendUInt32(MaxShield);
        m_Buffer.AppendInt32(HitPoints);
        m_Buffer.AppendUInt32(MaxHitPoints);
        m_Buffer.AppendUInt32(CargoSpace);
        m_Buffer.AppendUInt32(MaxCargoSpace);
        m_Buffer.AppendFloat(PositionX);
        m_Buffer.AppendFloat(PositionY);
        m_Buffer.AppendUInt32(MapId);
        m_Buffer.AppendUInt16(CompanyId);
        m_Buffer.AppendUInt32(ClanId);
        m_Buffer.AppendUInt32(MaxBattery);
        m_Buffer.AppendUInt32(MaxRockets);
        m_Buffer.AppendUInt16(WeaponState);
        m_Buffer.AppendBool(IsPremium);
        m_Buffer.AppendUInt32(Experience);
        m_Buffer.AppendInt32(Honour);
        m_Buffer.AppendUInt32(Level);
        m_Buffer.AppendUInt32(Credits);
        m_Buffer.AppendUInt32(Uridium);
        m_Buffer.AppendUInt32(JackPot);
        m_Buffer.AppendUInt16(Rank);
        m_Buffer.AppendChar(ClanTag.c_str());
        m_Buffer.AppendUInt16(GatesAchieved);
        m_Buffer.AppendBool(UseSystemFont);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_LOGGED_IN
    PacketBuffer const* LoggedIn::Write()
    {
        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_LOG_OUT
    PacketBuffer const* LogOut::Write()
    {
        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

}   ///< namespace Login
}   ///< namespace Packets
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone