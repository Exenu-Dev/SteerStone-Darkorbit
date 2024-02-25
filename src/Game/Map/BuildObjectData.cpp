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

#include "Packets/Server/LoginPackets.hpp"
#include "Packets/Server/MapPackets.hpp"
#include "Packets/Server/ShipPackets.hpp"
#include "Packets/Server/AttackPackets.hpp"
#include "Packets/Server/MiscPackets.hpp"
#include "Grid.hpp"
#include "Mob.hpp"
#include "Ore.hpp"
#include "Mine.hpp"
#include "BonusBox.hpp"
#include "ClanManager.hpp"
#include "Player.hpp"
#include "Utility/UtilMaths.hpp"

namespace SteerStone { namespace Game { namespace Map {

    ///////////////////////////////////////////
    //               OBJECTS
    ///////////////////////////////////////////

    /// Build surrounding objects near player
    /// @p_Player : Player to build surrounding objects for
    void Grid::BuildSurroundingObjects(Entity::Player* p_Player)
    {
        /// NOTE;
        /// When adding an object to the client, for some reason the position gets desync,
        /// so sending movement packet fixes it

        for (auto l_Itr : m_Objects)
        {
            /// Don't send packet to self because the initialize packet handles creation of ship for player
            if (l_Itr.second->GetGUID() == p_Player->GetGUID())
                continue;

            /// Don't add unit if they're dead
            if (l_Itr.second->ToUnit()->IsDead())
                continue;

            /// Portal and station are not ships but they exist in the grid
            if (l_Itr.second->IsPortal() || l_Itr.second->IsStation())
                continue;

            if (Core::Utils::IsInCircleRadius(p_Player->GetSpline()->GetPositionX(), l_Itr.second->GetSpline()->GetPositionY(),
                l_Itr.second->GetSpline()->GetPositionX(), l_Itr.second->GetSpline()->GetPositionY(), PLAYER_RADIUS_SCAN))
            {
                if (l_Itr.second->IsPlayer())
                {
                    if (!l_Itr.second->ToPlayer()->IsInSurrounding(p_Player) || l_Itr.second->NeedToBeUpdated())
                        BuildObjectSpawnAndSend(l_Itr.second, p_Player);
                    else
                        l_Itr.second->ToPlayer()->RemoveScheduleDespawn(p_Player);
                }

                if (!p_Player->IsInSurrounding(l_Itr.second) || l_Itr.second->NeedToBeUpdated())
                    BuildObjectSpawnAndSend(l_Itr.second, p_Player);
                else
                    p_Player->ToPlayer()->RemoveScheduleDespawn(l_Itr.second);
            }
        }
    }
    /// Build Player Spawn Packet
    /// @p_ObjectBuilt : Object being built
    /// @p_Player      : Player
    void Grid::BuildObjectSpawnAndSend(Entity::Object* p_ObjectBuilt, Entity::Player* p_Player)
    {
        /// This must be at the top or else we will enter a infinite recursive function
        p_Player->AddToSurrounding(p_ObjectBuilt);

        if (p_ObjectBuilt->IsBonusBox())
        {
            /// We do this because we want to change the friendly cargo to non friendly, only way I can find
            /// I believe this is for cargo
            if (p_ObjectBuilt->ToBonusBox()->NeedToBeUpdated())
            {
                Server::Packets::RemoveCargo l_Packet;
                l_Packet.Id = p_ObjectBuilt->GetObjectGUID().GetCounter();
                p_Player->SendPacket(l_Packet.Write());
            }

            /// TODO; Find packet which updates from friendly to non friendly cargo, currently it visually bugs out when sending Cargo packet again to update
            Server::Packets::Cargo l_Packet;
            l_Packet.Id = p_ObjectBuilt->GetObjectGUID().GetCounter();

            if (p_ObjectBuilt->ToBonusBox()->GetBoxType() == BonusBoxType::BONUS_BOX_TYPE_CARGO)
                l_Packet.Type = (p_ObjectBuilt->ToBonusBox()->GetOwnerId() != p_Player->GetObjectGUID().GetCounter() && p_ObjectBuilt->ToUnit()->GetCompany() == p_Player->GetCompany()) ? !p_ObjectBuilt->ToBonusBox()->IsFriendlyCargo() : 1;
            else
                l_Packet.Type = static_cast<uint16>(p_ObjectBuilt->ToBonusBox()->GetBoxType());

            l_Packet.PositionX  = p_ObjectBuilt->GetSpline()->GetPositionX();
            l_Packet.PositionY  = p_ObjectBuilt->GetSpline()->GetPositionY();
            p_Player->SendPacket(l_Packet.Write());

            return;
        }
        else if (p_ObjectBuilt->IsMine())
        {
            Server::Packets::SpawnMine l_Packet;
            l_Packet.Id = p_ObjectBuilt->GetObjectGUID().GetCounter();
            l_Packet.Type = 1;
            l_Packet.PositionX = p_ObjectBuilt->GetSpline()->GetPositionX();
            l_Packet.PositionY = p_ObjectBuilt->GetSpline()->GetPositionY();
            p_Player->SendPacket(l_Packet.Write());

            return;
        }
        else if (p_ObjectBuilt->IsOre())
        {
            Server::Packets::SpawnOre l_Packet;
			l_Packet.Id         = p_ObjectBuilt->GetObjectGUID().GetCounter();
			l_Packet.Type       = static_cast<uint16>(p_ObjectBuilt->ToOre()->GetOreType());
			l_Packet.PositionX  = p_ObjectBuilt->GetSpline()->GetPositionX();
			l_Packet.PositionY  = p_ObjectBuilt->GetSpline()->GetPositionY();
			p_Player->SendPacket(l_Packet.Write());

            return;
        }
        else if (p_ObjectBuilt->IsPlayer())
        {
            Server::Packets::Ship::SpawnShip l_Packet;
            l_Packet.UserId                 = p_ObjectBuilt->GetObjectGUID().GetCounter();
            l_Packet.ShipId                 = p_ObjectBuilt->ToUnit()->GetShipType();
            l_Packet.WeaponState            = p_ObjectBuilt->ToPlayer()->GetWeaponState();

            if (p_ObjectBuilt->ToPlayer()->GetClan())
            {
                l_Packet.ClanTag            = p_ObjectBuilt->ToPlayer()->GetClan()->GetTag();
                l_Packet.ClanId             = p_ObjectBuilt->ToPlayer()->GetClan()->GetID();
            }

            l_Packet.Name                   = p_ObjectBuilt->GetName();
            l_Packet.PositionX              = p_ObjectBuilt->GetSpline()->GetPositionX();
            l_Packet.PositionY              = p_ObjectBuilt->GetSpline()->GetPositionY();
            l_Packet.CompanyId              = static_cast<uint16>(p_ObjectBuilt->ToPlayer()->GetCompany());
            l_Packet.Rank                   = p_ObjectBuilt->ToPlayer()->GetRank();
            l_Packet.ClanDiplomat           = 0;
            l_Packet.ShowRedSquareOnMiniMap = p_ObjectBuilt->ToPlayer()->GetCompany() != p_Player->GetCompany();
            l_Packet.GalaxyGatesAchieved    = p_ObjectBuilt->ToPlayer()->GetGatesAchieved();
            l_Packet.UseBigFont             = false; ///< TODO; For big ships only?
            p_Player->SendPacket(l_Packet.Write());

            /// Also Send drone info
            if (p_ObjectBuilt->ToPlayer()->HasDrones())
            	p_Player->SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_DRONES, { p_ObjectBuilt->GetObjectGUID().GetCounter(), p_ObjectBuilt->ToPlayer()->BuildDronesString() }));
        }
        else if (p_ObjectBuilt->IsMob())
        {
            Server::Packets::Ship::SpawnShip l_Packet;
            l_Packet.UserId                 = p_ObjectBuilt->GetObjectGUID().GetCounter();
            l_Packet.ShipId                 = p_ObjectBuilt->ToUnit()->GetShipType();
            l_Packet.WeaponState            = p_ObjectBuilt->ToMob()->GetWeaponState();
            l_Packet.ClanTag                = p_ObjectBuilt->ToMob()->GetClan()->GetName();
            l_Packet.Name                   = p_ObjectBuilt->GetName();
            l_Packet.PositionX              = p_ObjectBuilt->GetSpline()->GetPositionX();
            l_Packet.PositionY              = p_ObjectBuilt->GetSpline()->GetPositionY();
            l_Packet.CompanyId              = static_cast<uint16>(p_ObjectBuilt->ToMob()->GetCompany());
            l_Packet.ClanId                 = p_ObjectBuilt->ToMob()->GetClan()->GetID();;
            l_Packet.Rank                   = p_ObjectBuilt->ToMob()->GetRank();
            l_Packet.ClanDiplomat           = 0;
            l_Packet.ShowRedSquareOnMiniMap = false;
            l_Packet.GalaxyGatesAchieved    = p_ObjectBuilt->ToMob()->GetGatesAchieved();
            l_Packet.UseBigFont             = false; ///< TODO; For big ships only?
            p_Player->SendPacket(l_Packet.Write());
        }

        if ((p_ObjectBuilt->IsMob() || p_ObjectBuilt->IsPlayer()) && p_ObjectBuilt->ToUnit()->IsAttacking())
        {
            if (p_ObjectBuilt->IsPlayer() && p_ObjectBuilt->ToUnit()->GetTarget())
                p_Player->SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_GREY_OPPONENT, { p_ObjectBuilt->ToUnit()->GetTarget()->GetObjectGUID().GetCounter(), p_ObjectBuilt->GetObjectGUID().GetCounter() }));

            /// Send Attack
            Server::Packets::Attack::LaserShoot l_Packet;
            l_Packet.FromId     = p_ObjectBuilt->GetObjectGUID().GetCounter();
            l_Packet.ToId       = p_ObjectBuilt->ToUnit()->GetTarget()->GetObjectGUID().GetCounter();
            l_Packet.LaserId    = p_ObjectBuilt->ToUnit()->GetLaserColourId();
            p_Player->SendPacket(l_Packet.Write());
        }

        Server::Packets::Ship::ObjectMove l_ObjectMovePacket;
        p_ObjectBuilt->GetSpline()->UpdatePosition();
        l_ObjectMovePacket.Id        = p_ObjectBuilt->GetObjectGUID().GetCounter();
        l_ObjectMovePacket.PositionX = p_ObjectBuilt->GetSpline()->GetPlannedPositionX();
        l_ObjectMovePacket.PositionY = p_ObjectBuilt->GetSpline()->GetPlannedPositionY();
        l_ObjectMovePacket.Time      = p_ObjectBuilt->GetSpline()->CalculateDestinationTime();
        p_Player->SendPacket(l_ObjectMovePacket.Write());
    }
    /// Build Object Despawn Packet
    /// @p_Object : Object being built
    void Grid::BuildObjectDespawnAndSend(Entity::Object* p_Object)
    {
        for (auto l_Itr : m_Objects)
        {
            if (l_Itr.second->GetGUID() != p_Object->GetGUID())
            {
                if (l_Itr.second->IsPlayer())
                {
                    Server::Packets::Ship::DespawnShip l_Packet;
                    l_Packet.Id = p_Object->GetObjectGUID().GetCounter();
                    l_Itr.second->ToPlayer()->SendPacket(l_Packet.Write());
                }
            }

            if (l_Itr.second->IsOre())
            {
                Server::Packets::DespawnOre l_Packet;
                l_Packet.Id = l_Itr.second->GetObjectGUID().GetCounter();
                l_Itr.second->GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), l_Itr.second);
            }
            else
            {
                Server::Packets::Ship::DespawnShip l_Packet;
                l_Packet.Id = l_Itr.second->GetObjectGUID().GetCounter();
                p_Object->ToPlayer()->SendPacket(l_Packet.Write());
            }
        }
    }
}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone