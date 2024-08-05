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

#include "Database/DatabaseTypes.hpp"
#include "Quest/Quest.hpp"

namespace SteerStone { namespace Game { namespace Quest {
    
    /// Constructor
    Quest::Quest()
    {

    }
    /// Deconstructor
    Quest::~Quest()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void Quest::LoadFromDB()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT id, condition_type_id, sort_order FROM quest_conditions WHERE quest_template_id = ? ORDER BY sort_order");
        l_PreparedStatement->SetUint32(0, m_Id);

        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        // create a lambda function to get the condition attributes
        auto GetConditionAttributes = [](uint32 p_ConditionId) -> std::vector<QuestConditionAttribute>
        {
			std::vector<QuestConditionAttribute> l_ConditionAttributes;

			Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
			l_PreparedStatement->PrepareStatement("SELECT quest_condition_attributes.id, condition_attribute_id, value, condition_attributes.attribute_keys_id FROM quest_condition_attributes INNER JOIN condition_attributes ON quest_condition_attributes.condition_attribute_id = condition_attributes.id WHERE quest_condition_id = ?");
			l_PreparedStatement->SetUint32(0, p_ConditionId);

			std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

            if (l_PreparedResultSet)
            {
                do
                {
					Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

					QuestConditionAttribute l_ConditionAttribute;
					l_ConditionAttribute.Id             = l_Result[0].GetUInt32();
					l_ConditionAttribute.ConditionId    = l_Result[1].GetUInt32();
					l_ConditionAttribute.Value          = l_Result[2].GetString();
                    l_ConditionAttribute.AttributeKeyId = l_Result[3].GetUInt32();

					l_ConditionAttributes.push_back(l_ConditionAttribute);
				} while (l_PreparedResultSet->GetNextRow());
			}

			return l_ConditionAttributes;
		};

        if (l_PreparedResultSet)
        {
            do
            {
				Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                QuestCondition l_Condition;
                l_Condition.Id              = l_Result[0].GetUInt32();
                l_Condition.ConditionTypeId = l_Result[1].GetUInt32();
                l_Condition.Attributes	    = GetConditionAttributes(l_Condition.Id);
                l_Condition.Order           = l_Result[2].GetUInt8();

                m_Conditions.push_back(l_Condition);

            } while (l_PreparedResultSet->GetNextRow());
		}

		LoadRewards();
    }

    void Quest::LoadRewards()
    {
        std::vector<QuestReward> l_Rewards;

        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT reward_type_id, amount FROM quest_rewards WHERE quest_template_id = ?");
        l_PreparedStatement->SetUint32(0, m_Id);

        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                QuestReward l_Reward;
                l_Reward.Type   = static_cast<RewardType>(l_Result[0].GetUInt32());
                l_Reward.Amount = l_Result[1].GetUInt32();

                l_Rewards.push_back(l_Reward);

            } while (l_PreparedResultSet->GetNextRow());
        }

        m_Rewards = l_Rewards;
    }

}   ///< namespace Quest
}   ///< namespace Game
}   ///< namespace Steerstone