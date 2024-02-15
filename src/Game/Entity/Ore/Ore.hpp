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
#include "Unit.hpp"
#include "GameFlags.hpp"
#include "Diagnostic/DiaIntervalTimer.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Ore
    class Ore : public Unit
    {
    public:

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        Ore(OreTypes p_OreType);
        /// Deconstructor
        ~Ore();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////
        //                GENERAL
        ///////////////////////////////////////////

        /// Reward Credit on loot
        /// @p_Player : Player being rewarded
        void RewardCredit(Entity::Player* p_Player);


        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
        OreTypes GetOreType()       const { return m_Type;          }
        Resource GetResourceType()  const { return m_ResourceType;  }

    public:
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        OreTypes m_Type;
        Resource m_ResourceType;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone