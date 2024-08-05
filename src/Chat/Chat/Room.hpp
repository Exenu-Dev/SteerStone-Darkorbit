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

#pragma once
#include <PCH/Precompiled.hpp>
#include "Common.hpp"

namespace SteerStone { namespace Chat { namespace Entity {
	class Player;
} ///< namespace Entity
} ///< namespace Chat
} ///< namespace SteerStone

namespace SteerStone { namespace Chat { namespace Channel {

	class Room
	{
	public:
		/// Constructor
		/// @p_Id : Room Id
		/// @p_Name : Room Name
		/// @p_Company : Room Company
		/// @p_Type : Room Type
		/// @p_Owner : Room Owner
		Room(uint16 p_Id, std::string p_Name, int16 p_TabOrder, Company p_Company, RoomType p_Type, Entity::Player const* p_Owner = nullptr);
		/// Deconstructor
		~Room();

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

	public:
		/// Update Room
		/// @p_Diff : Execution Time
		void Update(uint32 p_Diff);

		/// Send Message to Room
		/// @p_Message : Message
		/// @p_Player : Player sending message
		void SendRoomMessage(std::string p_Message, Entity::Player const* p_Player);

		/// Add Player to Room
		/// @p_Player : Player to add
		void AddPlayer(Entity::Player* p_Player);

		/// Remove Player from Room
		/// @p_Player : Player to remove
		void RemovePlayer(Entity::Player* p_Player);

		/// Invite Player to Room
		/// @p_Player : Player to invite
		void InvitePlayer(Entity::Player* p_Player);

		/// Close Room
		void CloseRoom();

	public:
		///////////////////////////////////////////
		//            GETTERS/SETTERS
		///////////////////////////////////////////
		uint16 GetId()			const { return m_Id;		}
		std::string GetName()	const { return m_Name;		}
		int16 GetTabOrder()		const { return m_TabOrder;	}
		Company GetCompany()	const { return m_Company;	}
		RoomType GetType()		const { return m_Type;		}
		Entity::Player const* GetOwner() const { return m_Owner; }
		bool IsScheduledForDeletion() const { return m_ScheduledForDeletion; }
		void SetScheduledForDeletion(bool p_ScheduledForDeletion) { m_ScheduledForDeletion = p_ScheduledForDeletion; }
		std::map<uint64, Entity::Player*> GetRoomPlayers() const { return m_RoomPlayers; }
		bool IsPrivateRoom() const { return m_Type == RoomType::ROOM_TYPE_PRIVATE;}

		/// Check if player is in room
		/// @p_Player : Player to check
		bool IsInRoom(Entity::Player const* p_Player) const;

	private:
		uint16 m_Id;
		std::string m_Name;
		int16 m_TabOrder;
		Company m_Company;
		RoomType m_Type;
		Entity::Player const* m_Owner;
		bool m_ScheduledForDeletion;
		std::map<uint64, Entity::Player*> m_RoomPlayers;
	};

} ///< namespace Channel
} ///< namespace Chat
} ///< namespace SteerStone