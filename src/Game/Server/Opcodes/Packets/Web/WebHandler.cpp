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

#include "Socket.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "GameFlags.hpp"
#include "Server/LoginPackets.hpp"
#include "ObjectManager.hpp"
#include "ClanManager.hpp"

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

        char l_IsOnline = l_Player ? '1' : '0';

        Write(&l_IsOnline, sizeof(l_IsOnline));
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
                /// TODO; Max Shields/Health are uint32 but packet struct is asking for int32 - could be a possible overflow?
                /// TODO; Check if this packet is correct
                l_Player->SendPacket(Server::Packets::Login::PlayerInfo().Write(Server::Packets::Login::InfoType::INFO_TYPE_SET_SHIELD_HEALTH,
                    {
                       l_Player->GetHitPoints(),
                       (int32)l_Player->GetHitMaxPoints(),
                       l_Player->GetShield(),
                       (int32)l_Player->GetMaxShield(),
                    }));

            }
        }
    }
    /// Web Handler
    /// @p_ClientPacket : Packet recieved from Web
    void GameSocket::HandleUpdateShip(ClientPacket* p_Packet)
    {
        uint32 l_PlayerId = p_Packet->ReadUInt32();

        Entity::Player* l_Player = sWorldManager->FindPlayer(l_PlayerId);
        char l_CanEquip = '0';

        if (l_Player)
        {
            if (l_Player->GetEvent() == EventType::EVENT_TYPE_STATION && !l_Player->IsInCombat())
            {
                uint32 l_EntryId = p_Packet->ReadUInt32();

                const Entity::ShipTemplate* l_ShipTemplate = sObjectManager->GetShipTemplate(l_EntryId);

                if (l_ShipTemplate)
                {
                    int32 l_CargoSpaceGained = l_ShipTemplate->Cargo - l_Player->m_MaxCargoSpace;

                    l_Player->m_HitPoints = l_ShipTemplate->HitPoints;
                    l_Player->m_ShipType = l_ShipTemplate->Entry;
                    l_Player->m_MaxRockets = l_ShipTemplate->Rockets;
                    l_Player->m_MaxCargoSpace = l_ShipTemplate->Cargo;
                    l_Player->m_CargoSpace += l_CargoSpaceGained;

                    l_Player->SaveShipToDB();
                    l_Player->SendInitializeShip();
                    l_Player->SendMapUpdate();
                    l_Player->GetMap()->SendConstantObjects(l_Player);

                    l_CanEquip = '1';
                }

                // Also update the drones
                l_Player->SendDrones();
            }
        }

        Write(&l_CanEquip, sizeof(l_CanEquip));
    }
    /// Web Handler
    /// @p_ClientPacket : Packet recieved from Web
    void GameSocket::HandleUpdateDrones(ClientPacket* p_Packet)
    {
		uint32 l_PlayerId = p_Packet->ReadUInt32();

		Entity::Player* l_Player = sWorldManager->FindPlayer(l_PlayerId);
		char l_CanEquip = '1';

        if (l_Player)
            if (l_Player->GetEvent() == EventType::EVENT_TYPE_STATION && !l_Player->IsInCombat())
				l_Player->SendDrones();

		Write(&l_CanEquip, sizeof(l_CanEquip));
	}
    void GameSocket::HandleBroughtAmmo(ClientPacket* p_Packet)
    {
        uint32 l_PlayerId = p_Packet->ReadUInt32();

        Entity::Player* l_Player = sWorldManager->FindPlayer(l_PlayerId);
        PacketBuffer l_PacketBuffer;
        l_PacketBuffer.AppendChar("Error");
        if (l_Player)
        {
            uint32 l_EntryId = p_Packet->ReadUInt32();
            uint32 l_Amount = p_Packet->ReadUInt32();
            const Entity::ItemTemplate* l_ItemTemplate = sObjectManager->GetItemTemplate(l_EntryId);

            if (l_ItemTemplate) {
                // TODO; Need a better method of detecting what ammo is being updated
                // As checking by name is not a good idea as this could change
                if (l_ItemTemplate->Name == "LCB-10")
                    l_Player->SetBatteryAmmo(BatteryType::BATTERY_TYPE_LCB10, l_Amount);
                else if (l_ItemTemplate->Name == "MCB-25")
                    l_Player->SetBatteryAmmo(BatteryType::BATTERY_TYPE_MCB25, l_Amount);
                else if (l_ItemTemplate->Name == "MCB-50")
                    l_Player->SetBatteryAmmo(BatteryType::BATTERY_TYPE_MCB50, l_Amount);
                else if (l_ItemTemplate->Name == "SAB-50")
                    l_Player->SetBatteryAmmo(BatteryType::BATTERY_TYPE_SAB50, l_Amount);
            }
            else {
                LOG_WARNING("Web", "Failed to find Item Template for %0", l_EntryId);
            }
        }

        Write((char*)l_PacketBuffer.GetContents(), l_PacketBuffer.GetSize(), true);
    }
    /// Web Handler
    /// @p_ClientPacket : Packet recieved from Web
    /// Web Handler
    /// @p_ClientPacket : Packet recieved from Web
    void GameSocket::HandleUpdateClans(ClientPacket* p_Packet)
    {
        sClanManager->LoadFromDB();

        nlohmann::json l_Json;
        l_Json["status"] = "success";

        Write(l_Json.dump().c_str(), l_Json.dump().length(), true);
	}
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone