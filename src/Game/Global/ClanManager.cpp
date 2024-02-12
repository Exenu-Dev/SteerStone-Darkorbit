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

#include "World.hpp"
#include "Diagnostic/DiaServerWatch.hpp"
#include "Database/DatabaseTypes.hpp"
#include "ClanManager.hpp"

namespace SteerStone { namespace Game { namespace Global {

    SINGLETON_P_I(ClanManager);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    ClanManager::ClanManager() : m_Clans()
    {
        /// Create Mob Clan for mobs
        Clan::Base* l_MobClan = new Clan::Base();
        l_MobClan->m_ID = 0;
        l_MobClan->m_Name = "";
        l_MobClan->m_Tag = "";

        m_MobClan = l_MobClan;
    }
    /// Deconstructor
    ClanManager::~ClanManager()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Load Clans from Database
    void ClanManager::LoadFromDB()
    {
        uint32 l_StartTime = sServerTimeManager->GetServerTime();

        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT id, name, tag FROM clans");

        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        uint32 l_Counter = 0;

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                uint32 l_Id         = l_Result[0].GetUInt32();
                std::string l_Name  = l_Result[1].GetString();
                std::string l_Tag   = l_Result[2].GetString();

                /// If Clan already exists in memory, update it
                if (Clan::Base* l_Clan = GetClanById(l_Id))
                {
                    l_Clan->m_ID    = l_Id;
                    l_Clan->m_Name  = l_Name;
                    l_Clan->m_Tag   = l_Tag;

                    l_Clan->LoadMembersFromDB();
				}
                else
                {
                    Clan::Base* l_NewClan = new Clan::Base();
                    l_NewClan->m_ID     = l_Id;
                    l_NewClan->m_Name   = l_Name;
                    l_NewClan->m_Tag    = l_Tag;

                    l_NewClan->LoadMembersFromDB();

                    m_Clans.push_back(l_NewClan);
                }

                l_Counter++;

            } while (l_PreparedResultSet->GetNextRow());
        }

        LOG_INFO("ClanManager", "Loaded %0 Clans in %1 ms", l_Counter, sServerTimeManager->GetTimeDifference(l_StartTime, sServerTimeManager->GetServerTime()));
	}

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Find Clan by Player ID
    /// @p_PlayerId : Player ID
    Clan::Base* ClanManager::GetClanByPlayerId(uint64 const p_PlayerId)
    {
        for (auto l_Clan : m_Clans)
        {
            for (auto l_MemberId : l_Clan->GetMemberIds())
            {
				if (l_MemberId == p_PlayerId)
					return l_Clan;
			}
		}

        return nullptr;
    }

    /// Find Clan by Clan ID
    /// @p_ClanId : Clan ID
    Clan::Base* ClanManager::GetClanById(uint64 const p_ClanId)
    {
        for (auto l_Clan : m_Clans)
        {
			if (l_Clan->GetID() == p_ClanId)
				return l_Clan;
		}
    }

    /// Return Empty Clan
    Clan::Base* ClanManager::MobClan()
    {
		return m_MobClan;
	}
}   ///< namespace Global
}   ///< namespace Game
}   ///< namespace Steerstone
