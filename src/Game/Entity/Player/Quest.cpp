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
#include <boost/algorithm/string.hpp>

#include "ZoneManager.hpp"
#include "Player.hpp"
#include "Quest/Quest.hpp"
#include "QuestManager.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    /// @p_Player : The player that the quest is associated with
    Quest::Quest(Player* p_Player) : m_Player(p_Player)
    {
        m_Id            = 0;
        m_Completed     = false;
        m_Quest         = nullptr;
    }
    /// Deconstructor
    Quest::~Quest()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    
    /// Load Quests from DB
    void Quest::LoadFromDB()
    {
        m_Progress.clear();

        LoadProgress();
    }
    /// Save Quests to DB
    void Quest::SaveToDB()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("UPDATE user_quests SET completed = ? WHERE id = ?");
        l_PreparedStatement->SetBool(0, m_Completed);
        l_PreparedStatement->SetUint32(1, m_Id);
        l_PreparedStatement->ExecuteStatement();

        for (auto l_QuestProgress : m_Progress)
		{
			Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
			l_PreparedStatement->PrepareStatement("UPDATE user_quest_progress SET progress = ?, completed = ? WHERE id = ?");
			l_PreparedStatement->SetInt32(0, std::get<int32>(l_QuestProgress.Progress));
			l_PreparedStatement->SetBool(1, l_QuestProgress.Completed);
			l_PreparedStatement->SetUint32(2, l_QuestProgress.Id);
            l_PreparedStatement->ExecuteStatement();
		}
    }
    /// Load the quest progress
    void Quest::LoadProgress()
    {
        std::vector<Game::Quest::QuestCondition> l_QuestConditions = m_Quest->GetConditions();

        auto GetConditionAttributes = [](uint32 p_QuestConditionId)->std::unordered_map<uint32, Attribute>
        {
            Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
            l_PreparedStatement->PrepareStatement("SELECT condition_attributes.attribute_keys_id, value "
                "FROM quest_condition_attributes INNER JOIN condition_attributes ON quest_condition_attributes.condition_attribute_id = condition_attributes.id "
                "WHERE quest_condition_id = ?"
            );

            l_PreparedStatement->SetUint32(0, p_QuestConditionId);

            std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

            std::unordered_map<uint32, Attribute> l_Attributes;

            if (l_PreparedResultSet)
            {
                do
                {
					Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

					Attribute l_Attribute;
                    l_Attribute.Id = l_Result[0].GetUInt32();

                    std::string l_Value = l_Result[1].GetString();

                    // Check if value is numeric, if so , convert to int32
                    if (std::all_of(l_Value.begin(), l_Value.end(), ::isdigit))
						l_Attribute.Value = std::stoi(l_Value);
                    else
						l_Attribute.Value = l_Value;

                    l_Attributes[l_Attribute.Id] = l_Attribute;

				} while (l_PreparedResultSet->GetNextRow());
			}

            return l_Attributes;
        };

        for (auto l_QuestCondition : l_QuestConditions)
        {
            Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
            l_PreparedStatement->PrepareStatement("SELECT user_quest_progress.id, progress, completed, quest_condition_attributes.quest_condition_id, quest_conditions.condition_type_id FROM user_quest_progress "
                "INNER JOIN quest_condition_attributes ON user_quest_progress.quest_condition_attributes_id = quest_condition_attributes.id "
                "INNER JOIN quest_conditions ON quest_condition_attributes.quest_condition_id = quest_conditions.id "
                "WHERE user_quest_id = ? AND quest_condition_id = ? ORDER BY quest_conditions.sort_order");
            l_PreparedStatement->SetUint32(0, m_Id);
            l_PreparedStatement->SetUint32(1, l_QuestCondition.Id);

            std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

            if (l_PreparedResultSet)
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                QuestProgress l_QuestProgress;

                l_QuestProgress.Id = l_Result[0].GetUInt32();
                l_QuestProgress.Progress = l_Result[1].GetUInt32();
                l_QuestProgress.Completed = l_Result[2].GetBool();
                l_QuestProgress.QuestConditionId = l_Result[3].GetUInt32();
                l_QuestProgress.ConditionType = static_cast<ConditionType>(l_Result[4].GetUInt32());
                l_QuestProgress.Attributes = GetConditionAttributes(l_QuestProgress.QuestConditionId);

                m_Progress.push_back(l_QuestProgress);
            }
		}
    }

    /// Cancel the quest
    /// This also deletes the quest from the database
    void Quest::Cancel()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("DELETE FROM user_quest_progress WHERE user_quest_id = ?");
        l_PreparedStatement->SetUint32(0, m_Id);
        l_PreparedStatement->ExecuteStatement();

        l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("DELETE FROM user_quests WHERE id = ?");
        l_PreparedStatement->SetUint32(0, m_Id);
        l_PreparedStatement->ExecuteStatement();
    }
    /// Finish the quest
    void Quest::Finish()
    {
        // Get our rewards
        std::vector<Game::Quest::QuestReward> l_Rewards = m_Quest->GetRewards();

        // Give the rewards to the player
        for (auto l_Reward : l_Rewards)
        {
            switch (l_Reward.Type)
            {
				case RewardType::REWARD_TYPE_CREDITS:
                {
					m_Player->RewardCredits(l_Reward.Amount);
				}
				break;
				case RewardType::REWARD_TYPE_URIDIUM:
                {
					m_Player->RewardUridium(l_Reward.Amount);
				}
				break;
				case RewardType::REWARD_TYPE_EXPERIENCE_POINTS:
                {
					m_Player->RewardExperience(l_Reward.Amount);
				}
				break;
                case RewardType::REWARD_TYPE_HONOUR:
                {
                    m_Player->RewardHonor(l_Reward.Amount);
                }
                break;
				default:
					LOG_ASSERT(false, "Quest", "Unknown reward type: %d", static_cast<uint8>(l_Reward.Type));
			}
		}

        // Mark the quest as completed
		m_Completed = true;

        // Save the quest to the database
        SaveToDB();
    }
    /// Fail the quest
    void Quest::Fail()
    {
        /// Just call the cancel as that will delete the quest from the database
        Cancel();
    }

    ///////////////////////////////////////////
    //            GETTERS/SETTERS
    ///////////////////////////////////////////

    std::vector<std::variant<int32, std::string>> Quest::GetConditionAttributes(uint32 const p_QuestProgressId)
    {
        std::vector<std::variant<int32, std::string>> l_Attributes;

        QuestProgress* l_QuestProgress = nullptr;

        // Find the quest progress, the m_Progress is a vector of QuestProgress
        for (auto& l_Itr : m_Progress)
        {
            if (l_Itr.Id == p_QuestProgressId)
            {
				l_QuestProgress = &l_Itr;
				break;
			}
		}

        if (l_QuestProgress)
        {
            switch (l_QuestProgress->ConditionType)
            {
                case ConditionType::CONDITION_TYPE_KILL_NPC:
                {
                    l_Attributes = {
                        l_QuestProgress->GetAttribute(AttributeKeys::ATTRIBUTE_KEY_NPC_ID),
                        l_QuestProgress->Progress,
                        l_QuestProgress->GetAttribute(AttributeKeys::ATTRIBUTE_KEY_COUNT_TARGET),
                    };
                }
                break;
                case ConditionType::CONDITION_TYPE_COLLECT:
{
					l_Attributes = {
                        l_QuestProgress->GetAttribute(AttributeKeys::ATTRIBUTE_KEY_ORE_ID),
                        l_QuestProgress->Progress,
                        l_QuestProgress->GetAttribute(AttributeKeys::ATTRIBUTE_KEY_COUNT_TARGET)
					};
				}
                break;
                case ConditionType::CONDITION_TYPE_VISIT_MAP:
                {
                    std::variant<int32, std::string> l_MapId = l_QuestProgress->GetAttribute(AttributeKeys::ATTRIBUTE_KEY_MAP_ID);

                    // Check if the map id is a string
                    if (auto l_MapIdString = std::get_if<std::string>(&l_MapId))
                    {
                        bool l_DynamicMap = boost::algorithm::contains(*l_MapIdString, "X-");

                        if (l_DynamicMap)
                        {
                            /// Replace X with the player company map id
                            switch (m_Player->GetCompany())
                            {
                            case Company::MMO:
                                boost::replace_all(*l_MapIdString, "X", "1");
                                break;
                            case Company::EARTH:
                                boost::replace_all(*l_MapIdString, "X", "2");
                                break;
                            case Company::VRU:
                                boost::replace_all(*l_MapIdString, "X", "3");
                                break;
                            }
                        }

                        // Convert the map string into the map id
                        l_MapId = sZoneManager->MapStringToId(*l_MapIdString);
					}

                    l_Attributes = {
						l_MapId,
                        l_QuestProgress->Progress,
                        0, // Visible state? 1 = visible, 0 = not visible
                        0, // Icon? 0 = info icon, 1 = square icon
					};
				}
                break;
                case ConditionType::CONDITION_TYPE_TAKE_DAMAGE:
                {
                    l_Attributes = {
                        l_QuestProgress->GetAttribute(AttributeKeys::ATTRIBUTE_KEY_DAMANGE_TYPE),
                        l_QuestProgress->Progress,
                        l_QuestProgress->GetAttribute(AttributeKeys::ATTRIBUTE_KEY_COUNT_TARGET)
					};
                }
                break;
                case ConditionType::CONDITION_TYPE_TIMER:
                {
                    l_Attributes = {
                        (std::get<int32>(l_QuestProgress->GetAttribute(AttributeKeys::ATTRIBUTE_KEY_TIME_TARGET)) - std::get<int32>(l_QuestProgress->Progress)) * 1000,
                    };
                }
                break;
                default:
                    LOG_ASSERT(false, "Quest", "Unknown condition type: %d", l_QuestProgress->ConditionType);
            }
		}

		return l_Attributes;
    }


}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone   