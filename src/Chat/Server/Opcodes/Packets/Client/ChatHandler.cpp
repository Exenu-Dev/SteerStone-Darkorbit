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

#include "Packets/Server/ChatPacket.hpp"
#include "Database/DatabaseTypes.hpp"
#include "Socket.hpp"
#include "ChatManager.hpp"
#include "Common.hpp"

namespace SteerStone { namespace Chat { namespace Server {

	void ChatSocket::HandleLogin(ClientPacket* p_Packet)
	{
		uint32 l_UserId = p_Packet->ReadUInt32();

		Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
		l_PreparedStatement->PrepareStatement("SELECT username, company FROM users WHERE id = ?");
		l_PreparedStatement->SetUint32(0, l_UserId);
		std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

		if (l_PreparedResultSet)
		{
			Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

			std::string l_Username = l_Result[0].GetString();

			uint16 l_CompanyId = l_Result[1].GetUInt16();
			Company l_Company = static_cast<Company>(l_CompanyId);
			std::vector<uint16> l_RoomIds;

			PacketBuffer l_PacketBuffer;

			switch (l_Company)
			{
				case Company::EARTH:
				{
					l_PacketBuffer.AppendChar("by%1|Global|1|-1|0|0}3|EIC|2|-1|0|0}5|Clan Search|3|-1|0|0#");
					l_RoomIds.push_back(3);
				}
				break;
				case Company::MMO:
				{
					l_PacketBuffer.AppendChar("by%1|Global|1|-1|0|0}2|MMO|2|-1|0|0}5|Clan Search|3|-1|0|0#");
					l_RoomIds.push_back(2);
				}
				break;
				case Company::VRU:
				{
					l_PacketBuffer.AppendChar("by%1|Global|1|-1|0|0}4|VRU|2|-1|0|0}5|Clan Search|3|-1|0|0#");
					l_RoomIds.push_back(4);
				}
				break;
				default:
				{
					LOG_WARNING("Chat", "Failed to find company %0", l_CompanyId);
				}
				break;
			}

			l_RoomIds.push_back(1);
			l_RoomIds.push_back(5);

			sChatManager->AddUser(l_UserId, l_Username, l_RoomIds);

			SendPacket(&l_PacketBuffer);
		}
	}
}   ///< namespace Server
}   ///< namespace Chat
}   ///< namespace SteerStone