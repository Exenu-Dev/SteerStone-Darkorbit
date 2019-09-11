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

namespace SteerStone { namespace Game { namespace Server {

    /// Login Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleLogin(ClientPacket* p_Packet)
    {
        m_Player                = new Entity::Player(this);
        m_Player->m_Id          = p_Packet->ReadInt32();
        m_Player->m_SessionId   = p_Packet->ReadString(); ///< Session Id gets updated to database when session ends
        if (!m_Player->LoadFromDB())
        {
            LOG_WARNING("Player", "Failed to load player details from database. User Id: %0", m_Player->GetId());
            CloseSocket(); ///< Close socket, nothing we can do without having player stored info
        }
        else
        {
            m_Player->SendClientSettings();
        }

    }

}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone