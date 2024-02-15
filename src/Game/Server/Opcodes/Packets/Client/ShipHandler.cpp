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

#include "Packets/Server/MiscPackets.hpp"
#include "Packets/Server/ShipPackets.hpp"
#include "Packets/Server/AttackPackets.hpp"
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

        // If attacking, also update laser colour
        if (m_Player->IsAttacking())
        {
            Server::Packets::Attack::LaserShoot l_Packet;
            l_Packet.FromId = m_Player->GetObjectGUID().GetCounter();
            l_Packet.ToId = m_Player->GetTarget()->GetObjectGUID().GetCounter();
            l_Packet.LaserId = m_Player->GetLaserColourId();
            m_Player->GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), m_Player, true);
        }
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

    /// Ship Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleSellOre(ClientPacket* p_Packet)
    {
        if (m_Player->GetEvent() != EventType::EVENT_TYPE_STATION)
        {
            m_Player->SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_MESSAGE, { "You are not near a station!" }));
            return;
        }

        uint32 l_ResourceId = p_Packet->ReadUInt32();
        uint32 l_Total = p_Packet->ReadUInt32();

        // Convert Client Id to our own id
        switch (static_cast<OreResource>(l_ResourceId))
        {
        case OreResource::ORE_PROMETIUM:
			l_ResourceId = Entity::Resource::RESOURCE_PROMETIUM;
			break;
        case OreResource::ORE_ENDURIUM:
			l_ResourceId = Entity::Resource::RESOURCE_ENDURIUM;
			break;
        case OreResource::ORE_TERBIUM:
            l_ResourceId = Entity::Resource::RESOURCE_TERBIUM;
            break;
        case OreResource::ORE_PROMETID:
            l_ResourceId = Entity::Resource::RESOURCE_PROMETID;
            break;
        case OreResource::ORE_DURANIUM:
			l_ResourceId = Entity::Resource::RESOURCE_DURANIUM;
			break;
        case OreResource::ORE_PROMERIUM:
            l_ResourceId = Entity::Resource::RESOURCE_PROMERIUM;
            break;
        default:
            LOG_WARNING("Player", "Player %0 attempted to sell ore with invalid resource id %1", m_Player->GetName(), l_ResourceId);
            return;
        }

        if (l_ResourceId > MAX_RESOURCE_COUNTER)
            return;

        if (l_Total > m_Player->GetResource(l_ResourceId))
        {
            LOG_WARNING("Player", "Player %0 attempted to sell ore more than what they have", m_Player->GetName());
            return;
        }

        if (m_Player->GetResource(l_ResourceId) == 0)
            return;

        m_Player->OnTradeOre(l_ResourceId, l_Total);
    }

    /// Ship Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleOrePrices(ClientPacket* p_Packet)
    {
        /// Can only trigger at a station
        if (m_Player->GetEvent() != EventType::EVENT_TYPE_STATION)
            return;

        m_Player->SetOrePrices();
    }

}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone