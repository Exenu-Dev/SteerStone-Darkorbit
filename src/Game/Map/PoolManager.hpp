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
#include "Core/Core.hpp"
#include "Pool.hpp"

namespace SteerStone { namespace Game { namespace Map {

    class Base;

    /// Pool Manager
    class PoolManager
    {
        DISALLOW_COPY_AND_ASSIGN(PoolManager);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        /// @p_Map : Reference of map
        PoolManager(Base* p_Map);
        /// Deconstructor
        ~PoolManager();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
    
    public:
        /// Initialize Pool for map
        void Initialize();

        /// Update Pool
        /// @p_Diff : Execution Time
        void Update(uint32 const p_Diff);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
    
    private:
        /// Load Mobs into pool
        void InitializeMobs();

    private:
        Pool<Entity::Mob*> m_MobPool;
        Base* m_Map;
    };

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone