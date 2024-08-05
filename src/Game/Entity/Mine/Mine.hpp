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

#define MINE_MCB_TIMER 5000

namespace SteerStone { namespace Game { namespace Entity {

    /// Mine
    class Mine : public Unit
    {
    public:

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        Mine(Player * p_Owner, MinesType const p_MineType);
        /// Deconstructor
        ~Mine();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////
        //                GENERAL
        ///////////////////////////////////////////

        /// Update
        /// @p_Diff : Execution Time
        bool Update(uint32 const p_Diff) override;

        /// Check if mine can explode
        /// @p_Diff : Execution Time
        bool CanExplode(uint32 const p_Diff);

        /// Explode the Mine
        void Explode();

        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
    public:
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        Core::Diagnostic::IntervalTimer m_Timer;
        MinesType m_MineType;
        Player* m_Owner;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone