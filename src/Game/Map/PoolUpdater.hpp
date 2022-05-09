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
#include "GameFlags.hpp"

#include "Grid.hpp"

namespace SteerStone { namespace Game { namespace Map {

    class PoolUpdater
    {
    public:
        /// Constructor
        PoolUpdater()
        {
        }
        /// Deconstructor
        virtual ~PoolUpdater()
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Update the pool
        /// @p_Diff : Execution Time
        virtual void Update(uint32 const p_Diff) = 0;

        /// Add to Pool
        /// @p_Object : Object being added
        virtual void Add(Entity::Object* p_Object) = 0;

        /// Remove from Pool
        /// @p_Object : Object being removed
        virtual void Remove(Entity::Object* p_Object) = 0;

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
    };

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone