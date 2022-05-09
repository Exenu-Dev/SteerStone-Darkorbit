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

#include "ChatManager.hpp"

namespace SteerStone { namespace Chat { namespace Server {

	/// Handle Login Handler
	/// @p_ClientPacket : Packet recieved from client
	void ChatSocket::HandleLogin(ClientPacket* p_Packet)
	{
		// Skip first two
		p_Packet->ReadSkip();
		p_Packet->ReadSkip();

		uint32 l_UserId = p_Packet->ReadUInt32();

		m_Player = new Entity::Player(this);
		m_Player->m_Id = l_UserId;

		if (m_Player->LoadFromDB())
			sChatManager->AddPlayer(m_Player);
		else
			LOG_WARNING("Player", "Failed to initialize player %0", l_UserId);
	}
	/// Handle Send Message
	/// @p_ClientPacket : Packet recieved from client
	void ChatSocket::HandleSendMessage(ClientPacket* p_Packet)
	{
		uint16 l_RoomId = p_Packet->ReadUInt16();
		std::string l_Message = p_Packet->ReadString();

		sChatManager->SendMessageToRoom(m_Player, l_Message, l_RoomId);
	}
}   ///< namespace Server
}   ///< namespace Chat
}   ///< namespace SteerStone