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

#include "Socket.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "GameFlags.hpp"
#include "Server/LoginPackets.hpp"

/// Note;
/// I'm not using the Server Packets to send a response back.
/// TODO; Use Server packets e.g WebPackets and update CMS to handle new server response

namespace SteerStone { namespace Game { namespace Server {

    /// Web Handler
    /// @p_ClientPacket : Packet recieved from Web
    void GameSocket::HandleCheckOnline(ClientPacket* p_Packet)
    {
        uint32 l_PlayerId = p_Packet->ReadUInt32();

        Entity::Player* l_Player = sWorldManager->FindPlayer(l_PlayerId);

        char l_Online = (l_Player ? '1' : '0');

        Write(&l_Online, sizeof(l_Online));
    }
    /// Web Handler
    /// @p_ClientPacket : Packet recieved from Web
    void GameSocket::HandleGetOnlineCount(ClientPacket* p_Packet)
    {
        uint32 l_Count = sWorldManager->GetPlayerCount();
        Write((const char*)&l_Count, sizeof(l_Count));
    }
    /// Web Handler
    /// @p_ClientPacket : Packet recieved from Web
    void GameSocket::HandleCanEquip(ClientPacket* p_Packet)
    {
        uint32 l_PlayerId = p_Packet->ReadUInt32();
        char l_CanEquip = '0';

        Entity::Player* l_Player = sWorldManager->FindPlayer(l_PlayerId);

        if (l_Player)
        {
            if (l_Player->GetEvent() == EventType::EVENT_TYPE_STATION && !l_Player->IsInCombat())
                l_CanEquip = '1';
        }


        Write(&l_CanEquip, sizeof(l_CanEquip));
    }
    /// Web Handler
    /// @p_ClientPacket : Packet recieved from Web
    void GameSocket::HandleUpdateInventory(ClientPacket* p_Packet)
    {
        uint32 l_PlayerId = p_Packet->ReadUInt32();

        Entity::Player* l_Player = sWorldManager->FindPlayer(l_PlayerId);

        if (l_Player)
        {
            if (l_Player->GetEvent() == EventType::EVENT_TYPE_STATION && !l_Player->IsInCombat())
            {
                /// Reload our inventory
                l_Player->GetInventory()->LoadInventory();
                l_Player->GetInventory()->CalculateStats();
                
                /// Send Packet
                /// TODO; Shields are int32 but packet struct is asking for uint32 - could be a possible overflow?
                /// TODO; Check if this packet is correct
                l_Player->SendPacket(Server::Packets::Login::PlayerInfo().Write(Server::Packets::Login::InfoType::INFO_TYPE_SET_SHIELD_HEALTH,
                    {
                       (uint32)l_Player->GetHitPoints(),
                       (uint32)l_Player->GetHitMaxPoints(),
                       (uint32)l_Player->GetShield(),
                       (uint32)l_Player->GetMaxShield(),
                    }));

            }
        }
    }
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone