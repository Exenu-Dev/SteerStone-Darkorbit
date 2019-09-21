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

#include "Server/MiscPackets.hpp"
#include "Socket.hpp"
#include "Portal.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "GameFlags.hpp"

namespace SteerStone { namespace Game { namespace Server {

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleObjectMove(ClientPacket* p_Packet)
    {
        float l_NewPositionX = static_cast<float>(p_Packet->ReadUInt32());
        float l_NewPositionY = static_cast<float>(p_Packet->ReadUInt32());

        /// The client sends the current position - but we should never trust the client
        /// So skip this
        float l_PrevPositionX = static_cast<float>(p_Packet->ReadUInt32());
        float l_PrevPositionY = static_cast<float>(p_Packet->ReadUInt32());

        m_Player->GetSpline()->Move(l_NewPositionX, l_NewPositionY, 0, 0);


    }
    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleInitializePlayer(ClientPacket* p_Packet)
    {
        uint32 l_Id = p_Packet->ReadUInt32();

        /// Attempt to find player in grid
        Entity::Object* l_Player = m_Player->GetMap()->GetGrid(m_Player)->FindPlayer(l_Id);

        /// If not find in world
        if (!l_Player)
            l_Player = sWorldManager->FindPlayer(l_Id);
        
        LOG_ASSERT(l_Player, "World", "Cannot find player Id: %0", l_Id);

        m_Player->SendPacket(&m_Player->GetMap()->GetGrid(m_Player)->BuildPlayerSpawn(l_Player, m_Player));
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandlePortalJump(ClientPacket* p_Packet)
    {
        Entity::Portal* l_Portal = m_Player->GetGrid()->CanJumpPortal(m_Player);

        if (l_Portal)
        {
            ;
        }
        else
        {
            Packets::Message l_Packet;
            l_Packet.Type = "STD";
            l_Packet.Text = "You are not near a portal!";
            SendPacket(l_Packet.Write());
        }
    }
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone