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

#include "QuestPackets.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets { namespace Quest {

    /// SERVER_PACKET_QUEST_HUD
    PacketBuffer const* QuestHud::Write()
    {
        std::string l_Type = "";

        switch (Type)
        {
            case QuestHudTypes::QUEST_HUD_TYPE_INI:
				l_Type = "ini";
				break;
            case QuestHudTypes::QUEST_HUD_TYPE_UPDATE:
                l_Type = "upd";
                break;
            case QuestHudTypes::QUEST_HUD__TYPE_PRIVILEGE:
                l_Type = "p";
				break;
            case QuestHudTypes::QUEST_HUD_TYPE_CANCEL:
				l_Type = "c";
				break;
            case QuestHudTypes::QUEST_HUD_TYPE_FINISH:
				l_Type = "a";
				break;
            case QuestHudTypes::QUEST_HUD_TYPE_FAIL:
                l_Type = "f";
                break;
            default:
                LOG_ASSERT(false, "Packets", "Invalid QuestHudType");
        }

        m_Buffer.AppendChar(l_Type.c_str());

        if (Type == QuestHudTypes::QUEST_HUD_TYPE_INI)
        {
            std::string l_QuestFormat = "";

            for (auto& l_Case : Cases)
            {
                l_QuestFormat.append("case");
                l_QuestFormat.append(",");
                l_QuestFormat.append(std::to_string(l_Case.Id));
                l_QuestFormat.append(",");
                l_QuestFormat.append(l_Case.Active ? "1" : "0");
                l_QuestFormat.append(",");
                l_QuestFormat.append(l_Case.Mandatory ? "1" : "0");
                l_QuestFormat.append(",");
                l_QuestFormat.append(l_Case.Ordered ? "1" : "0");
                l_QuestFormat.append(",");

                for (auto& l_Condition = l_Case.Conditions.begin(); l_Condition != l_Case.Conditions.end(); l_Condition++)
                {
                    l_QuestFormat.append("{");
                    l_QuestFormat.append("cond");
                    l_QuestFormat.append(",");
                    l_QuestFormat.append(std::to_string(l_Condition->Id));
                    l_QuestFormat.append(",");
                    l_QuestFormat.append(std::to_string(l_Condition->ConditionType));

                    if (!l_Case.Ordered)
                    {
                        l_Condition->Values.push_back(1);

                        if (l_Condition->Completed)
                            l_Condition->Values.push_back(1);
                    }
                    else
                    {
                        if (l_Condition->Completed)
                        {
                            l_Condition->Values.push_back(1);
                            l_Condition->Values.push_back(1);
                        }
                    }

                    for (const auto& l_Itr : l_Condition->Values)
                    {
                        l_QuestFormat.append(",");

                        if (auto l_Value = std::get_if<std::string>(&l_Itr))
                            l_QuestFormat.append(l_Value->c_str());
                        else if (auto l_Value = std::get_if<int32>(&l_Itr))
                            l_QuestFormat.append(std::to_string(*l_Value));
                    }

                    if (l_Condition->Values.size() < 5)
                    {
                        // For some odd reason, the client needs 5 values or more for each condition
                        const char* l_EmptyParams[7] = {
                            "a",
                            "b",
                            "c",
                            "d",
                            "e",
                            "f",
                            "g"
                        }; 

                        int32 l_RemainingParams = 5 - l_Condition->Values.size();

                        /// We need to fill the rest of the values with 0
                        for (size_t i = 0; i <= l_RemainingParams; i++)
                        {
                            l_QuestFormat.append(",");
                            l_QuestFormat.append(l_EmptyParams[i]);
                        }
                    }


                    l_QuestFormat.append("}");
                }

                l_QuestFormat.append(",");
            }

            // Remove last comma
            l_QuestFormat.pop_back();

            m_Buffer.AppendChar(l_QuestFormat.c_str());
		}
        else if (Type == QuestHudTypes::QUEST_HUD_TYPE_UPDATE)
        {
            m_Buffer.AppendUInt32(Update.QuestId);
            m_Buffer.AppendChar("i");
            m_Buffer.AppendUInt32(Update.Id);

            if (auto l_Value = std::get_if<int32>(&Update.Value))
				m_Buffer.AppendInt32(*l_Value);
			else if (auto l_Value = std::get_if<std::string>(&Update.Value))
				m_Buffer.AppendChar(l_Value->c_str());

            m_Buffer.AppendBool(Update.Visibility);
            m_Buffer.AppendBool(Update.ActivityState);
        }
        else if (Type == QuestHudTypes::QUEST_HUD__TYPE_PRIVILEGE)
		{
			m_Buffer.AppendUInt32(Privilege.QuestId);
		}
        else if (Type == QuestHudTypes::QUEST_HUD_TYPE_CANCEL)
        {
            m_Buffer.AppendUInt32(Cancel.QuestId);
        }
        else if (Type == QuestHudTypes::QUEST_HUD_TYPE_FINISH)
        {
			m_Buffer.AppendUInt32(Finish.QuestId);
			m_Buffer.AppendUInt32(0);
            // m_Buffer.AppendChar("sta_quest");
            // m_Buffer.AppendChar("q2_condition_KILL_NPC_challenge");
		}
        else if (Type == QuestHudTypes::QUEST_HUD_TYPE_FAIL)
        {
            m_Buffer.AppendUInt32(Finish.QuestId);
        }

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

}   ///< namespace Quest
}   ///< namespace Packets
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone