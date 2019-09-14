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
#include "Database/DatabaseTypes.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    /// @p_Player : Player
    Ship::Ship(Player* p_Player) 
        : m_Player(p_Player)
    {
        m_ShipId        = PlayerShips::Phoenix;
        m_Speed         = 0;
        m_Shield        = 0;
        m_MaxShield     = 0;
        m_HitPoints     = 0;
        m_MaxHitPoints  = 0;
        m_CargoSpace    = 0;
        m_MaxCargoSpace = 0;
        m_PositionX     = 0.0f;
        m_PositionY     = 0.0f;
        m_MapId         = 0;
        m_MaxAmmo    = 0;
        m_MaxRockets    = 0;
        m_WeaponState   = 0;
        m_UseSystemFont = 0;
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
        "position_x, position_y, map_id, max_ammo, max_rockets, weapon_state, use_system_font,"
            " rocket_r310, rocket_plt_2026, rocket_plt_2021, mines, smart_bombs, instant_shields FROM account_ship WHERE id = ?");
        l_PreparedStatement->SetUint32(0, m_Player->GetId());
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

            m_ShipId        = static_cast<PlayerShips>(l_Result[0].GetUInt16());
            m_Speed         = l_Result[1].GetUInt32();
            m_Shield        = l_Result[2].GetInt32();
            m_MaxShield     = l_Result[3].GetUInt32();
            m_HitPoints     = l_Result[4].GetInt32();
            m_MaxHitPoints  = l_Result[5].GetUInt32();
            m_CargoSpace    = l_Result[6].GetUInt32();
            m_MaxCargoSpace = l_Result[7].GetUInt32();
            m_PositionX     = l_Result[8].GetFloat();
            m_PositionY     = l_Result[9].GetFloat();
            m_MapId         = l_Result[10].GetUInt32();
            m_MaxAmmo       = l_Result[11].GetUInt32();
            m_MaxRockets    = l_Result[12].GetUInt32();
            m_WeaponState   = l_Result[13].GetUInt16();
            m_UseSystemFont = l_Result[14].GetBool();
            m_RocketR310    = l_Result[15].GetUInt32();
            m_RocketPLT2026 = l_Result[16].GetUInt32();
            m_RocketPLT2021 = l_Result[17].GetUInt32();
            m_Mines         = l_Result[18].GetUInt32();
            m_SmartBombs    = l_Result[19].GetUInt32();   
            m_InstantShields= l_Result[20].GetUInt32();   

            return true;
        }

        return false;
    }

    /// Send ship details
    /// @p_Packet : Packet being formed
    void Ship::FormulateInitializeShip(Server::Packets::InitializeShip& p_Packet)
    {
        p_Packet.ShipId         = static_cast<uint16>(m_ShipId);
        p_Packet.Speed          = m_Speed;
        p_Packet.Shield         = m_Shield;
        p_Packet.MaxShield      = m_MaxShield;
        p_Packet.HitPoints      = m_HitPoints;
        p_Packet.m_MaxHitPoints = m_MaxHitPoints;
        p_Packet.CargoSpace     = m_CargoSpace;
        p_Packet.MaxCargoSpace  = m_MaxCargoSpace;
        p_Packet.PositionX      = m_PositionX;
        p_Packet.PositionY      = m_PositionY;
        p_Packet.MapId          = m_MapId;
        p_Packet.MaxBattery     = m_MaxAmmo;
        p_Packet.MaxRockets     = m_MaxRockets;
        p_Packet.WeaponState    = m_WeaponState;
    }
    /// Send Map update
    void Ship::SendMapUpdate()
    {
        Server::Packets::MapUpdate l_MapUpdatePacket;
        l_MapUpdatePacket.MapId = m_MapId;

        Server::Packets::MiniMapUpdate l_MiniMapUpdatePacket;
        l_MiniMapUpdatePacket.MapId = m_MapId;

        /// TODO; Mini map seems to also update the background
        /// maybe this packet has a purpose, not sure yet
        //m_Player->SendPacket(l_MapUpdatePacket.Write());
        m_Player->SendPacket(l_MiniMapUpdatePacket.Write());

    }
    /// Send Rocket and Mine Update
    void Ship::SendRocketMineUpdate()
    {
        Server::Packets::UpdateRocketMineAmmo l_Packet;
        l_Packet.RocketR310     = m_RocketR310;
        l_Packet.RocketPLT2021  = m_RocketPLT2021;
        l_Packet.RocketPLT2026  = m_RocketPLT2026;
        l_Packet.Mines          = m_Mines;
        l_Packet.SmartBombs     = m_SmartBombs;
        l_Packet.InstantShields = m_InstantShields;
        
        m_Player->SendPacket(l_Packet.Write());
    }
}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone