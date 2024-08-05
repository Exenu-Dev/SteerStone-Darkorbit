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
#include <variant>
#include <PCH/Precompiled.hpp>

namespace SteerStone { namespace Game { namespace Quest {
    class Quest;
} ///< namespace Quest
} ///< namespace Game
} ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Entity {

    class Player;

    struct Attribute
    {
		uint32 Id;
        std::variant<int32, std::string> Value;
	};

    struct QuestProgress
    {
        QuestProgress()
        {
            Id = 0;
            QuestConditionId    = 0;
            ConditionType       = ConditionType::CONDITION_TYPE_QUESTCASE;
            Progress            = 0;
            Completed           = false;
            Attributes.clear();
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        std::variant<int32, std::string> GetAttribute(uint32 const p_AttributeId) const
        {
			auto l_Attribute = Attributes.find(p_AttributeId);
            if (l_Attribute != Attributes.end())
            {
				return l_Attribute->second.Value;
			}

			return 0;
		}

        void UpdateProgress(std::variant<int32, std::string> const p_Progress)
        {
			Progress = p_Progress;
		}

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        uint32 Id;
        uint32 QuestConditionId;
        ConditionType ConditionType;
        std::variant<int32, std::string> Progress;
        bool Completed;
        std::unordered_map<uint32, Attribute> Attributes;
    };

    typedef std::vector<QuestProgress> QuestProgressVec;

    class Quest
    {
    public:
        friend class Player;

    public:
        /// Constructor
        /// @p_Player : The player that the quest is associated with
        Quest(Player* p_Player);
        /// Deconstructor
        ~Quest();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    public:
        /// Load Quests from DB
        void LoadFromDB();
        /// Save Quests to DB
        void SaveToDB();
        /// Load the quest progress
        void LoadProgress();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

        /// Cancel the quest
        /// This also deletes the quest from the database
        void Cancel();
        /// Finish the quest
        void Finish();
        /// Fail the quest
        void Fail();

    ///////////////////////////////////////////
    //            GETTERS/SETTERS
    ///////////////////////////////////////////
    public:
        uint32 GetId()                      const { return m_Id;        }
        Game::Quest::Quest* GetQuestData()  const { return m_Quest;     }
        QuestProgressVec& GetProgress()           { return m_Progress;  }

        std::vector<std::variant<int32, std::string>> GetConditionAttributes(uint32 const p_QuestProgressId);
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        uint32 m_Id;
        bool m_Completed;
        QuestProgressVec m_Progress;
        Game::Quest::Quest* m_Quest;
        Player* m_Player;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone