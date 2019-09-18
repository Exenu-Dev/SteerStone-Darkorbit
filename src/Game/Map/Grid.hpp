/*
* Liam Ashdown
* Copyright (C) 2019
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
#include <mutex>
#include "Core/Core.hpp"
#include "GridDefines.hpp"
#include "Diagnostic/DiaIntervalTimer.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    class Object;

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Map {

    enum class State
    {
        Idle,       ///< Grid is in Idle mode, there's no players
        Active,     ///< Grid is Active, there are players
        InActive    ///< Grid is turned off regardless if there's players
    };


    class Entity::Object;

    /// Grid
    class Grid
    {
        DISALLOW_COPY_AND_ASSIGN(Grid);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
    public:
        /// Constructor
        /// @p_GridX : Grid X
        /// @p_GridY : Grid Y
        Grid(uint32 const p_GridX, uint32 const p_GridY);
        /// Deconstructor
        ~Grid();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Add Object to Grid
        /// @p_Object : Object being added
        void Add(Entity::Object* p_Object);
        /// Remove Object from Grid
        /// @p_Object : Object being removed
        void Remove(Entity::Object* p_Object);

        /// Get State of Grid
        State GetState() const;

        /// Check if a player is inside the grid
        /// @p_Diff : Execution Time
        void CheckForPlayer(uint32 const p_Diff);

        /// Update Grid
        /// @p_Diff : Execution Time
        bool Update(uint32 const p_Diff);


        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        std::unordered_map<uint32, Entity::Object*> m_Objects;  ///< Objects in Grid
        State m_State;                                          ///< Grid State
        uint32 m_GridX;                                         ///< Grid Index X
        uint32 m_GridY;                                         ///< Grid Index Y
        Core::Diagnostic::IntervalTimer m_IntervalCheckPlayer;  ///< Interval Timer
        std::mutex m_Mutex;                                     ///< Global Mutex
    };

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone