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
#include "Grid.hpp"
#include "GameFlags.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Mob Pool
    template <typename T> class Pool
    {
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
    public:
        /// Add to pool
        /// @p_Grid : Grid Target
        /// @p_Object : Object being added
        void Add(Grid* p_Grid, T p_Object)
        {
            m_Pool[p_Grid].push_back(p_Object);
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        std::map<Grid*, std::vector<T>> m_Pool;
    };

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone