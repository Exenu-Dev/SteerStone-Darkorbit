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

#include "Socket.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "GameFlags.hpp"

namespace SteerStone { namespace Game { namespace Server {

    /// Debug Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleDebugInitializePlayer(ClientPacket* p_Packet)
    {
        /// Attempt to find player in grid
        /*Entity::Object* l_Player = m_Player->GetMap()->GetGrid(m_Player)->FindPlayer(l_Id);

        /// If not find in world
        if (!l_Player)
            l_Player = sWorldManager->FindPlayer(l_Id);
        
        LOG_ASSERT(l_Player, "World", "Cannot find player Id: %0", l_Id);

        m_Player->SendPacket(&m_Player->GetMap()->GetGrid(m_Player)->BuildPlayerSpawn(l_Player, m_Player));*/
    }
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone