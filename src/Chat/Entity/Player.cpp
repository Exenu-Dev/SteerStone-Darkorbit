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
#include "Player.hpp"
#include "Database/DatabaseTypes.hpp"
#include "Packets/Server/ChatPacket.hpp"

namespace SteerStone { namespace Chat { namespace Entity {

    /// Constructor
    /// @p_GameSocket : Socket
    Player::Player(Server::ChatSocket* p_ChatSocket) :
        m_Socket(p_ChatSocket ? p_ChatSocket->Shared<Server::ChatSocket>() : nullptr)
    {
        m_Id			= 0;
		m_Username		= "";
		m_Clan			= Clan();
		m_Rooms			= std::map<uint32, Chat::Channel::Room*>();
		m_CurrentRoom	= nullptr;
		m_CompanyId		= Company::NOMAD;
		m_AccessLevel	= AccessLevel::ACCESS_LEVEL_PLAYER;
		IntervalPing.SetInterval(PING_TIMER);
    }
    /// Deconstructor
    Player::~Player()
    {
		std::vector<uint32> l_RoomIds;

		for (auto l_Itr : m_Rooms)
			l_RoomIds.push_back(l_Itr.first);

		for (auto l_Itr : l_RoomIds)
			LeaveRoom(l_Itr);

		LOG_INFO("Chat", "Player %0 disconnected", m_Username);
    }

    ///////////////////////////////////////////
    //              GENERAL
    ///////////////////////////////////////////
	/// Load Player Info from database
    bool Player::LoadFromDB()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT username, company, `rank`, access_level FROM users WHERE id = ?");
        l_PreparedStatement->SetUint32(0, m_Id);
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

		if (l_PreparedResultSet)
		{
			Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

			m_Username	= l_Result[0].GetString();
			m_CompanyId = static_cast<Company>(l_Result[1].GetUInt16());
			m_RankId	= static_cast<Rank>(l_Result[2].GetUInt8());
			m_AccessLevel = static_cast<AccessLevel>(l_Result[3].GetUInt8());

			LoadClan();
			LoadRooms();

			return true;
		}
		return false;
    }

	/// Load Clan Info from database
	void Player::LoadClan()
	{
		m_Clan = Clan();

		Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
		l_PreparedStatement->PrepareStatement("SELECT clans.id, clans.name, clans.tag FROM clan_members INNER JOIN clans ON clans.id = clan_members.clan_id WHERE clan_members.user_id = ?");
		l_PreparedStatement->SetUint32(0, m_Id);
		std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

		if (l_PreparedResultSet)
		{
			Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

			m_Clan.Id	= l_Result[0].GetUInt32();
			m_Clan.Name = l_Result[1].GetString();
			m_Clan.Tag	= l_Result[2].GetString();
		}
	}
	/// Load Rooms
	void Player::LoadRooms()
	{
		std::map<uint32, Chat::Channel::Room*> l_Rooms = sChatManager->GetStandardRoomsBasedOnCompany(m_CompanyId);

		if (InClan())
		{
			Chat::Channel::Room* l_Room = sChatManager->GetRoomById(m_Clan.RoomId());

			if (!l_Room)
			{
				Chat::Channel::Room* l_Room = new Chat::Channel::Room(
					m_Clan.RoomId(),
					m_Clan.Tag,
					3,
					m_CompanyId,
					RoomType::ROOM_TYPE_CLAN
				);

				l_Rooms[l_Room->GetId()] = l_Room;
			}
		}
		else
			l_Rooms[RoomId::ROOM_ID_CLAN_SEARCH] = sChatManager->GetRoomById(RoomId::ROOM_ID_CLAN_SEARCH);

		// Add player to the rooms
		for (auto l_Itr : l_Rooms)
			AddRoom(l_Itr.second);
	}

	/// Check if player is in room
	///@ p_RoomId: Room Id to search for
	bool Player::IsInRoom(uint16 const p_RoomId)
	{
		auto l_Itr = m_Rooms.find(p_RoomId);
		if (l_Itr != m_Rooms.end())
			return true;

		return false;
	}

	/// Add Room
	/// @p_Room : Room to add
	void Player::AddRoom(Chat::Channel::Room* p_Room)
	{
		m_Rooms[p_Room->GetId()] = p_Room;

		switch (p_Room->GetType())
		{
			case RoomType::ROOM_TYPE_PRIVATE:
			{
				Server::Packets::PrivateRoomCreated l_PrivateRoomPacket;
				l_PrivateRoomPacket.RoomId = p_Room->GetId();
				l_PrivateRoomPacket.RoomName = p_Room->GetName();
				l_PrivateRoomPacket.OwnerId = p_Room->GetOwner()->GetId();
				l_PrivateRoomPacket.OwnerName = p_Room->GetOwner()->GetUsername();

				SendPacket(l_PrivateRoomPacket.Write());
			}
			break;
		}

		p_Room->AddPlayer(this);
	}

	/// Leave Room
	/// @p_RoomId : Room Id to leave
	void Player::LeaveRoom(uint16 p_RoomId)
	{
		auto l_Itr = m_Rooms.find(p_RoomId);
		if (l_Itr != m_Rooms.end())
		{
			if (l_Itr->second->GetOwner() == this)
			{
				l_Itr->second->CloseRoom();
				m_Rooms.erase(l_Itr);
				return;
			}

			Chat::Channel::Room* l_Room = l_Itr->second;
			l_Room->RemovePlayer(this);

			Server::Packets::DeleteRoom l_DeleteRoomPacket;
			l_DeleteRoomPacket.RoomId = l_Room->GetId();
			SendPacket(l_DeleteRoomPacket.Write());

			m_Rooms.erase(l_Itr);
		}
	}
	/// Close Room
	/// @p_RoomId : Room Id to close
	void Player::CloseRoom(uint16 p_RoomId)
	{
		auto l_Itr = m_Rooms.find(p_RoomId);
		if (l_Itr != m_Rooms.end())
		{
			Chat::Channel::Room* l_Room = l_Itr->second;
			if (l_Room->GetOwner() == this)
				l_Room->CloseRoom();
		}
	}
	/// Ignore Player
	/// @p_PlayerId : Player to ignore
	void Player::IgnorePlayer(Entity::Player const* p_PlayerId)
	{
		m_IgnoredPlayers.push_back(p_PlayerId->GetId());
	}

	/// Unignore Player
	/// @p_PlayerId : Player to unignore
	void Player::UnIgnorePlayer(Entity::Player const* p_PlayerId)
	{
		auto l_Itr = std::find(m_IgnoredPlayers.begin(), m_IgnoredPlayers.end(), p_PlayerId->GetId());
		if (l_Itr != m_IgnoredPlayers.end())
			m_IgnoredPlayers.erase(l_Itr);
	}

	///////////////////////////////////////////
	//              PACKETS
	///////////////////////////////////////////
	/// Send Packet
	/// @p_PacketBuffer : Packet Buffer
	void Player::SendPacket(Server::PacketBuffer const* p_PacketBuffer) const
	{
		if (!m_Socket || !p_PacketBuffer)
			return;

		m_Socket->SendPacket(p_PacketBuffer);
	}
	/// Send Login
	void Player::SendLogin()
	{
		Server::Packets::UserLogin l_UserLoginPacket;
		l_UserLoginPacket.UserId = m_Id;

		SendPacket(l_UserLoginPacket.Write());
	}
	/// Send Ping
	void Player::SendPing()
	{
		Server::PacketBuffer l_PacketBuffer;
		l_PacketBuffer.AppendChar("");
	}
	/// Whisper the player
	/// @p_Player : Player to whisper
	/// @p_Message : Message to send
	void Player::Whisper(Entity::Player const* p_Player, std::string const p_Message) const
	{
		Server::Packets::YouWhisper l_YouWhisperPacket;
		l_YouWhisperPacket.Username = p_Player->GetUsername();
		l_YouWhisperPacket.Message = p_Message;
		SendPacket(l_YouWhisperPacket.Write());

		Server::Packets::UserWhispers l_WhisperPacket;
		l_WhisperPacket.Username = m_Username;
		l_WhisperPacket.Message = p_Message;
		p_Player->SendPacket(l_WhisperPacket.Write());
	}
	/// Send Rooms
	void Player::SendRooms() const
	{
		Server::Packets::SetRoomList l_RoomListPacket;

		for (auto l_Itr : m_Rooms)
		{
			auto l_Room = l_Itr.second;

			l_RoomListPacket.Rooms.push_back(Server::Packets::Room(
				l_Room->GetId(),
				l_Room->GetName(),
				l_Room->GetTabOrder(),
				l_Room->GetCompany(),
				l_Room->GetType(),
			0));
		}

		SendPacket(l_RoomListPacket.Write());
	}
	/// Send Message to self
	/// This uses the system message packet
	/// @p_Message : Message to be sent to the player
	void Player::SendMessageToSelf(std::string const p_Message) const
	{
		Server::Packets::SendAdminMessage l_SystemMessagePacket;
		l_SystemMessagePacket.Message = p_Message;
		SendPacket(l_SystemMessagePacket.Write());
	}
	/// Get Username for Display
	std::string Player::GetUsernameForDisplay() const
	{
		if (InClan())
			return "[" + m_Clan.Tag + "] " + m_Username;

		return m_Username;
	}
	/// Get Room
	/// @p_RoomId : Room Id
	Chat::Channel::Room* Player::GetRoom(uint16 p_RoomId) const
	{
		auto l_Itr = m_Rooms.find(p_RoomId);
		if (l_Itr != m_Rooms.end())
			return l_Itr->second;

		return nullptr;
	}
}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone