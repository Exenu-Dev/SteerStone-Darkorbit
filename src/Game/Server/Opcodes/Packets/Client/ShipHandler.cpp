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

#include "Packets/Server/MiscPackets.hpp"
#include "Packets/Server/ShipPackets.hpp"
#include "Socket.hpp"
#include "Player.hpp"
#include "GameFlags.hpp"

namespace SteerStone { namespace Game { namespace Server {

    /// Ship Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleChangeLaser(ClientPacket* p_Packet)
    {
        int16 l_LaserType = p_Packet->ReadInt16();

        if (l_LaserType >= BatteryType::MAX_BATTERY || l_LaserType < BatteryType::BATTERY_TYPE_LCB10)
        {
            LOG_WARNING("Ship", "Player %0 attempted to change laser type %1 but laser type is out of bounds!", m_Player->GetName(), l_LaserType);
            return;
        }

        m_Player->SetLaserType(l_LaserType);
    }

    /// Ship Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleChangeRocket(ClientPacket* p_Packet)
    {
        int16 l_RocketType = p_Packet->ReadInt16();

        if (l_RocketType >= RocketType::MAX_ROCKET || l_RocketType < RocketType::ROCKET_TYPE_R310)
        {
            LOG_WARNING("Ship", "Player %0 attempted to change rocket type %1 but rocket type is out of bounds!", m_Player->GetName(), l_RocketType);
            return;
        }

        m_Player->SetRocketType(l_RocketType);
    }

    /// Ship Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleChangeConfig(ClientPacket* p_Packet)
    {
        if (p_Packet->ReadString() == "CFG")
        {
            m_Player->ChangeConfiguration(p_Packet->ReadString()[0] == '1' ? 1 : 2);
        }
    }

}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone