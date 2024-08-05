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
#include "GameFlags.hpp"

namespace SteerStone { namespace Game { namespace Global {
	class QuestManager;
}   ///< namespace Global
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Quest {

	struct QuestReward
	{
		QuestReward()
		{
			Type			= RewardType::REWARD_TYPE_NONE;
			Amount			= 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		
		RewardType Type;
		int32 Amount;
	};
	
	struct QuestConditionAttribute
	{
		QuestConditionAttribute()
		{
			Id				= 0;
			ConditionId		= 0;
			Value			= "";
			AttributeKeyId	= 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		uint32 Id;
		uint32 ConditionId;
		std::string Value;
		uint32 AttributeKeyId;
	};

	struct QuestCondition
	{
		QuestCondition()
		{
			Id				= 0;
			ConditionTypeId = 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		uint32 Id;
		std::vector<QuestConditionAttribute> Attributes;
		uint32 ConditionTypeId;
		uint8 Order;
	};

	class Quest
	{
	public:
		friend class Game::Global::QuestManager;

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

	public:
		/// Constructor
		Quest();
		/// Deconstructor
		~Quest();

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
	public:
		void LoadFromDB();

		uint32 GetId()					const { return m_Id;			}
		std::string GetTitle()			const { return m_Title;			}
		std::string GetDescription()	const { return m_Description;	}
		std::string GetOverview()		const { return m_Overview;		}
		bool IsOrder()					const { return m_Order;			}

		std::vector<QuestCondition>		const GetConditions() const { return m_Conditions; }
		std::vector<QuestReward> const GetRewards() const { return m_Rewards; }

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

	private:
		void LoadRewards();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	private:
		uint32 m_Id;
		std::string m_Title;
		std::string m_Description;
		std::string m_Overview;
		bool m_Order;

		std::vector<QuestCondition> m_Conditions;
		std::vector<QuestReward> m_Rewards;
	};
}   ///< namespace Quest
}   ///< namespace Game
}   ///< namespace SteerStone