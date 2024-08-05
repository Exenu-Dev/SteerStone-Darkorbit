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
#include "Server/QuestPackets.hpp"
#include "GameFlags.hpp"

namespace SteerStone { namespace Game { namespace Server {

    /// Quest Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleQuestHUD(ClientPacket* p_Packet)
    {
        std::string l_Type = p_Packet->ReadString();

        if (l_Type == "p")
        {
            int32 l_QuestId = p_Packet->ReadInt32();

            /// TODO: Check if this player has the quest

            Server::Packets::Quest::QuestHud l_Packet;
            l_Packet.Type = Server::Packets::Quest::QuestHudTypes::QUEST_HUD__TYPE_PRIVILEGE;
            Server::Packets::Quest::UpdatePrivilege l_UpdatePrivilege;
            l_UpdatePrivilege.QuestId = l_QuestId;

            l_Packet.Privilege = l_UpdatePrivilege;

            m_Player->SendPacket(l_Packet.Write());
        }
    }
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone