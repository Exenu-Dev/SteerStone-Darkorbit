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

#include "ShipPackets.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets { namespace Ship {

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

    /// SERVER_PACKET_SPAWN_SHIP
    PacketBuffer const* SpawnShip::Write()
    {
        m_Buffer.AppendUInt32(UserId);
        m_Buffer.AppendUInt16(ShipId);
        m_Buffer.AppendUInt16(WeaponState);
        m_Buffer.AppendChar(Clan.c_str());
        m_Buffer.AppendChar(Name.c_str());
        m_Buffer.AppendFloat(PositionX);
        m_Buffer.AppendFloat(PositionY);
        m_Buffer.AppendUInt16(CompanyId);
        m_Buffer.AppendUInt32(ClanId);
        m_Buffer.AppendUInt16(Rank);
        m_Buffer.AppendBool(ShowRedSquareOnMiniMap);
        m_Buffer.AppendUInt16(ClanDiplomat);
        m_Buffer.AppendUInt16(GalaxyGatesAchieved);
        m_Buffer.AppendBool(UseBigFont);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_DESPAWN_SHIP
    PacketBuffer const* DespawnShip::Write()
    {
        m_Buffer.AppendUInt32(Id);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_OBJECT_MOVE
    PacketBuffer const* ObjectMove::Write()
    {
        m_Buffer.AppendUInt32(Id);
        m_Buffer.AppendUInt32(PositionX);
        m_Buffer.AppendUInt32(PositionY);
        m_Buffer.AppendUInt32(Time);
        m_Buffer.AppendUInt16(ShipType);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_CARGO_BAY_FULL
    PacketBuffer const* CargoFull::Write()
    {
        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_CHANGE_CONFIG
    PacketBuffer const* ChangeConfig::Write()
    {
        m_Buffer.AppendChar("CFG");
        m_Buffer.AppendUInt16(Config);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_UPDATE_ORE
    PacketBuffer const* UpdateOre::Write()
    {
        m_Buffer.AppendUInt32(Prometium);
        m_Buffer.AppendUInt32(Endurium);
        m_Buffer.AppendUInt32(Terbium);
        m_Buffer.AppendUInt32(Xenomit);
        m_Buffer.AppendUInt32(Prometid);
        m_Buffer.AppendUInt32(Duranium);
        m_Buffer.AppendUInt32(Promerium);
        m_Buffer.AppendUInt32(Palladium);
        m_Buffer.AppendUInt32(Seprom);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_SET_ORE_PRICES
    PacketBuffer const* OrePrices::Write()
    {
        m_Buffer.AppendUInt32(Prometium);
        m_Buffer.AppendUInt32(Endurium);
        m_Buffer.AppendUInt32(Terbium);
        m_Buffer.AppendUInt32(Prometid);
        m_Buffer.AppendUInt32(Duranium);
        m_Buffer.AppendUInt32(Promerium);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

}   ///< namespace Ship
}   ///< namespace Packets
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone