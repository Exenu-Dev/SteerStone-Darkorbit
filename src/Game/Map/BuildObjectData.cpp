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

#include "Packets/Server/MapPackets.hpp"
#include "Grid.hpp"
#include "Mob.hpp"
#include "Player.hpp"

namespace SteerStone { namespace Game { namespace Map {
    
    /// Spawn Object to grid
    /// @p_Obiect : Object being spawned
    void Grid::BuildObjectSpawnAndSend(Entity::Object* p_Object)
    {
        for (auto l_Itr : m_Objects)
        {
            if (l_Itr.second->GetType() == Entity::Type::OBJECT_TYPE_PLAYER)
            { 
                if (l_Itr.second->GetGUID() != p_Object->GetGUID())
                {
                    Server::Packets::SpawnShip l_Packet;
                    l_Packet.UserId                 = p_Object->ToPlayer()->GetObjectGUID().GetCounter();
                    l_Packet.ShipId                 = static_cast<uint16>(p_Object->ToPlayer()->GetShip()->GetShipId());
                    l_Packet.WeaponState            = p_Object->ToPlayer()->GetWeaponState();
                    l_Packet.Clan                   = p_Object->ToPlayer()->GetClanName();
                    l_Packet.Name                   = p_Object->GetName();
                    l_Packet.PositionX              = p_Object->GetSpline()->GetPositionX();
                    l_Packet.PositionY              = p_Object->GetSpline()->GetPositionY();
                    l_Packet.CompanyId              = static_cast<uint16>(p_Object->ToPlayer()->GetCompany());
                    l_Packet.ClanId                 = p_Object->ToPlayer()->GetClanId();
                    l_Packet.Rank                   = p_Object->ToPlayer()->GetRank();
                    l_Packet.ClanDiplomat           = 0;
                    l_Packet.ShowRedSquareOnMiniMap = l_Itr.second->ToPlayer()->GetCompany() != p_Object->ToPlayer()->GetCompany();
                    l_Packet.GalaxyGatesAchieved    = p_Object->ToPlayer()->GetGatesAchieved();
                    l_Packet.UseBigFont             = false; ///< TODO; For big ships only?
                    l_Itr.second->ToPlayer()->SendPacket(l_Packet.Write());

                    /// Also send movement packet as client has problems without it
                    Server::Packets::ObjectMove l_ObjectMovePacket;
                    l_ObjectMovePacket.Id        = p_Object->GetGUID();
                    l_ObjectMovePacket.PositionX = p_Object->GetSpline()->GetPositionX();
                    l_ObjectMovePacket.PositionY = p_Object->GetSpline()->GetPositionY();
                    l_ObjectMovePacket.Time      = 0;
                    l_Itr.second->ToPlayer()->SendPacket(l_ObjectMovePacket.Write());
                }
                
                Server::Packets::SpawnShip l_Packet;
                l_Packet.UserId                 = l_Itr.second->GetObjectGUID().GetCounter();
                l_Packet.ShipId                 = static_cast<uint16>(l_Itr.second->ToPlayer()->GetShip()->GetShipId());
                l_Packet.WeaponState            = l_Itr.second->ToPlayer()->GetWeaponState();
                l_Packet.Clan                   = l_Itr.second->ToPlayer()->GetClanName();
                l_Packet.Name                   = l_Itr.second->GetName();
                l_Packet.PositionX              = l_Itr.second->GetSpline()->GetPositionX();
                l_Packet.PositionY              = l_Itr.second->GetSpline()->GetPositionY();
                l_Packet.CompanyId              = static_cast<uint16>(l_Itr.second->ToPlayer()->GetCompany());
                l_Packet.ClanId                 = l_Itr.second->ToPlayer()->GetClanId();
                l_Packet.Rank                   = l_Itr.second->ToPlayer()->GetRank();
                l_Packet.ClanDiplomat           = 0;
                l_Packet.ShowRedSquareOnMiniMap = l_Itr.second->ToPlayer()->GetCompany() != p_Object->ToPlayer()->GetCompany();
                l_Packet.GalaxyGatesAchieved    = l_Itr.second->ToPlayer()->GetGatesAchieved();
                l_Packet.UseBigFont             = false; ///< TODO; For big ships only?
                p_Object->ToPlayer()->SendPacket(l_Packet.Write());
            } 
            else if (l_Itr.second->GetType() == Entity::Type::OBJECT_TYPE_NPC)
            {
                Server::Packets::SpawnShip l_Packet;
                l_Packet.UserId                 = l_Itr.second->GetObjectGUID().GetCounter();
                l_Packet.ShipId                 = l_Itr.second->ToMob()->GetShipId();
                l_Packet.WeaponState            = l_Itr.second->ToMob()->GetWeaponState();
                l_Packet.Clan                   = l_Itr.second->ToMob()->GetClanName();
                l_Packet.Name                   = l_Itr.second->GetName();
                l_Packet.PositionX              = l_Itr.second->GetSpline()->GetPositionX();
                l_Packet.PositionY              = l_Itr.second->GetSpline()->GetPositionY();
                l_Packet.CompanyId              = static_cast<uint16>(l_Itr.second->ToMob()->GetCompany());
                l_Packet.ClanId                 = l_Itr.second->ToMob()->GetClanId();
                l_Packet.Rank                   = l_Itr.second->ToMob()->GetRank();
                l_Packet.ClanDiplomat           = 0;
                l_Packet.ShowRedSquareOnMiniMap = false;
                l_Packet.GalaxyGatesAchieved    = l_Itr.second->ToMob()->GetGatesAchieved();
                l_Packet.UseBigFont             = false; ///< TODO; For big ships only?
                p_Object->ToPlayer()->SendPacket(l_Packet.Write());
            }

            /// Also send movement packet as client has problems without it
            Server::Packets::ObjectMove l_ObjectMovePacket;
            l_ObjectMovePacket.Id           = l_Itr.second->GetObjectGUID().GetCounter();
            l_ObjectMovePacket.PositionX    = l_Itr.second->GetSpline()->GetPositionX();
            l_ObjectMovePacket.PositionY    = l_Itr.second->GetSpline()->GetPositionY();
            l_ObjectMovePacket.Time         = 0;
            p_Object->ToPlayer()->SendPacket(l_ObjectMovePacket.Write());
        }
    }
    /// Build Player Spawn Packet
    /// @p_ObjectBuilt : Object being built
    /// @p_Object      : Object
    Server::PacketBuffer const Grid::BuildPlayerSpawn(Entity::Object* p_ObjectBuilt, Entity::Object* p_Object)
    {
        Server::Packets::SpawnShip l_Packet;

        if (p_ObjectBuilt->GetType() == Entity::Type::OBJECT_TYPE_PLAYER)
        {
            l_Packet.UserId                 = p_ObjectBuilt->GetObjectGUID().GetCounter();
            l_Packet.ShipId                 = static_cast<uint16>(p_ObjectBuilt->ToPlayer()->GetShip()->GetShipId());
            l_Packet.WeaponState            = p_ObjectBuilt->ToPlayer()->GetWeaponState();
            l_Packet.Clan                   = p_ObjectBuilt->ToPlayer()->GetClanName();
            l_Packet.Name                   = p_ObjectBuilt->GetName();
            l_Packet.PositionX              = p_ObjectBuilt->GetSpline()->GetPositionX();
            l_Packet.PositionY              = p_ObjectBuilt->GetSpline()->GetPositionY();
            l_Packet.CompanyId              = static_cast<uint16>(p_ObjectBuilt->ToPlayer()->GetCompany());
            l_Packet.ClanId                 = p_ObjectBuilt->ToPlayer()->GetClanId();
            l_Packet.Rank                   = p_ObjectBuilt->ToPlayer()->GetRank();
            l_Packet.ClanDiplomat           = 0;
            l_Packet.ShowRedSquareOnMiniMap = p_ObjectBuilt->ToPlayer()->GetCompany() != p_Object->ToPlayer()->GetCompany();
            l_Packet.GalaxyGatesAchieved    = p_ObjectBuilt->ToPlayer()->GetGatesAchieved();
            l_Packet.UseBigFont             = false; ///< TODO; For big ships only?
        }
        else if (p_ObjectBuilt->GetType() == Entity::Type::OBJECT_TYPE_NPC)
        {
            l_Packet.UserId                 = p_ObjectBuilt->GetObjectGUID().GetCounter();
            l_Packet.ShipId                 = static_cast<uint16>(p_ObjectBuilt->ToMob()->GetShipId());
            l_Packet.WeaponState            = p_ObjectBuilt->ToMob()->GetWeaponState();
            l_Packet.Clan                   = p_ObjectBuilt->ToMob()->GetClanName();
            l_Packet.Name                   = p_ObjectBuilt->GetName();
            l_Packet.PositionX              = p_ObjectBuilt->GetSpline()->GetPositionX();
            l_Packet.PositionY              = p_ObjectBuilt->GetSpline()->GetPositionY();
            l_Packet.CompanyId              = static_cast<uint16>(p_ObjectBuilt->ToMob()->GetCompany());
            l_Packet.ClanId                 = p_ObjectBuilt->ToMob()->GetClanId();
            l_Packet.Rank                   = p_ObjectBuilt->ToMob()->GetRank();
            l_Packet.ClanDiplomat           = 0;
            l_Packet.ShowRedSquareOnMiniMap = false;
            l_Packet.GalaxyGatesAchieved    = p_ObjectBuilt->ToMob()->GetGatesAchieved();
            l_Packet.UseBigFont             = false; ///< TODO; For big ships only?
        }

        return *l_Packet.Write();
    }
    /// Build Object Despawn Packet
    /// @p_Object : Object being built
    void Grid::BuildObjectDespawnAndSend(Entity::Object* p_Object)
    {
        for (auto l_Itr : m_Objects)
        {
            if (l_Itr.second->GetGUID() != p_Object->GetGUID())
            {
                if (l_Itr.second->GetType() == Entity::Type::OBJECT_TYPE_PLAYER)
                {
                    Server::Packets::DespawnShip l_Packet;
                    l_Packet.Id = p_Object->GetObjectGUID().GetCounter();
                    l_Itr.second->ToPlayer()->SendPacket(l_Packet.Write());
                }
            }

            Server::Packets::DespawnShip l_Packet;
            l_Packet.Id = l_Itr.second->GetObjectGUID().GetCounter();
            p_Object->ToPlayer()->SendPacket(l_Packet.Write());
        }
    }
}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone