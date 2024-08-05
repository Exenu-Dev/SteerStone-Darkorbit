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

#include "Utility/UtilRandom.hpp"

#include "Server/Opcodes/Packets/Server/MiscPackets.hpp"
#include "Server/Opcodes/Packets/Server/LoginPackets.hpp"
#include "Server/Opcodes/Packets/Server/ShipPackets.hpp"
#include "Mine.hpp"
#include "Player.hpp"
#include "Utility/UtilMaths.hpp"
#include "ZoneManager.hpp"
#include <array> 

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Mine::Mine(Player* p_Owner, MinesType const p_MineType)
    {
        SetType(Type::OBJECT_TYPE_MINE);
        SetGUID(ObjectGUID(GUIDType::Mine));

        if (p_MineType == MinesType::MINE)
            m_Timer.SetInterval(MINE_MCB_TIMER);
		else if (p_MineType == MinesType::SMART_BOMB)
            /// Denotates the moment the smart bomb is created
			m_Timer.SetInterval(0);

        m_MineType = p_MineType;
        m_Owner = p_Owner;
    }
    /// Deconstructor
    Mine::~Mine()
    {
        Unit::CleanupsBeforeDelete();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////
    //                GENERAL
    ///////////////////////////////////////////

    bool Mine::Update(uint32 const p_Diff)
    {
        Object::Update(p_Diff);

        if (CanExplode(p_Diff))
            Explode();

        return true;
    }

    /// Check if mine can explode
    /// @p_Diff : Execution Time
    bool Mine::CanExplode(uint32 const p_Diff)
    {
        m_Timer.Update(p_Diff);

        return m_Timer.Passed();
	}

    void Mine::Explode()
    {
        for (auto l_Itr : GetGrid()->GetObjects())
        {
            if (Core::Utils::IsInCircleRadius(GetSpline()->GetPositionX(), GetSpline()->GetPositionY(), l_Itr.second->GetSpline()->GetPositionX(), l_Itr.second->GetSpline()->GetPositionY(), 200))
            {
                if (l_Itr.second->IsPlayer() && m_Owner != l_Itr.second)
                {
                    uint32 l_Damage = 1000;

                    if (m_MineType == MinesType::SMART_BOMB)
                    {
                        /// 20% damage of players current health
                        l_Damage = Core::Utils::CalculatePercentage(l_Itr.second->ToPlayer()->GetHitPoints(), 20);
                    }

                    DealDamage(l_Itr.second->ToUnit(), l_Damage, false);
                }
			}
        }

        Server::Packets::Misc::Reward l_Reward;

        // Yeah this is weird, the packet to detonate the mine is in the reward packet
        switch (m_MineType)
        {
            case MinesType::MINE:
            {
                m_Owner->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_MINE, { 1 }));
            }
            break;
            case MinesType::SMART_BOMB:
            {
                m_Owner->GetGrid()->SendPacketIfInSurrounding(Server::Packets::Misc::Info().Write(
                    Server::Packets::Misc::InfoType::INFO_TYPE_SMART_MINE,
                    {
                        m_Owner->GetObjectGUID().GetCounter()
                    }
                ), this, true);
            }
            break;
        }

        SetScheduleForDelete(true);
	}

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone