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
#include "Packets/Server/ShipPackets.hpp"
#include "Packets/Server/AttackPackets.hpp"
#include "Packets/Server/MiscPackets.hpp"
#include "Grid.hpp"
#include "Mob.hpp"
#include "Player.hpp"
#include "Utility/UtilMaths.hpp"

namespace SteerStone { namespace Game { namespace Map {

    ///////////////////////////////////////////
    //               OBJECTS
    ///////////////////////////////////////////

    /// Build surrounding objects near player
    /// @p_Object : Player
    void Grid::BuildSurroundingObjects(Entity::Object* p_Object)
    {
        /// NOTE;
        /// When adding an object to the client, for some reason the position gets desync,
        /// so sending movement packet fixes it

        for (auto l_Itr : m_Objects)
        {
            /// Don't send packet to self because the initialize packet handles creation of ship for player
            if (l_Itr.second->GetGUID() == p_Object->GetGUID())
                continue;

            /// Don't add unit if they're dead
            if (l_Itr.second->ToUnit()->IsDead())
                continue;

            /// Portal and station are not ships but they exist in the grid
            if (l_Itr.second->GetType() == Entity::Type::OBJECT_TYPE_PORTAL || l_Itr.second->GetType() == Entity::Type::OBJECT_TYPE_STATION)
                return;

            if (Core::Utils::IsInCircleRadius(p_Object->GetSpline()->GetPositionX(), p_Object->GetSpline()->GetPositionY(),
                l_Itr.second->GetSpline()->GetPositionX(), l_Itr.second->GetSpline()->GetPositionY(), PLAYER_RADIUS_SCAN))
            {
                if (l_Itr.second->GetType() == Entity::Type::OBJECT_TYPE_PLAYER)
                {
                    if (!l_Itr.second->ToPlayer()->IsInSurrounding(p_Object))
                        BuildObjectSpawnAndSend(p_Object, l_Itr.second);
                    else
                        l_Itr.second->ToPlayer()->RemoveScheduleDespawn(p_Object);
                }

                if (!p_Object->ToPlayer()->IsInSurrounding(l_Itr.second))
                    BuildObjectSpawnAndSend(l_Itr.second, p_Object);
                else
                    p_Object->ToPlayer()->RemoveScheduleDespawn(l_Itr.second);
            }
        }
    }
    /// Build Player Spawn Packet
    /// @p_ObjectBuilt : Object being built
    /// @p_Object      : Object
    void Grid::BuildObjectSpawnAndSend(Entity::Object* p_ObjectBuilt, Entity::Object* p_Object)
    {
        /// This must be at the top or else we will enter a infinite recursive function
        p_Object->ToPlayer()->AddToSurrounding(p_ObjectBuilt);

        Server::Packets::Ship::SpawnShip l_Packet;

        if (p_ObjectBuilt->GetType() == Entity::Type::OBJECT_TYPE_PLAYER)
        {
            l_Packet.UserId                 = p_ObjectBuilt->GetObjectGUID().GetCounter();
            l_Packet.ShipId                 = p_ObjectBuilt->ToUnit()->GetShipType();
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
            p_Object->ToPlayer()->SendPacket(l_Packet.Write());

            /// Also Send drone info
            if (p_ObjectBuilt->ToPlayer()->HasDrones())
                p_Object->ToPlayer()->SendPacket(&p_ObjectBuilt->ToPlayer()->BuildDronePacket());
        }
        else if (p_ObjectBuilt->GetType() == Entity::Type::OBJECT_TYPE_NPC)
        {
            l_Packet.UserId                 = p_ObjectBuilt->GetObjectGUID().GetCounter();
            l_Packet.ShipId                 = p_ObjectBuilt->ToUnit()->GetShipType();
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
            p_Object->ToPlayer()->SendPacket(l_Packet.Write());
        }

        if (p_ObjectBuilt->ToUnit()->IsAttacking())
        {
            if (p_ObjectBuilt->ToUnit()->GetTarget() && !p_Object->ToPlayer()->IsInSurrounding(p_ObjectBuilt->ToUnit()->GetTarget()))
                BuildObjectSpawnAndSend(p_ObjectBuilt->ToUnit()->GetTarget(), p_Object);

            if (p_ObjectBuilt->GetType() == Entity::Type::OBJECT_TYPE_PLAYER && p_ObjectBuilt->ToUnit()->GetTarget())
                p_Object->ToPlayer()->SendPacket(Server::Packets::Misc::Info().Write(Server::Packets::Misc::InfoType::INFO_TYPE_GREY_OPPONENT, { p_ObjectBuilt->ToUnit()->GetTarget()->GetObjectGUID().GetCounter(), p_ObjectBuilt->GetObjectGUID().GetCounter() }));

            /// Send Attack
            Server::Packets::Attack::LaserShoot l_Packet;
            l_Packet.FromId     = p_ObjectBuilt->GetObjectGUID().GetCounter();
            l_Packet.ToId       = p_ObjectBuilt->ToUnit()->GetTarget()->GetObjectGUID().GetCounter();
            l_Packet.LaserId    = p_ObjectBuilt->ToUnit()->GetLaserType();
            p_Object->ToPlayer()->SendPacket(l_Packet.Write());
        }

        if (!p_ObjectBuilt->GetSpline()->IsMoving())
        {
            Server::Packets::Ship::ObjectMove l_ObjectMovePacket;
            l_ObjectMovePacket.Id        = p_ObjectBuilt->GetObjectGUID().GetCounter();
            l_ObjectMovePacket.PositionX = p_ObjectBuilt->GetSpline()->GetPositionX();
            l_ObjectMovePacket.PositionY = p_ObjectBuilt->GetSpline()->GetPositionY();
            l_ObjectMovePacket.Time      = 0;
            p_Object->ToPlayer()->SendPacket(l_ObjectMovePacket.Write());
        }
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
                    Server::Packets::Ship::DespawnShip l_Packet;
                    l_Packet.Id = p_Object->GetObjectGUID().GetCounter();
                    l_Itr.second->ToPlayer()->SendPacket(l_Packet.Write());
                }
            }
            
            Server::Packets::Ship::DespawnShip l_Packet;
            l_Packet.Id = l_Itr.second->GetObjectGUID().GetCounter();
            p_Object->ToPlayer()->SendPacket(l_Packet.Write());
        }
    }
}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone