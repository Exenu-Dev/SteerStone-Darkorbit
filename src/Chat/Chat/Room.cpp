/*
* Liam Ashdown
* Copyright (C) 2024
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

#include "Room.hpp"
#include "Player.hpp"

namespace SteerStone { namespace Chat { namespace Channel {

	/// Constructor
	/// @p_Id : Room Id
	/// @p_Name : Room Name
	/// @p_Company : Room Company
	/// @p_Type : Room Type
	/// @p_Owner : Room Owner
	Room::Room(uint16 p_Id, std::string p_Name, int16 p_TabOrder, Company p_Company, RoomType p_Type, Entity::Player const* p_Owner/*= nullptr*/)
	{
		m_Id		= p_Id;
		m_Name		= p_Name;
		m_Company	= p_Company;
		m_Type		= p_Type;
		m_TabOrder  = p_TabOrder;
		m_Owner		= p_Owner;
		m_ScheduledForDeletion = false;
	}

	/// Deconstructor
	Room::~Room()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/// Update Room
	/// @p_Diff : Execution Time
	void Room::Update(uint32 p_Diff)
	{
		/// Does nothing at the moment
	}

	/// Send Message to Room
	/// @p_Message : Message
	/// @p_Player : Player sending message
	void Room::SendRoomMessage(std::string p_Message, Entity::Player const* p_Player)
	{
		for (auto l_Itr : m_RoomPlayers)
		{
			if (l_Itr.second->IsIgnored(p_Player->GetId()))
				continue;

			switch (p_Player->GetAccessLevel())
			{
				case AccessLevel::ACCESS_LEVEL_MODERATOR:
				case AccessLevel::ACCESS_LEVEL_PLAYER:
				{
					Server::Packets::SendMessageToRoom l_Packet;
					l_Packet.RoomId = m_Id;
					l_Packet.Username = p_Player->GetUsernameForDisplay();
					l_Packet.Message = p_Message;
					l_Itr.second->SendPacket(l_Packet.Write());
				}
				break;
				case AccessLevel::ACCESS_LEVEL_ADMIN:
				{
					Server::Packets::SendAdminMessage l_Packet;
					l_Packet.RoomId = m_Id;
					l_Packet.Username = p_Player->GetUsernameForDisplay();
					l_Packet.Message = p_Message;
					l_Itr.second->SendPacket(l_Packet.Write());
				}
				break;
				case AccessLevel::ACCESS_LEVEL_DEVELOPER:
				{
					Server::Packets::DeveloperMessage l_Packet;
					l_Packet.Message = p_Message;
					l_Itr.second->SendPacket(l_Packet.Write());
				}
				break;
			}
		}
	}
	/// Add Player to Room
	/// @p_Player : Player to add
	void Room::AddPlayer(Entity::Player* p_Player)
	{
		m_RoomPlayers[p_Player->GetId()] = p_Player;
	}
	/// Remove Player from Room
	/// @p_Player : Player to remove
	void Room::RemovePlayer(Entity::Player* p_Player)
	{
		auto l_Itr = m_RoomPlayers.find(p_Player->GetId());
		if (l_Itr != m_RoomPlayers.end())
			m_RoomPlayers.erase(l_Itr);

		if (m_Type == RoomType::ROOM_TYPE_PRIVATE)
		{
			Server::Packets::UserLeftInvitedRoom l_UserLeftRoomPacket;
			l_UserLeftRoomPacket.RoomId = GetId();
			l_UserLeftRoomPacket.UserId = p_Player->GetId();
			l_UserLeftRoomPacket.Username = p_Player->GetUsername();

			for (auto l_Itr : GetRoomPlayers())
				l_Itr.second->SendPacket(l_UserLeftRoomPacket.Write());
		}
	}
	/// Invite Player to Room
	/// @p_Player : Player to invite
	void Room::InvitePlayer(Entity::Player* p_Player)
	{
		Server::Packets::UserJoinedInvitedRoom l_Packet;
		l_Packet.Username = p_Player->GetUsername();

		for (auto l_Itr : m_RoomPlayers)
		{
			if (l_Itr.second->GetId() != p_Player->GetId())
				l_Itr.second->SendPacket(l_Packet.Write());
		}

		p_Player->AddRoom(this);
	}
	/// Close Room
	void Room::CloseRoom()
	{
		for (auto l_Itr : m_RoomPlayers)
		{
			Server::Packets::DeleteRoom l_DeleteRoomPacket;
			l_DeleteRoomPacket.RoomId = m_Id;
			l_Itr.second->SendPacket(l_DeleteRoomPacket.Write());
		}

		m_RoomPlayers.clear();

		m_ScheduledForDeletion = true;
	}
	/// Check if player is in room
	/// @p_Player : Player to check
	bool Room::IsInRoom(Entity::Player const* p_Player) const
	{
		auto l_Itr = m_RoomPlayers.find(p_Player->GetId());
		return l_Itr != m_RoomPlayers.end();
	
	}
} ///< namespace Channel
} ///< namespace Chat
} ///< namespace SteerStone