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

		if (m_Player->LoadFromDB())
			sChatManager->AddPlayer(m_Player);
		else
		{
			LOG_WARNING("Player", "Failed to initialize player %0", l_UserId);

			/// Close the socket, because we can't do anything if we cannot load
			/// the player
			CloseSocket();
		}
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

		/// Check if it's a command
		if (l_Message[0] == '/') {
			std::string l_Command = l_Message.erase(0, 1);

			/// Check what sort of command it is
			/// If it's a command we can process here e.g system message then we can do it here
			/// else if it's a command to save player to database, then the game server needs to do it

			if (boost::algorithm::contains(l_Command, "announce"))
			{
				std::vector<std::string> l_Splitter = Core::Utils::SplitAll(l_Command, " ", false);

				l_Splitter.erase(l_Splitter.begin());

				std::string l_AnnounceMessage = boost::algorithm::join(l_Splitter, " ");

				sChatManager->SendSystemMessage(l_AnnounceMessage);
			}
			else if (l_Command == "users")
			{
				Server::Packets::UserCount l_Packet;
				l_Packet.Total = sChatManager->TotalPlayers();
				m_Player->SendPacket(l_Packet.Write());
			}
			else
				sChatManager->AddProcessCommand(m_Player->GetId(), l_Command);

			return;
		}

		sChatManager->SendMessageToRoom(m_Player, l_Message, l_RoomId);
	}
}   ///< namespace Server
}   ///< namespace Chat
}   ///< namespace SteerStone