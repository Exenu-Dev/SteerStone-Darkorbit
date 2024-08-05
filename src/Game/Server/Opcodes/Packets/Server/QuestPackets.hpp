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
#include "Socket.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets { namespace Quest {

    enum QuestHudTypes
    {
        QUEST_HUD_TYPE_NONE             = 0,
        QUEST_HUD_TYPE_INI              = 1,
        QUEST_HUD_TYPE_UPDATE           = 2,
        QUEST_HUD__TYPE_PRIVILEGE       = 3,
        QUEST_HUD_TYPE_CANCEL           = 4,
        QUEST_HUD_TYPE_FAIL			    = 5,
        QUEST_HUD_TYPE_FINISH           = 6,
    };

    struct Condition
    {
        uint32 Id;
        uint32 ConditionType;
        bool Completed;
        std::vector<std::variant<int32, std::string>> Values;
    };

    struct Case
    {
        uint32 Id;
        bool Active;
        bool Mandatory;
        bool Ordered;
        std::vector<Condition> Conditions;
    };

    struct UpdateCondition
    {
        uint32 QuestId;
		uint32 Id;
        std::variant<int32, std::string> Value;
        bool Visibility;
        bool ActivityState;
	};

    struct UpdatePrivilege
	{
		uint32 QuestId;
	};

    struct CancelQuest
    {
		uint32 QuestId;
	};

    struct QuestFinish
    {
        uint32 QuestId;
    };

    struct QuestFail
    {
		uint32 QuestId;
	};

    /// SERVER_PACKET_QUEST_HUD packet builder
    class QuestHud final : public ServerPacket
    {
    public:
        /// Constructor 
        QuestHud() : ServerPacket(ServerOpCodes::SERVER_PACKET_QUEST_HUD) 
        {
            Type        = QuestHudTypes::QUEST_HUD_TYPE_NONE;
            Cases       = std::vector<Case>();
            Update      = UpdateCondition();
            Privilege   = UpdatePrivilege();
            Cancel      = CancelQuest();
            Finish      = QuestFinish();
            Fail        = QuestFail();
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        QuestHudTypes Type;
        std::vector<Case> Cases;
        UpdateCondition Update;
        UpdatePrivilege Privilege;
        CancelQuest Cancel;
        QuestFinish Finish;
        QuestFail Fail;
    };

}   ///< namespace Quest
}   ///< namespace Packets
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone