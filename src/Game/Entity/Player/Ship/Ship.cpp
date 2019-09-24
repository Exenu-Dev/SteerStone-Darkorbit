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

#include "Packets/Server/LoginPackets.hpp"
#include "Packets/Server/ShipPackets.hpp"
#include "Player.hpp"
#include "ZoneManager.hpp"
#include "Database/DatabaseTypes.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    /// @p_Player : Player
    Ship::Ship(Player* p_Player) 
        : m_Player(p_Player)
    {
        m_ShipId         = PlayerShips::Phoenix;
        m_CargoSpace     = 0;
        m_MaxCargoSpace  = 0;
        m_MaxBattery     = 0;
        m_MaxRockets     = 0;
        m_UseSystemFont  = 0;
    }
    /// Deconstructor
    Ship::~Ship()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    bool Ship::LoadFromDB()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT type, speed, shield, max_shield, hit_points, max_hit_points, cargo_space, cargo_space_max, "
        "position_x, position_y, map_id, max_battery, max_rockets, weapon_state, use_system_font,"
            " battery_lcb_10, battery_mcb_25, battery_mcb_50, battery_ucb_100, battery_sab_50, rocket_r310, rocket_plt_2026, rocket_plt_2021, mines, smart_bombs, instant_shields FROM account_ship WHERE id = ?");
        l_PreparedStatement->SetUint32(0, m_Player->GetId());
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

            m_ShipId        = static_cast<PlayerShips>(l_Result[0].GetUInt16());
            m_Player->GetSpline()->SetSpeed(l_Result[1].GetUInt32());
            m_Player->m_Shield          = l_Result[2].GetInt32();
            m_Player->m_MaxShield       = l_Result[3].GetUInt32();
            m_Player->m_HitPoints       = l_Result[4].GetInt32();
            m_Player->m_MaxHitPoints    = l_Result[5].GetUInt32();
            m_CargoSpace                = l_Result[6].GetUInt32();
            m_MaxCargoSpace             = l_Result[7].GetUInt32();

            m_Player->GetSpline()->SetPosition(l_Result[8].GetFloat(), l_Result[9].GetFloat(), l_Result[8].GetFloat(), l_Result[9].GetFloat());
            m_Player->SetMap(sZoneManager->GetMap(l_Result[10].GetUInt32()));

            m_MaxBattery                = l_Result[11].GetUInt32();
            m_MaxRockets                = l_Result[12].GetUInt32();
            m_Player->m_WeaponState     = l_Result[13].GetUInt16();
            m_UseSystemFont             = l_Result[14].GetBool();

            m_Ammo.m_BatteryLCB10       = l_Result[15].GetInt32();
            m_Ammo.m_BatteryMCB25       = l_Result[16].GetInt32();
            m_Ammo.m_BatteryMCB50       = l_Result[17].GetInt32();
            m_Ammo.m_BatteryUCB100      = l_Result[18].GetInt32();
            m_Ammo.m_BatterySAB50       = l_Result[19].GetInt32();

            m_Ammo.m_RocketR310         = l_Result[20].GetInt32();
            m_Ammo.m_RocketPLT2026      = l_Result[21].GetInt32();
            m_Ammo.m_RocketPLT2021      = l_Result[22].GetInt32();

            m_Ammo.m_Mines              = l_Result[23].GetInt32();
            m_Ammo.m_SmartBombs         = l_Result[24].GetInt32();
            m_Ammo.m_InstantShields     = l_Result[25].GetInt32();

            return true;
        }

        return false;
    }

    /// Send ship details
    /// @p_Packet : Packet being formed
    void Ship::FormulateInitializeShip(Server::Packets::InitializeShip& p_Packet)
    {
        p_Packet.ShipId         = static_cast<uint16>(m_ShipId);
        p_Packet.Speed          = m_Player->GetSpline()->GetSpeed();
        p_Packet.Shield         = m_Player->m_Shield;
        p_Packet.MaxShield      = m_Player->m_MaxShield;
        p_Packet.HitPoints      = m_Player->m_HitPoints;
        p_Packet.m_MaxHitPoints = m_Player->m_MaxHitPoints;
        p_Packet.CargoSpace     = m_CargoSpace;
        p_Packet.MaxCargoSpace  = m_MaxCargoSpace;
        p_Packet.PositionX      = m_Player->GetSpline()->GetPositionX();
        p_Packet.PositionY      = m_Player->GetSpline()->GetPositionY();
        p_Packet.MapId          = m_Player->GetMap()->GetId();
        p_Packet.MaxBattery     = m_MaxBattery;
        p_Packet.MaxRockets     = m_MaxRockets;
        p_Packet.WeaponState    = m_Player->m_WeaponState;
    }
    /// Send Map update
    void Ship::SendMapUpdate()
    {
        Server::Packets::MapUpdate l_MapUpdatePacket;
        l_MapUpdatePacket.MapId = m_Player->GetMap()->GetId();

        /// TODO; Mini map seems to also update the background
        /// maybe this packet has a purpose, not sure yet
        //m_Player->SendPacket(l_MapUpdatePacket.Write());

        Server::Packets::MiniMapUpdate l_MiniMapUpdatePacket;
        l_MiniMapUpdatePacket.MapId = m_Player->GetMap()->GetId();
        m_Player->SendPacket(l_MiniMapUpdatePacket.Write());

    }
    /// Send Battery, Rocket and mine update
    void Ship::SendAmmoUpdate()
    {
        Server::Packets::UpdateRocketMineAmmo l_RocketMineAmmoPacket;
        l_RocketMineAmmoPacket.RocketR310     = m_Ammo.m_RocketR310;
        l_RocketMineAmmoPacket.RocketPLT2021  = m_Ammo.m_RocketPLT2021;
        l_RocketMineAmmoPacket.RocketPLT2026  = m_Ammo.m_RocketPLT2026;
        l_RocketMineAmmoPacket.Mines          = m_Ammo.m_Mines;
        l_RocketMineAmmoPacket.SmartBombs     = m_Ammo.m_SmartBombs;
        l_RocketMineAmmoPacket.InstantShields = m_Ammo.m_InstantShields;
        
        m_Player->SendPacket(l_RocketMineAmmoPacket.Write());

        Server::Packets::UpdateBatteryAmmo l_BatteryAmmoPacket;
        l_BatteryAmmoPacket.BatteryLCB10      = m_Ammo.m_BatteryLCB10;
        l_BatteryAmmoPacket.BatteryMCB25      = m_Ammo.m_BatteryMCB25;
        l_BatteryAmmoPacket.BatteryMCB50      = m_Ammo.m_BatteryMCB50;
        l_BatteryAmmoPacket.BatteryUCB100     = m_Ammo.m_BatteryUCB100;
        l_BatteryAmmoPacket.BatterySAB50      = m_Ammo.m_BatterySAB50;

        m_Player->SendPacket(l_BatteryAmmoPacket.Write());
    }

    /// Get Ammo
    Ammo* Ship::GetAmmo()
    {
        return &m_Ammo;
    }
}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone