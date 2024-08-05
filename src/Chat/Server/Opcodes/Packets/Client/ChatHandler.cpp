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

#include <boost/algorithm/string.hpp>

#include "ChatManager.hpp"
#include "Packets/Server/ChatPacket.hpp"

namespace SteerStone { namespace Chat { namespace Server {

	/// Handle Login Handler
	/// @p_ClientPacket : Packet recieved from client
	void ChatSocket::HandleLogin(ClientPacket* p_Packet)
	{
		// Skip first two
		// Not sure what these are
		p_Packet->ReadSkip();
		p_Packet->ReadSkip();

		uint32 l_UserId = p_Packet->ReadUInt32();

		m_Player = new Entity::Player(this);
		m_Player->m_Id = l_UserId;

		sChatManager->PlayerIsBanned(m_Player);

		if (m_Player->LoadFromDB())
		{
			sChatManager->AddPlayer(m_Player);

			m_Player->SendLogin();
			m_Player->SendRooms();

			Server::Packets::DeveloperMessage l_SystemMessagePacket;
			l_SystemMessagePacket.Message = "\n<span class='mod'>Welcome to <a class='highlight' target=\"_blank\" href=\"http://www.github.com/LiamAshdown/Darkorbit-Emulator\">Darkorbit Emulator</a> made by <a class='highlight' target=\"_blank\" href=\"http://www.github.com/LiamAshdown\">Quadral.</a>\n</span>";
			SendPacket(l_SystemMessagePacket.Write());
		}
		else
		{
			LOG_WARNING("Player", "Failed to initialize player %0", l_UserId);

			/// Close the socket, because we can't do anything if we cannot load
			/// the player
			CloseSocket();
		}
	}
	/// Handle Pong Handler
	/// @p_ClientPacket : Packet recieved from client
	void ChatSocket::HandlePong(ClientPacket* p_Packet)
	{
		Server::Packets::Ping l_Ping;
		SendPacket(l_Ping.Write());
	}
	/// Handle Send Message
	/// @p_ClientPacket : Packet recieved from client
	void ChatSocket::HandleSendMessage(ClientPacket* p_Packet)
	{
		uint16 l_RoomId = p_Packet->ReadUInt16();
		std::string l_Message = p_Packet->ReadString();

		if (!l_Message.length()) 
		{
			LOG_WARNING("Chat", "Received empty message from %0", m_Player->GetId());
			return;
		}

		if (sChatManager->PlayerIsBanned(m_Player))
			return;

		/// Check if it's a command
		if (l_Message[0] == '/') {
			std::string l_Command = l_Message.erase(0, 1);

			sChatManager->ProcessCommand(l_Message, m_Player);

			return;
		}

		Chat::Channel::Room* l_Room = m_Player->GetRoom(l_RoomId);
		if (!l_Room)
		{
			LOG_WARNING("Chat", "Player %0 is not in room %1", m_Player->GetId(), l_RoomId);
			return;
		}

		l_Room->SendRoomMessage(l_Message, m_Player);
	}
	/// Handle Change Room
	/// @p_ClientPacket : Packet recieved from client
	void ChatSocket::HandleChangeRoom(ClientPacket* p_Packet)
	{
		p_Packet->ReadSkip(); ///< Not sure what this is
		uint16 l_RoomId = p_Packet->ReadUInt16();

		Chat::Channel::Room* l_Room = sChatManager->GetRoomById(l_RoomId);
		if (!l_Room)
		{
			LOG_WARNING("Chat", "Player %0 tried to join room %1, but room does not exist", m_Player->GetId(), l_RoomId);
			return;
		}

		if (!m_Player->IsInRoom(l_RoomId))
		{
			LOG_WARNING("Chat", "Player %0 tried to join room %1, but player is not in room", m_Player->GetId(), l_RoomId);
			return;
		}

		m_Player->m_CurrentRoom = l_Room;
	}
}   ///< namespace Server
}   ///< namespace Chat
}   ///< namespace SteerStone