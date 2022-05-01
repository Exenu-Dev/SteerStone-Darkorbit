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

#include "Player.hpp"
#include "Database/DatabaseTypes.hpp"

namespace SteerStone { namespace Chat { namespace Entity {

    /// Constructor
    /// @p_GameSocket : Socket
    Player::Player(Server::ChatSocket* p_ChatSocket) :
        m_Socket(p_ChatSocket ? p_ChatSocket->Shared<Server::ChatSocket>() : nullptr)
    {
        m_Id        = 0;
		m_RoomIds   = {};
		m_Username  = "";
		m_CompanyId = Company::NOMAD;
    }
    /// Deconstructor
    Player::~Player()
    {
    }

    ///////////////////////////////////////////
    //              GENERAL
    ///////////////////////////////////////////
	/// Load Player Info from database
    bool Player::LoadFromDB()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT username, company FROM users WHERE id = ?");
        l_PreparedStatement->SetUint32(0, m_Id);
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

		if (l_PreparedResultSet)
		{
			Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

			m_Username	= l_Result[0].GetString();
			m_CompanyId = static_cast<Company>(l_Result[1].GetUInt16());

			Server::PacketBuffer l_PacketBuffer;

			switch (m_CompanyId)
			{
			case Company::EARTH:
			{
				l_PacketBuffer.AppendChar("by%1|Global|1|-1|0|0}3|EIC|2|-1|0|0}5|Clan Search|3|-1|0|0#");
				m_RoomIds.push_back(3);
			}
			break;
			case Company::MMO:
			{
				l_PacketBuffer.AppendChar("by%1|Global|1|-1|0|0}2|MMO|2|-1|0|0}5|Clan Search|3|-1|0|0#");
				m_RoomIds.push_back(2);
			}
			break;
			case Company::VRU:
			{
				l_PacketBuffer.AppendChar("by%1|Global|1|-1|0|0}4|VRU|2|-1|0|0}5|Clan Search|3|-1|0|0#");
				m_RoomIds.push_back(4);
			}
			break;
			default:
			{
				LOG_WARNING("Chat", "Failed to find company %0", static_cast<uint16>(m_CompanyId));
			}
			break;
			}

			m_RoomIds.push_back(1);
			m_RoomIds.push_back(5);
			l_PacketBuffer.AppendEndSplitter();

			SendPacket(&l_PacketBuffer);

			return true;
		}
		return false;
    }

	/// Check if player is in room
	///@ p_RoomId: Room Id to search for
	bool Player::IsInRoom(uint16 const p_RoomId)
	{
		return std::find(m_RoomIds.begin(), m_RoomIds.end(), p_RoomId) != m_RoomIds.end();
	}

	///////////////////////////////////////////
	//              PACKETS
	///////////////////////////////////////////
	/// Send Packet
	/// @p_PacketBuffer : Packet Buffer
	void Player::SendPacket(Server::PacketBuffer const* p_PacketBuffer)
	{
		if (!m_Socket || !p_PacketBuffer)
			return;

		m_Socket->SendPacket(p_PacketBuffer);
	}
	/// Send Ping
	void Player::SendPing()
	{
		Server::PacketBuffer l_PacketBuffer;
		l_PacketBuffer.AppendChar("");
	}
}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone