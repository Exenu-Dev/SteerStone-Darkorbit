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

#include "Player.hpp"
#include "ClanManager.hpp"
#include "World.hpp"

namespace SteerStone { namespace Game { namespace Clan {

	/// Constructor
	Base::Base()
	{
	}
	/// Deconstructor
	Base::~Base()
	{
	}
 
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/// Load Clan Members from Database
	void Base::LoadMembersFromDB()
	{
		Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
		l_PreparedStatement->PrepareStatement("SELECT user_id FROM clan_members WHERE clan_id = ?");

		m_MemberIds.clear();

		l_PreparedStatement->SetUint64(0, m_ID);

		std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

		if (l_PreparedResultSet)
		{
			do
			{
				Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

				uint64 l_MemberId = l_Result[0].GetUInt64();

				if (Entity::Player* l_Player = sWorldManager->FindPlayer(l_MemberId))
				{
					l_Player->SetClan(this);
				}

				m_MemberIds.push_back(l_MemberId);
			} while (l_PreparedResultSet->GetNextRow());
		}
	} 

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

} ///< Clan
} ///< Game
} ///< SteerStone