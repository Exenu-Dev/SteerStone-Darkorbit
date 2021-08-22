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
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone