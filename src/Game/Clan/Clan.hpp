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
#include "Core/Core.hpp"

namespace SteerStone { namespace Game { namespace Global {
    class ClanManager;
} ///< namespace Global
} ///< namespace Game
} ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Clan {

	class Player;

	class Base
	{
		DISALLOW_COPY_AND_ASSIGN(Base);

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		friend class Global::ClanManager;

	public:
		Base();
		/// Deconstructor
		~Base();

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

	public:
		/// Load Clan Members from Database
		void LoadMembersFromDB();

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////
		//            GETTERS/SETTERS
		///////////////////////////////////////////
	public:
		uint32 GetID()				{ return m_ID;		}
		std::string GetName() const { return m_Name;	}
		std::string GetTag() const	{ return m_Tag;		}

		std::vector<uint32> GetMemberIds() const { return m_MemberIds; }

		///////////////////////////////////////////
		//            VARIABLES
		///////////////////////////////////////////
	private:
		uint32 m_ID;			///< Clan ID
		std::string m_Name;		///< Clan name
		std::string m_Tag;		///< Clan tag

		std::vector<uint32> m_MemberIds; ///< Clan members
	};

} ///< Clan
} ///< Game
} ///< SteerStone