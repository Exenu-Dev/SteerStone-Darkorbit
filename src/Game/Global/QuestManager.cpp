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

#include "Diagnostic/DiaServerWatch.hpp"
#include "Database/DatabaseTypes.hpp"
#include "QuestManager.hpp"

namespace SteerStone { namespace Game { namespace Global {

    SINGLETON_P_I(QuestManager);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    QuestManager::QuestManager()
    {

    }
    /// Deconstructor
    QuestManager::~QuestManager()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Load Quests
    void QuestManager::LoadQuests()
    {
        uint32 l_StartTime = sServerTimeManager->GetServerTime();
        uint32 l_Counter = 0;

        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT id, title, description, overview, sort_order FROM quest_templates");

        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                Game::Quest::Quest* l_Quest = new Game::Quest::Quest();
                l_Quest->m_Id = l_Result[0].GetUInt32();
                l_Quest->m_Title = l_Result[1].GetString();
                l_Quest->m_Description = l_Result[2].GetString();
                l_Quest->m_Overview = l_Result[3].GetString();
                l_Quest->m_Order = l_Result[4].GetBool();
                l_Quest->LoadFromDB();

                m_Quest[l_Quest->m_Id] = l_Quest;

                l_Counter++;

            } while (l_PreparedResultSet->GetNextRow());
        }

        LOG_INFO("QuestManager", "Loaded %0 Quests in %1 ms", l_Counter, sServerTimeManager->GetTimeDifference(l_StartTime, sServerTimeManager->GetServerTime()));
    }
    /// Get the quest by Id
    /// @p_Id : The quest Id
    Game::Quest::Quest* const QuestManager::GetQuestById(uint32 const p_Id) const
    {
        auto l_Itr = m_Quest.find(p_Id);
        if (l_Itr != m_Quest.end())
            return l_Itr->second;

        return nullptr;
    }
}   ///< namespace Global
}   ///< namespace Game
}   ///< namespace Steerstone
