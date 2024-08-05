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

#pragma once
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"
#include "Singleton/Singleton.hpp"
#include "Quest/Quest.hpp"
#include "GameFlags.hpp"

namespace SteerStone { namespace Game { namespace Quest {
    class Quest;
} ///< namespace Quest
} ///< namespace Game
} ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Global {
    typedef std::unordered_map<uint32, Game::Quest::Quest*> QuestMap;

    /// Quest Manager
    class QuestManager
    {
        SINGLETON_P_D(QuestManager);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Load Quests
        void LoadQuests();

        /// Get the quest by Id
        /// @p_Id : The quest Id
        Game::Quest::Quest* const GetQuestById(uint32 const p_Id) const;

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        /// Quest Map
		QuestMap m_Quest;
    };

}   ///< namespace Global
}   ///< namespace Game
}   ///< namespace Steerstone

#define sQuestManager SteerStone::Game::Global::QuestManager::GetSingleton()