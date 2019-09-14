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

#include "ShipPackets.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets {

    /// SERVER_PACKET_MAP_UPDATE
    PacketBuffer const* MapUpdate::Write()
    {
        m_Buffer.AppendUInt32(MapId);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_MINI_MAP_UPDATE
    PacketBuffer const* MiniMapUpdate::Write()
    {
        m_Buffer.AppendUInt32(MapId);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_UPDATE_ROCKET_MINE
    PacketBuffer const* UpdateRocketMineAmmo::Write()
    {
        m_Buffer.AppendInt32(RocketR310);
        m_Buffer.AppendInt32(RocketPLT2026);
        m_Buffer.AppendInt32(RocketPLT2021);
        m_Buffer.AppendInt32(Mines);
        m_Buffer.AppendInt32(SmartBombs);
        m_Buffer.AppendInt32(InstantShields);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_UPDATE_BATTERY
    PacketBuffer const* UpdateBatteryAmmo::Write()
    {
        m_Buffer.AppendInt32(BatteryLCB10);
        m_Buffer.AppendInt32(BatteryMCB25);
        m_Buffer.AppendInt32(BatteryMCB50);
        m_Buffer.AppendInt32(BatteryUCB100);
        m_Buffer.AppendInt32(BatterySAB50);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

}   ///< Packets
}   ///< Server
}   ///< Game
}   ///< SteerStone