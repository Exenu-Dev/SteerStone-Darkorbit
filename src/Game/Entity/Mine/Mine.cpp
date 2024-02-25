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
    Mine::Mine(Player* p_Owner)
    {
        SetType(Type::OBJECT_TYPE_MINE);
        SetGUID(ObjectGUID(GUIDType::Mine));

        m_Timer.SetInterval(MINE_MCB_TIMER);

        m_Owner = p_Owner;
    }
    /// Deconstructor
    Mine::~Mine()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////
    //                GENERAL
    ///////////////////////////////////////////

    void Mine::Update(uint32 const p_Diff)
    {
        if (CanExplode(p_Diff))
            Explode();
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
                if (l_Itr.second->IsPlayer())
                    DealDamage(l_Itr.second->ToUnit(), 1000, false);
			}
        }

        GetGrid()->SendPacketIfInSurrounding(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_MINE_MIN, { GetObjectGUID().GetCounter() }), this);
        SetScheduleForDelete(true);
	}

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone